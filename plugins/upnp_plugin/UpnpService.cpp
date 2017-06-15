//******************************************************************
//
// Copyright 2016 Intel Corporation All Rights Reserved.
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

#include <soup.h>
#include <future>
#include <thread>

#include "UpnpConstants.h"
#include "UpnpException.h"
#include "UpnpInternal.h"
#include "UpnpService.h"

using namespace std;

static const string MODULE = "UpnpService";

static const map< string, function< char *(GUPnPServiceInfo *serviceInfo)>> s_serviceInfo2AttributesMap =
{
    {"serviceId",     gupnp_service_info_get_id},
    {"scpdUrl",       gupnp_service_info_get_scpd_url},
    {"controlUrl",    gupnp_service_info_get_control_url},
    {"eventSubUrl",   gupnp_service_info_get_event_subscription_url}
};

UpnpService::UpnpService(GUPnPServiceInfo *serviceInfo,
                         string type,
                         UpnpRequestState *requestState,
                         vector <UpnpAttributeInfo> *attributeInfo)
{
    DEBUG_PRINT("(" << std::this_thread::get_id() << ")");
    m_proxy = nullptr;
    m_resourceType = type;

    if (attributeInfo == nullptr)
    {
        ERROR_PRINT("Service attribute table for " << m_resourceType << " not present!");
        throw NotImplementedException("UpnpService::ctor: Service attribute table for " + m_resourceType +
                                      " not present!");
        return;
    }

    m_serviceAttributeInfo = attributeInfo;
    m_requestState = requestState;

    //UDN of the hosting device
    m_udn = gupnp_service_info_get_udn(serviceInfo);

    // Service ID (differentiates between different services of identical type hosted by one device).
    m_serviceId = getStringField(gupnp_service_info_get_id, serviceInfo);

    // Use service ID as basis for resource name, stripping service ID prefix.
    // Check that service ID was set. If not, raise exception.
    if (m_serviceId == "")
    {
        //TODO change to invalid exception
        ERROR_PRINT("Service ID for " << m_resourceType << " not present!");
        throw NotImplementedException("UpnpService::ctor: Service ID for " + m_resourceType +
                                      " not present!");
        return;
    }

    if (m_serviceId.compare(0, UPNP_PREFIX_SERVICE_ID.size(), UPNP_PREFIX_SERVICE_ID) != 0)
    {
        ERROR_PRINT("Invalid service ID format " << m_serviceId);
        return;
    }

    m_name = m_serviceId.substr(UPNP_PREFIX_SERVICE_ID.size());

    //TODO: discuss service URI format. Currently: resource_type/service_id/udn
    m_uri = UpnpUriPrefixMap[m_resourceType] + m_name + "/" + m_udn;
    DEBUG_PRINT("service URI: " << m_uri);
    if (m_uri.length() > MAX_URI_LENGTH)
    {
        ERROR_PRINT("URI too long " << m_uri << "( " << m_uri.length());
        throw BadUriException("UpnpService::ctor: uri length too long");
        return;
    }

    m_interface = UpnpInterfaceMap[m_resourceType];

}

void UpnpService::stop()
{
    if (!m_stateVarMap.empty())
    {
        std::map<string, StateVarAttr>::iterator it;
        for (it = m_stateVarMap.begin(); it != m_stateVarMap.end(); ++it)
        {
            DEBUG_PRINT("remove notify for \"" << it->first << "\"");

//            gupnp_service_proxy_remove_notify (m_proxy,
//                                               (it->first).c_str(),
//                                               onStateChanged,
//                                               this);
        }
        m_stateVarMap.clear();
    }
}

UpnpService::~UpnpService()
{
    DEBUG_PRINT("(" << std::this_thread::get_id() << "), uri:" << m_uri);
//    this->stop();
    m_proxy = nullptr;
}

void UpnpService::setProxy(GUPnPServiceProxy *proxy)
{
    m_proxy = proxy;
}

GUPnPServiceProxy *UpnpService::getProxy()
{
    return m_proxy;
}

string UpnpService::getId()
{
    return m_serviceId;
}

// TODO: This should probably live in some UpnpUtil class
string UpnpService::getStringField(function< char *(GUPnPServiceInfo *serviceInfo)> f,
                                   GUPnPServiceInfo *serviceInfo)
{
    char *c_field = f(serviceInfo);
    if (c_field != NULL)
    {
        string s_field = string(c_field);
        g_free(c_field);
        return s_field;
    }
    return "";
}

OCEntityHandlerResult UpnpService::processGetRequest(OCRepPayload *payload, string resourceType)
{
    if (payload == NULL)
    {
        throw "payload is null";
    }

    if (UPNP_SERVICE_RESOURCE == resourceType)
    {
        GUPnPServiceInfo *serviceInfo = GUPNP_SERVICE_INFO(m_proxy);

        const char *serviceType = gupnp_service_info_get_service_type(serviceInfo);
        if (OCRepPayloadSetPropString(payload, "serviceType", serviceType))
        {
            DEBUG_PRINT("serviceType: " << serviceType);
        }
        else
        {
            ERROR_PRINT("Failed to set serviceType value in payload");
        }

        for (auto const &kv : s_serviceInfo2AttributesMap)
        {
            char *c_field = kv.second(serviceInfo);
            if (c_field != NULL)
            {
                if (OCRepPayloadSetPropString(payload, kv.first.c_str(), c_field))
                {
                    DEBUG_PRINT(kv.first << ": " << c_field);
                    g_free(c_field);
                }
                else
                {
                    ERROR_PRINT("Failed to set " << kv.first << " value in payload");
                }
            }
        }
    }

    if (!m_links.empty())
    {
        DEBUG_PRINT("Setting links");
        const OCRepPayload *links[m_links.size()];
        size_t dimensions[MAX_REP_ARRAY_DEPTH] = {m_links.size(), 0, 0};
        for (unsigned int i = 0; i < m_links.size(); ++i) {
            DEBUG_PRINT(OC_RSRVD_LINKS << "[" << i << "]");
            DEBUG_PRINT("\t" << OC_RSRVD_HREF << "=" << m_links[i].href);
            DEBUG_PRINT("\t" << OC_RSRVD_REL << "=" << m_links[i].rel);
            DEBUG_PRINT("\t" << OC_RSRVD_RESOURCE_TYPE << "=" << m_links[i].rt);
            OCRepPayload *linkPayload = OCRepPayloadCreate();
            OCRepPayloadSetPropString(linkPayload, OC_RSRVD_HREF, m_links[i].href.c_str());
            OCRepPayloadSetPropString(linkPayload, OC_RSRVD_REL, m_links[i].rel.c_str());
            OCRepPayloadSetPropString(linkPayload, OC_RSRVD_RESOURCE_TYPE, m_links[i].rt.c_str());
            links[i] = linkPayload;
        }
        OCRepPayloadSetPropObjectArray(payload, OC_RSRVD_LINKS, links, dimensions);
    }

    return OC_EH_OK;
}

OCEntityHandlerResult UpnpService::processPutRequest(OCEntityHandlerRequest*,
            string, string, OCRepPayload*)
{
    ERROR_PRINT("Service processPutRequest() not implemented!");
    throw NotImplementedException("Service processPutRequest() not implemented!");
    return OC_EH_ERROR;
}
