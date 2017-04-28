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

OCEntityHandlerResult UpnpService::processGetRequest(OCRepPayload *payload)
{
	(void) payload;
    return OC_EH_OK;
}

OCEntityHandlerResult UpnpService::processPutRequest(OCEntityHandlerRequest*,
            string, string, OCRepPayload*)
{
    ERROR_PRINT("Service processPutRequest() not implemented!");
    throw NotImplementedException("Service processPutRequest() not implemented!");
    return OC_EH_ERROR;
}
