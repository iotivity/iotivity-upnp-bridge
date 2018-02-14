//******************************************************************
//
// Copyright 2016-2018 Intel Corporation All Rights Reserved.
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
    {"friendlyName",          gupnp_device_info_get_friendly_name},
    {"manufacturer",          gupnp_device_info_get_manufacturer},
    {"manufacturerUrl",       gupnp_device_info_get_manufacturer_url},
    {"modelNumber",           gupnp_device_info_get_model_number},
    {"modelName",             gupnp_device_info_get_model_name},
    {"modelDescription",      gupnp_device_info_get_model_description},
    {"modelUrl",              gupnp_device_info_get_model_url},
    {"serialNumber",          gupnp_device_info_get_serial_number},
    {"presentationUrl",       gupnp_device_info_get_presentation_url},
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

OCEntityHandlerResult UpnpDevice::processGetRequest(OCRepPayload *payload, string resourceType)
{
    if (payload == NULL)
    {
        throw "payload is null";
    }

    vector<_iconLink> iconLinks;

    if (UPNP_DEVICE_RESOURCE == resourceType || UPNP_OIC_TYPE_DEVICE_LIGHT == resourceType)
    {
        GUPnPDeviceInfo *deviceInfo = GUPNP_DEVICE_INFO(m_proxy);
        if (! GUPNP_IS_DEVICE_INFO(deviceInfo))
        {
            ERROR_PRINT("GUPNP_DEVICE_INFO failed for proxy for " << m_uri);
            return OC_EH_ERROR;
        }

        if (!OCRepPayloadSetPropString(payload, DEVICE_TYPE.c_str(), m_deviceType.c_str()))
        {
            string message = "Failed to set " + DEVICE_TYPE + " value in payload";
            throw message;
        }
        DEBUG_PRINT(DEVICE_TYPE << ": " << m_deviceType);

        if (!OCRepPayloadSetPropString(payload, UDN.c_str(), m_udn.c_str()))
        {
            string message = "Failed to set " + UDN + " value in payload";
            throw message;
        }
        DEBUG_PRINT(UDN << ": " << m_udn);

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

        // get the smallest icon first
        char *mimeType = NULL;
        int width = 0;
        int height = 0;
        char *iconUrl = gupnp_device_info_get_icon_url(deviceInfo, NULL, -1, -1, -1, false,
                &mimeType, NULL, &width, &height);
        if (iconUrl != NULL)
        {
            _iconLink iconLink;
            iconLink.href = m_uri;
            iconLink.rel = LINK_REL_ICON;
            iconLink.rt = OIC_TYPE_ICON;
            iconLink.mimetype = mimeType;
            iconLink.media = iconUrl;
            iconLink.width = width;
            iconLink.height = height;
            iconLinks.push_back(iconLink);

            g_free(iconUrl);
            g_free(mimeType);
        }

        // continue getting larger icons
        width = -1;
        height = -1;
        int nextWidth = 0;
        int nextHeight = 0;
        bool gettingIcons = true;
        while (gettingIcons)
        {
            iconUrl = gupnp_device_info_get_icon_url(deviceInfo, NULL, -1, width, height, true,
                    &mimeType, NULL, &nextWidth, &nextHeight);
            if (iconUrl != NULL)
            {
                if (width != nextWidth || height != nextHeight)
                {
                    _iconLink iconLink;
                    iconLink.href = m_uri;
                    iconLink.rel = LINK_REL_ICON;
                    iconLink.rt = OIC_TYPE_ICON;
                    iconLink.mimetype = mimeType;
                    iconLink.media = iconUrl;
                    iconLink.width = nextWidth;
                    iconLink.height = nextHeight;
                    iconLinks.push_back(iconLink);

                    width = nextWidth;
                    height = nextHeight;
                }
                else
                {
                    gettingIcons = false;
                }
                g_free(iconUrl);
                g_free(mimeType);
            }
            else{
                gettingIcons = false;
            }
        }
    }

    if (!m_links.empty() || !iconLinks.empty())
    {
        DEBUG_PRINT("Setting links");
        const OCRepPayload *links[m_links.size()+iconLinks.size()];
        size_t dimensions[MAX_REP_ARRAY_DEPTH] = {m_links.size()+iconLinks.size(), 0, 0};
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
        for (unsigned int i = 0; i < iconLinks.size(); ++i) {
            DEBUG_PRINT(OC_RSRVD_LINKS << "[" << m_links.size()+i << "]");
            DEBUG_PRINT("\t" << OC_RSRVD_HREF << "=" << iconLinks[i].href);
            DEBUG_PRINT("\t" << OC_RSRVD_REL << "=" << iconLinks[i].rel);
            DEBUG_PRINT("\t" << OC_RSRVD_RESOURCE_TYPE << "=" << iconLinks[i].rt);
            DEBUG_PRINT("\t" << MIMETYPE.c_str() << "=" << iconLinks[i].mimetype);
            DEBUG_PRINT("\t" << MEDIA.c_str() << "=" << iconLinks[i].media);
            DEBUG_PRINT("\t" << ICON_WIDTH.c_str() << "=" << iconLinks[i].width);
            DEBUG_PRINT("\t" << ICON_HEIGHT.c_str() << "=" << iconLinks[i].height);
            OCRepPayload *linkPayload = OCRepPayloadCreate();
            OCRepPayloadSetPropString(linkPayload, OC_RSRVD_HREF, iconLinks[i].href.c_str());
            OCRepPayloadSetPropString(linkPayload, OC_RSRVD_REL, iconLinks[i].rel.c_str());
            OCRepPayloadSetPropString(linkPayload, OC_RSRVD_RESOURCE_TYPE, iconLinks[i].rt.c_str());
            OCRepPayloadSetPropString(linkPayload, MIMETYPE.c_str(), iconLinks[i].mimetype.c_str());
            OCRepPayloadSetPropString(linkPayload, MEDIA.c_str(), iconLinks[i].media.c_str());
            OCRepPayloadSetPropInt(linkPayload, ICON_WIDTH.c_str(), iconLinks[i].width);
            OCRepPayloadSetPropInt(linkPayload, ICON_HEIGHT.c_str(), iconLinks[i].height);
            links[m_links.size()+i] = linkPayload;
        }
        OCRepPayloadSetPropObjectArray(payload, OC_RSRVD_LINKS, links, dimensions);
    }

    return OC_EH_OK;
}
