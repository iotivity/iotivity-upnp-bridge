//******************************************************************
//
// Copyright 2017 Intel Corporation All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <iostream>

#include <ocrandom.h>
#include <ocstack.h>
#include <octypes.h>
#include <oic_string.h>
#include <oic_malloc.h>
#include <pluginServer.h>

#include <ConcurrentIotivityUtils.h>

#include "UpnpBridgeDevice.h"
#include "UpnpException.h"
#include "UpnpInternal.h"

using namespace std;
using namespace OC::Bridging;

static const string MODULE = "UpnpBridgeDevice";

static const string BRIDGE_RESOURCE_TYPE = "oic.d.bridge";
static const string SECUREMODE_RESOURCE_TYPE = "oic.r.securemode";

const uint SECUREMODE_CALLBACK = 0;
const uint COLLECTION_CALLBACK = 1;

static const string SECUREMODE_PROPERTY_KEY = "secureMode";
static const string SECUREMODE_RESOURCE_URI = "/securemode";
static const string BRIDGE_RESOURCE_URI_PREFIX = "/upnp-bridge/";

static string s_bridgeUri = BRIDGE_RESOURCE_URI_PREFIX + "0";
static vector<_link> s_links;

static UpnpManager *s_upnpManager;

OCStackResult createResource(const string uri, const string resourceTypeName,
        const char *resourceInterfaceName, OCEntityHandler resourceEntityHandler,
        void* callbackParam, uint8_t resourceProperties)
{
    OCStackResult result = OC_STACK_ERROR;
    OCResourceHandle handle = OCGetResourceHandleAtUri(uri.c_str());

    if (!handle)
    {
        result = OCCreateResource(&handle, resourceTypeName.c_str(), resourceInterfaceName,
                uri.c_str(), resourceEntityHandler, callbackParam, resourceProperties);
        if (result == OC_STACK_OK)
        {
            DEBUG_PRINT("Created resource " << uri);
        }
        else
        {
            DEBUG_PRINT("Failed to create resource " << uri);
        }
    }
    else
    {
        DEBUG_PRINT("Not creating resource " << uri << " (already exists)");
        result = OC_STACK_OK;
    }

    return result;
}

UpnpBridgeDevice::UpnpBridgeDevice()
{
    OCResourceHandle handle = OCGetResourceHandleAtUri(OC_RSRVD_DEVICE_URI);
    if (!handle)
    {
        DEBUG_PRINT("OCGetResourceHandleAtUri(" << OC_RSRVD_DEVICE_URI << ") failed");
    }
    DEBUG_PRINT("OCGetResourceHandleAtUri(" << OC_RSRVD_DEVICE_URI << ") " << handle);

    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);

    // Generate a UUID for the Protocol Independent ID
    uint8_t uuid[UUID_SIZE] = {0};
    if (!OCGenerateUuid(uuid))
    {
        // TODO: throw error on failure ?
        DEBUG_PRINT("Generate UUID for Upnp Bridge Device failed");
    }
    char uuidString[UUID_STRING_SIZE] = {0};
    if (!OCConvertUuidToString(uuid, uuidString)) {
        // TODO: throw error on failure ?
        DEBUG_PRINT("Convert UUID to string for Upnp Bridge Device failed");
    }

    s_bridgeUri = BRIDGE_RESOURCE_URI_PREFIX + string(uuidString);

    OCStackResult result = OCBindResourceTypeToResource(handle, BRIDGE_RESOURCE_TYPE.c_str());
    if (result != OC_STACK_OK)
    {
        DEBUG_PRINT("OCBindResourceTypeToResource() = " << result);
    }

    handle = OCGetResourceHandleAtUri(OC_RSRVD_WELL_KNOWN_URI);
    if (!handle)
    {
        DEBUG_PRINT("OCGetResourceHandleAtUri(" << OC_RSRVD_DEVICE_URI << ") failed");
    }
    result = OCSetResourceProperties(handle, OC_DISCOVERABLE | OC_OBSERVABLE);
    if (result != OC_STACK_OK)
    {
        DEBUG_PRINT("OCSetResourceProperties() = " << result);
    }

    result = createResource(SECUREMODE_RESOURCE_URI, SECUREMODE_RESOURCE_TYPE.c_str(),
            OC_RSRVD_INTERFACE_READ, entityHandler, (void *) SECUREMODE_CALLBACK, resourceProperties);
    if (result != OC_STACK_OK)
    {
        DEBUG_PRINT("CreateResource() = " << result);
    }

    result = createResource(s_bridgeUri, BRIDGE_RESOURCE_TYPE.c_str(),
            OC_RSRVD_INTERFACE_READ, entityHandler, (void *) COLLECTION_CALLBACK, resourceProperties);
    if (result == OC_STACK_OK)
    {
        OCResourceHandle bridgeHandle = OCGetResourceHandleAtUri(s_bridgeUri.c_str());
        result = OCBindResourceTypeToResource(bridgeHandle, OC_RSRVD_RESOURCE_TYPE_COLLECTION);
        if (result != OC_STACK_OK)
        {
            DEBUG_PRINT("Failed to bind collection resource type to " << bridgeHandle << " " << result);
        }

        result = OCBindResourceInterfaceToResource(bridgeHandle, OC_RSRVD_INTERFACE_LL);
        if (result != OC_STACK_OK)
        {
            DEBUG_PRINT("Failed to bind ll resource interface to " << bridgeHandle << " " << result);
        }
    }
    else {
        DEBUG_PRINT("CreateResource() = " << result);
    }

    DEBUG_PRINT("di=" << OCGetServerInstanceIDString());
}

UpnpBridgeDevice::~UpnpBridgeDevice()
{
    OCStackResult result = OC::Bridging::ConcurrentIotivityUtils::queueDeleteResource(s_bridgeUri);
    DEBUG_PRINT("Plugin stop queueDeleteResource() result = " << result);
}

void UpnpBridgeDevice::setUpnpManager(UpnpManager *upnpManager)
{
    s_upnpManager = upnpManager;
}

void UpnpBridgeDevice::addResource(UpnpResource::Ptr pResource)
{
    _link singleLink;

    singleLink.href = pResource->m_uri;
    singleLink.rel = LINK_REL_CONTAINS;
    singleLink.rt = pResource->getResourceType();

    s_links.push_back(singleLink);
}

void UpnpBridgeDevice::removeResource(string uri)
{
    for(vector<_link>::iterator itor = s_links.begin(); itor != s_links.end(); ++itor) {
        if (itor->href == uri) {
            DEBUG_PRINT("Removing link for " << uri);
            s_links.erase(itor);
            break;
        }
    }
}

// Entity handler
OCEntityHandlerResult UpnpBridgeDevice::entityHandler(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void *callback)
{
    DEBUG_PRINT("");
    uintptr_t callbackParamResourceType = (uintptr_t)callback;
    string resourceType;

    if (callbackParamResourceType == SECUREMODE_CALLBACK)
    {
        resourceType = SECUREMODE_RESOURCE_TYPE;
    }
    else if (callbackParamResourceType == COLLECTION_CALLBACK)
    {
        resourceType = OC_RSRVD_RESOURCE_TYPE_COLLECTION;
    }

    if (! resourceType.empty()) {
        return handleEntityHandlerRequests(flag, entityHandlerRequest, resourceType);
    }
    else {
        ERROR_PRINT("Unknown callback type " << callbackParamResourceType);
    }

    return OC_EH_ERROR;
}

OCEntityHandlerResult UpnpBridgeDevice::handleEntityHandlerRequests(OCEntityHandlerFlag,
    OCEntityHandlerRequest *entityHandlerRequest, string resourceType)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCRepPayload *responsePayload = NULL;
    OCRepPayload *payload = OCRepPayloadCreate();

    try
    {
        if ((entityHandlerRequest == NULL))
        {
            throw "Entity handler received a null entity request context" ;
        }

        string uri = OCGetResourceUri(entityHandlerRequest->resource);
        DEBUG_PRINT("URI from resource " << uri);

        char *interfaceQuery = NULL;
        char *resourceTypeQuery = NULL;
        char *dupQuery = OICStrdup(entityHandlerRequest->query);
        if (dupQuery)
        {
            MPMExtractFiltersFromQuery(dupQuery, &interfaceQuery, &resourceTypeQuery);
        }

        switch (entityHandlerRequest->method)
        {
            case OC_REST_GET:
                DEBUG_PRINT(" GET Request for: " << uri);
                ehResult = processGetRequest(uri, resourceType, payload);
                break;

            default:
                DEBUG_PRINT("UnSupported Method [" << entityHandlerRequest->method << "] Received");
                ConcurrentIotivityUtils::respondToRequestWithError(entityHandlerRequest, "Unsupported Method", OC_EH_METHOD_NOT_ALLOWED);
                 return OC_EH_OK;
        }

        responsePayload = getCommonPayload(uri.c_str(), interfaceQuery, resourceType, payload);
        ConcurrentIotivityUtils::respondToRequest(entityHandlerRequest, responsePayload, ehResult);
        OICFree(dupQuery);
    }
    catch (const char *errorMessage)
    {
        DEBUG_PRINT("Error - " << errorMessage);
        ConcurrentIotivityUtils::respondToRequestWithError(entityHandlerRequest, errorMessage, OC_EH_ERROR);
        ehResult = OC_EH_OK;
    }

    OCRepPayloadDestroy(responsePayload);
    return ehResult;
}

OCEntityHandlerResult UpnpBridgeDevice::processGetRequest(string uri, string resType, OCRepPayload *payload)
{
    if (payload == NULL)
    {
        throw "payload is null";
    }

    if (SECUREMODE_RESOURCE_TYPE == resType)
    {
        bool secureMode = false;
        if (!OCRepPayloadSetPropBool(payload, SECUREMODE_PROPERTY_KEY.c_str(), secureMode))
        {
            string message = "Failed to set '" + SECUREMODE_PROPERTY_KEY + "' in payload";
            throw message;
        }
        DEBUG_PRINT(uri << " -- " << SECUREMODE_PROPERTY_KEY << ": " << (secureMode ? "true" : "false"));
    }
    else if (OC_RSRVD_RESOURCE_TYPE_COLLECTION == resType)
    {
        DEBUG_PRINT("Setting bridge device links");
        const OCRepPayload *links[s_upnpManager->m_devices.size()];
        size_t dimensions[MAX_REP_ARRAY_DEPTH] = {s_upnpManager->m_devices.size(), 0, 0};
        int linksIndex = 0;
        for (const auto& device : s_upnpManager->m_devices) {
            DEBUG_PRINT(OC_RSRVD_LINKS << "[" << linksIndex << "]");
            DEBUG_PRINT("\t" << OC_RSRVD_HREF << "=" << device.second->m_uri);
            DEBUG_PRINT("\t" << OC_RSRVD_RESOURCE_TYPE << "=" << device.second->m_resourceType);
            OCRepPayload *linkPayload = OCRepPayloadCreate();
            OCRepPayloadSetPropString(linkPayload, OC_RSRVD_HREF, device.second->m_uri.c_str());
            OCRepPayloadSetPropString(linkPayload, OC_RSRVD_REL, LINK_REL_HOSTS.c_str());
            OCRepPayloadSetPropString(linkPayload, OC_RSRVD_RESOURCE_TYPE, device.second->m_resourceType.c_str());
            links[linksIndex] = linkPayload;
            ++linksIndex;
        }
        OCRepPayloadSetPropObjectArray(payload, OC_RSRVD_LINKS, links, dimensions);
    }
    else
    {
        throw "Failed due to unknown resource type";
    }

    return OC_EH_OK;
}

OCRepPayload* UpnpBridgeDevice::getCommonPayload(const char *uri, char *interfaceQuery, string resourceType,
        OCRepPayload *payload)
{
    if (!OCRepPayloadSetUri(payload, uri))
    {
        throw "Failed to set uri in the payload";
    }

    if (!OCRepPayloadAddResourceType(payload, resourceType.c_str()))
    {
        throw "Failed to set resource type in the payload" ;
    }

    DEBUG_PRINT("Checking against if: " << interfaceQuery);

    // If the interface filter is explicitly oic.if.baseline, include all properties.
    if (interfaceQuery && string(interfaceQuery) == string(OC_RSRVD_INTERFACE_DEFAULT))
    {
        if (!OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_READ))
        {
            throw "Failed to set read interface";
        }

        if (!OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_DEFAULT))
        {
            throw "Failed to set baseline interface" ;
        }

        if (!OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_LL))
        {
            throw "Failed to set ll interface";
        }
    }

    return payload;
}
