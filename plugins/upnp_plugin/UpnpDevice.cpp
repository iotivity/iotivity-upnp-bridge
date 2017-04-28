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

#include "UpnpConstants.h"
#include "UpnpException.h"
#include "UpnpInternal.h"
#include "UpnpDevice.h"
#include "UpnpHelper.h"
#include "UpnpService.h"

using namespace std;

static const string MODULE = "UpnpDevice";

UpnpDevice::UpnpDevice(GUPnPDeviceInfo *deviceInfo,
                       UpnpRequestState *requestState)
{
    m_proxy = nullptr;

    m_requestState = requestState;
    m_udn = gupnp_device_info_get_udn(deviceInfo);
    m_deviceType = gupnp_device_info_get_device_type(deviceInfo);

    m_resourceType = findResourceType(m_deviceType);

    if (m_resourceType == "")
    {
        throw logic_error(string("Device type ") + m_deviceType + string(" not implemented!"));
        return;
    }

    m_name = getStringField(gupnp_device_info_get_friendly_name, deviceInfo);
    m_uri = UpnpUriPrefixMap[m_resourceType] + gupnp_device_info_get_udn(deviceInfo);
    if (m_uri.length() > MAX_URI_LENGTH)
    {
        ERROR_PRINT("URI too long " << m_uri << "( " << m_uri.length());
        throw BadUriException("UpnpDevice::ctor: uri length too long");
        return;
    }
    m_interface = UpnpInterfaceMap[m_resourceType];

    m_registered = false;
    m_deviceList.clear();
    m_serviceList.clear();
    initBasicAttributes(deviceInfo);
}

UpnpDevice::~UpnpDevice()
{
    m_deviceList.clear();
    m_serviceList.clear();
}

void UpnpDevice::insertDevice(string udn)
{
    m_deviceList.push_back(udn);
}

void UpnpDevice::insertService(string id)
{
    m_serviceList.push_back(id);
}

std::vector<string> &UpnpDevice::getDeviceList()
{
    return m_deviceList;
}

std::vector<string> &UpnpDevice::getServiceList()
{
    return m_serviceList;
}

static const map< string, function< char *(GUPnPDeviceInfo *deviceInfo)>> s_deviceInfo2AttributesMap
        =
{
    {"n",                     gupnp_device_info_get_friendly_name},
    {"manufacturer",          gupnp_device_info_get_manufacturer},
    {"manufacturer_url",      gupnp_device_info_get_manufacturer_url},
    {"model_number",          gupnp_device_info_get_model_number},
    {"model_name",            gupnp_device_info_get_model_name},
    {"model_description",     gupnp_device_info_get_model_description},
    {"model_url",             gupnp_device_info_get_model_url},
    {"serial_number",         gupnp_device_info_get_serial_number},
    {"presentation_url",      gupnp_device_info_get_presentation_url},
    {"upc",                   gupnp_device_info_get_upc}
};

void UpnpDevice::initBasicAttributes(GUPnPDeviceInfo *deviceInfo)
{
    for (auto const &kv : s_deviceInfo2AttributesMap)
    {
        char *c_field = kv.second(deviceInfo);
        if (c_field != NULL)
        {
            string s_field = string(c_field);
            g_free(c_field);
        }
    }

    char *iconUrl = gupnp_device_info_get_icon_url(deviceInfo, NULL, -1, -1, -1, false, NULL, NULL, NULL, NULL);
    if (iconUrl != NULL)
    {
        g_free(iconUrl);
    }
}

void UpnpDevice::initAttributes()
{
}

void UpnpDevice::setProxy(GUPnPDeviceProxy *proxy)
{
    m_proxy = proxy;
}

GUPnPDeviceProxy *UpnpDevice::getProxy()
{
    return m_proxy;
}

void UpnpDevice::setParent(const string udn)
{
    m_parent = udn;
}

const string UpnpDevice::getParent()
{
    return m_parent;
}

// TODO" This should probably live in some UpnpUtil class
string UpnpDevice::getStringField(function< char *(GUPnPDeviceInfo *deviceInfo)> f,
                                  GUPnPDeviceInfo *deviceInfo)
{
    char *c_field = f(deviceInfo);
    if (c_field != NULL)
    {
        string s_field = string(c_field);
        g_free(c_field);
        return s_field;
    }
    return "";
}

OCEntityHandlerResult UpnpDevice::processGetRequest(OCRepPayload *payload)
{
    if (payload == NULL)
    {
        throw "payload is null";
    }

    GUPnPDeviceInfo *deviceInfo = GUPNP_DEVICE_INFO(m_proxy);

    if (!OCRepPayloadSetPropString(payload, "device_type", m_deviceType.c_str()))
    {
        throw "Failed to set device_type value in payload";
    }
    DEBUG_PRINT("device_type: " << m_deviceType);

    for (auto const &kv : s_deviceInfo2AttributesMap)
    {
        char *c_field = kv.second(deviceInfo);
        if (c_field != NULL)
        {
            string s_field = string(c_field);
            if (!OCRepPayloadSetPropString(payload, kv.first.c_str(), s_field.c_str()))
            {
                throw "Failed to set property value in payload";
            }
            DEBUG_PRINT(kv.first << ": " << s_field);
            g_free(c_field);
        }
    }

    char *iconUrl = gupnp_device_info_get_icon_url(deviceInfo, NULL, -1, -1, -1, false, NULL, NULL, NULL, NULL);
    if (iconUrl != NULL)
    {
        if (!OCRepPayloadSetPropString(payload, "icon_url", iconUrl))
        {
            throw "Failed to set icon_url value in payload";
        }
        DEBUG_PRINT("icon_url: " << iconUrl);
        g_free(iconUrl);
    }

    if (!OCRepPayloadSetPropString(payload, "name", m_name.c_str()))
    {
        throw "Failed to set name in payload";
    }
    DEBUG_PRINT("name: " << m_name);

//    if (!OCRepPayloadSetPropString(payload, "uri", m_uri.c_str()))
//    {
//        throw "Failed to set uri in payload";
//    }
//    DEBUG_PRINT("uri: " << m_uri);

    if (!m_links.empty())
    {
        DEBUG_PRINT("Setting links");
        const OCRepPayload *links[m_links.size()];
        size_t dimensions[MAX_REP_ARRAY_DEPTH] = {m_links.size(), 0, 0};
        for (unsigned int i = 0; i < m_links.size(); ++i) {
            DEBUG_PRINT("link[" << i << "]");
            DEBUG_PRINT("\thref=" << m_links[i].href);
            DEBUG_PRINT("\trel=" << m_links[i].rel);
            DEBUG_PRINT("\trt=" << m_links[i].rt);
            OCRepPayload *linkPayload = OCRepPayloadCreate();
            OCRepPayloadSetPropString(linkPayload, "href", m_links[i].href.c_str());
            OCRepPayloadSetPropString(linkPayload, "rel", m_links[i].rel.c_str());
            OCRepPayloadSetPropString(linkPayload, "rt", m_links[i].rt.c_str());
            links[i] = linkPayload;
        }
        OCRepPayloadSetPropObjectArray(payload, "links", links, dimensions);
    }

    return OC_EH_OK;
}
