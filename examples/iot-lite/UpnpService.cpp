//******************************************************************
//
// Copyright 2018 Intel Corporation All Rights Reserved.
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

#include "UpnpDimmingService.h"
#include "UpnpPowerSwitchService.h"

using namespace std;

static const string MODULE = "UpnpService";

static const map< string, function< char *(GUPnPServiceInfo *serviceInfo)>> s_serviceInfo2AttributesMap =
{
    {"serviceId",     gupnp_service_info_get_id},
    {"scpdUrl",       gupnp_service_info_get_scpd_url},
    {"controlUrl",    gupnp_service_info_get_control_url},
    {"eventSubUrl",   gupnp_service_info_get_event_subscription_url}
};

static map <string, int> s_deviceIndexLookup; // key is uri

UpnpService::UpnpService(GUPnPServiceInfo *serviceInfo,
                         string type,
                         UpnpRequestState *requestState)
{
    DEBUG_PRINT("(" << std::this_thread::get_id() << ")");
    m_proxy = nullptr;
    m_resourceType = type;
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

    // URI format: resource_type/service_id/udn_without_uuid_prefix
    string udnWithoutPrefix;
    if (m_udn.find(UPNP_PREFIX_UDN) == 0)
    {
        udnWithoutPrefix = m_udn.substr(UPNP_PREFIX_UDN.size());
    }
    else
    {
        udnWithoutPrefix = m_udn;
    }
    m_uri = UpnpUriPrefixMap[m_resourceType] + m_name + "/" + udnWithoutPrefix;
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
    // TODO: ?
}

UpnpService::~UpnpService()
{
    DEBUG_PRINT("(" << std::this_thread::get_id() << "), uri:" << m_uri);
    this->stop();
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

void UpnpService::setDeviceIndexForUri(const string uri, int deviceIndex)
{
    s_deviceIndexLookup[uri] = deviceIndex;
}

int UpnpService::getDeviceIndexForUri(const string uri)
{
    int deviceIndex = s_deviceIndexLookup[uri];
    DEBUG_PRINT("deviceIndex = " << deviceIndex << " for uri " << uri);
    return deviceIndex;
}

void UpnpService::processGetRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    DEBUG_PRINT("process get request");
    DEBUG_PRINT("interface=" << interface);
    DEBUG_PRINT("request->resource->name=" << oc_string(request->resource->name));
    DEBUG_PRINT("request->resource->uri=" << oc_string(request->resource->uri));

    GUPnPServiceInfo *serviceInfo = (GUPnPServiceInfo *)user_data;

    if (! GUPNP_IS_SERVICE_INFO(serviceInfo))
    {
        // remove dead service resource
        char *uri = (char *)(request->resource->uri).ptr;
        size_t deviceIndex = getDeviceIndexForUri(string(uri));
        DEBUG_PRINT("Removing resource for: " << uri << ", deviceIndex: " << deviceIndex);
        oc_resource_t *res = oc_ri_get_app_resource_by_uri(uri, strlen(uri), deviceIndex);
        bool result = oc_delete_resource(res);
        DEBUG_PRINT("Delete resource result: " << result);

        oc_send_response(request, OC_STATUS_INTERNAL_SERVER_ERROR);
        return;
    }

    for (int i = 0; i < (int)oc_string_array_get_allocated_size(request->resource->types); i++)
    {
//      size_t size = oc_string_array_get_item_size(request->resource->types, i);
      const char *type = oc_string_array_get_item(request->resource->types, i);
      DEBUG_PRINT("request->resource->type[" << i << "]=" << type);
      if (string(type) == UPNP_OIC_TYPE_POWER_SWITCH)
      {
          return UpnpPowerSwitch::processGetRequest(request, interface, user_data);
      }
      if (string(type) == UPNP_OIC_TYPE_BRIGHTNESS)
      {
          return UpnpDimming::processGetRequest(request, interface, user_data);
      }
    }
}

void UpnpService::processPostRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    DEBUG_PRINT("process post request");
    DEBUG_PRINT("interface=" << interface);
    DEBUG_PRINT("request->resource->name=" << oc_string(request->resource->name));
    DEBUG_PRINT("request->resource->uri=" << oc_string(request->resource->uri));

    GUPnPServiceInfo *serviceInfo = (GUPnPServiceInfo *)user_data;

    if (! GUPNP_IS_SERVICE_INFO(serviceInfo))
    {
        // remove dead service resource
        char *uri = (char *)(request->resource->uri).ptr;
        size_t deviceIndex = getDeviceIndexForUri(string(uri));
        DEBUG_PRINT("Removing resource for: " << uri << ", deviceIndex: " << deviceIndex);
        oc_resource_t *res = oc_ri_get_app_resource_by_uri(uri, strlen(uri), deviceIndex);
        bool result = oc_delete_resource(res);
        DEBUG_PRINT("Delete resource result: " << result);

        oc_send_response(request, OC_STATUS_INTERNAL_SERVER_ERROR);
        return;
    }

    for (int i = 0; i < (int)oc_string_array_get_allocated_size(request->resource->types); i++)
    {
//      size_t size = oc_string_array_get_item_size(request->resource->types, i);
      const char *type = oc_string_array_get_item(request->resource->types, i);
      DEBUG_PRINT("request->resource->type[" << i << "]=" << type);
      if (string(type) == UPNP_OIC_TYPE_POWER_SWITCH)
      {
          return UpnpPowerSwitch::processPostRequest(request, interface, user_data);
      }
      if (string(type) == UPNP_OIC_TYPE_BRIGHTNESS)
      {
          return UpnpDimming::processPostRequest(request, interface, user_data);
      }
    }
}

void UpnpService::processPutRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    DEBUG_PRINT("process put request");
    DEBUG_PRINT("interface=" << interface);
    DEBUG_PRINT("request->resource->name=" << oc_string(request->resource->name));
    DEBUG_PRINT("request->resource->uri=" << oc_string(request->resource->uri));

    return processPostRequest(request, interface, user_data);
}
