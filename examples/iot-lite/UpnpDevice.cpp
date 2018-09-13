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
        throw logic_error(DEVICE_TYPE + string(": ") + m_deviceType + string(" not implemented!"));
        return;
    }

    m_name = getStringField(gupnp_device_info_get_friendly_name, deviceInfo);

    string udnWithoutPrefix;
    if (m_udn.find(UPNP_PREFIX_UDN) == 0)
    {
        udnWithoutPrefix = m_udn.substr(UPNP_PREFIX_UDN.size());
    }
    else
    {
        udnWithoutPrefix = m_udn;
    }
    m_uri = UpnpUriPrefixMap[m_resourceType] + udnWithoutPrefix;

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

void UpnpDevice::addDeviceCallback(void *data)
{
    if (data == NULL)
    {
        ERROR_PRINT("device data is null");
        return;
    }

    GUPnPDeviceInfo *deviceInfo = (GUPnPDeviceInfo *)data;

    // TODO:  need a way to remove dead devices
    if (! GUPNP_IS_DEVICE_INFO(deviceInfo))
    {
        DEBUG_PRINT("invalid device info (TODO: remove this device)");
        return;
    }

    oc_set_custom_device_property(DEVICE_TYPE.c_str(), gupnp_device_info_get_device_type(deviceInfo));
    DEBUG_PRINT(DEVICE_TYPE << ": " << gupnp_device_info_get_device_type(deviceInfo));

    oc_set_custom_device_property(UDN.c_str(), gupnp_device_info_get_udn(deviceInfo));
    DEBUG_PRINT(UDN << ": " << gupnp_device_info_get_udn(deviceInfo));

    for (auto const &kv : s_deviceInfo2AttributesMap)
    {
        char *c_field = kv.second(deviceInfo);
        if (c_field != NULL)
        {
            oc_set_custom_device_property(kv.first.c_str(), c_field);
            DEBUG_PRINT(kv.first << ": " << c_field);
            g_free(c_field);
        }
    }
}
