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

static string s_bridgeUri = "/oic/upnp-bridge/0";
static vector<_link> s_links;

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
//            result = OCBindResourceTypeToResource(handle, "oic.d.virtual");
//            if (result == OC_STACK_OK)
//            {
//                DEBUG_PRINT("Bound virtual resource type to " << uri);
//            }
//            else
//            {
//                DEBUG_PRINT("Failed to bind virtual resource type to " << uri);
//            }
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

    s_bridgeUri = "/oic/upnp-bridge/" + string(uuidString);
//    OCStackResult result = ConcurrentIotivityUtils::queueCreateResource(s_bridgeUri,
//                    BRIDGE_RESOURCE_TYPE, OC_RSRVD_INTERFACE_READ, entityHandler,
//                    (void *) 0, resourceProperties);
//    DEBUG_PRINT("Plugin start queueCreateResource() result = " << result);

    OCStackResult result = OCBindResourceTypeToResource(handle, "oic.d.bridge");
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
    result = createResource("/securemode", "oic.r.securemode",
                            OC_RSRVD_INTERFACE_READ,
                            entityHandler, (void *) 0, resourceProperties);
    if (result != OC_STACK_OK)
    {
        DEBUG_PRINT("CreateResource() = " << result);
    }

    DEBUG_PRINT("di=" << OCGetServerInstanceIDString());
}

UpnpBridgeDevice::~UpnpBridgeDevice()
{
    OCStackResult result = OC::Bridging::ConcurrentIotivityUtils::queueDeleteResource(s_bridgeUri);
    DEBUG_PRINT("Plugin stop queueDeleteResource() result = " << result);
}

void UpnpBridgeDevice::addResource(UpnpResource::Ptr pResource)
{
    _link singleLink;

    singleLink.href = pResource->m_uri;
    singleLink.rel = "contains";
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
    uintptr_t callbackParamResourceType = (uintptr_t)callback;
    (void)callbackParamResourceType;
    return handleEntityHandlerRequests(flag, entityHandlerRequest, /*BRIDGE_RESOURCE_TYPE*/"oic.r.securemode");
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

    if (/*BRIDGE_RESOURCE_TYPE*/"oic.r.securemode" == resType)
    {
        bool secureMode = false;
        if (!OCRepPayloadSetPropBool(payload, "secureMode", secureMode))
        {
            throw "Failed to set 'secureMode' in payload";
        }
        DEBUG_PRINT(uri << " -- secureMode: " << (secureMode ? "true" : "false"));

//        if (!s_links.empty())
//        {
//            DEBUG_PRINT("Setting links");
//            const OCRepPayload *links[s_links.size()];
//            size_t dimensions[MAX_REP_ARRAY_DEPTH] = {s_links.size(), 0, 0};
//            for (unsigned int i = 0; i < s_links.size(); ++i) {
//                DEBUG_PRINT("link[" << i << "]");
//                DEBUG_PRINT("\thref=" << s_links[i].href);
//                DEBUG_PRINT("\trel=" << s_links[i].rel);
//                DEBUG_PRINT("\trt=" << s_links[i].rt);
//                OCRepPayload *linkPayload = OCRepPayloadCreate();
//                OCRepPayloadSetPropString(linkPayload, "href", s_links[i].href.c_str());
//                OCRepPayloadSetPropString(linkPayload, "rel", s_links[i].rel.c_str());
//                OCRepPayloadSetPropString(linkPayload, "rt", s_links[i].rt.c_str());
//                links[i] = linkPayload;
//            }
//            OCRepPayloadSetPropObjectArray(payload, "links", links, dimensions);
//        }
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
    }

    return payload;
}
