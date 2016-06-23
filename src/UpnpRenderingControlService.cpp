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

#include "UpnpRenderingControlService.h"

using namespace OIC::Service;

static const string MODULE = "UpnpRenderingControlService";

// Rendering Control Service

// Organization:
// Attribute Name,
// State Variable Name (can be empty string), State variable type, "evented" flag
// Actions:
//    0: "GET" action name, action type, optional out parameters: var_name,var_type
//    1: "SET" action name, action type, optional in parameters: var_name,var_type
// Vector of embedded attributes (if present)
vector <UpnpAttributeInfo> UpnpRenderingControl::Attributes =
{
    {
        "lastChange",
        "LastChange", G_TYPE_STRING, true,
        {},
        {}
    },
    {
        "presetNameList",
        "PresetNameList", G_TYPE_STRING, false,
        {{"ListPresets", UPNP_ACTION_GET, "CurrentPresetNameList", G_TYPE_STRING}},
        {}
    },
    {
        "presetName",
        "PresetName", G_TYPE_STRING, false,
        {
            {"", UPNP_ACTION_GET, "", G_TYPE_NONE},
            {"SelectPreset", UPNP_ACTION_POST, "PresetName", G_TYPE_STRING}
        },
        {}
    },
    {
        "mute",
        "Mute", G_TYPE_BOOLEAN, false,
        {   {"GetMute", UPNP_ACTION_GET, "CurrentMute", G_TYPE_BOOLEAN},
            {"SetMute", UPNP_ACTION_POST, "DesiredMute", G_TYPE_BOOLEAN}
        },
        {}
    },
    {
        "volume",
        "Volume", G_TYPE_UINT, false,
        {   {"GetVolume", UPNP_ACTION_GET, "CurrentVolume", G_TYPE_UINT},
            {"SetVolume", UPNP_ACTION_POST, "DesiredVolume", G_TYPE_UINT}
        },
        {}
    }
};

// Custom action maps:
// "attribute name" -> GET request handlers
map <const string, UpnpRenderingControl::GetAttributeHandler>
    UpnpRenderingControl::GetAttributeActionMap =
{
    {"presetNameList", &UpnpRenderingControl::getPresetNameList},
    {"mute", &UpnpRenderingControl::getMute},
    {"volume", &UpnpRenderingControl::getVolume},
};

// "attribute name" -> SET request handlers
map <const string, UpnpRenderingControl::SetAttributeHandler>
    UpnpRenderingControl::SetAttributeActionMap =
{
    {"presetName", &UpnpRenderingControl::setPresetName},
    {"mute", &UpnpRenderingControl::setMute},
    {"volume", &UpnpRenderingControl::setVolume},
};

// TODO Implement additional OCF attributes/UPnP Actions as necessary

void UpnpRenderingControl::getPresetNameListCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;
    char *currentPresetNameList;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "CurrentPresetNameList",
                                                  G_TYPE_STRING,
                                                  &currentPresetNameList,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetPresetNameList failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    if (status)
    {
        DEBUG_PRINT("GetPresetNameList currentPresetNameList=" << currentPresetNameList);

        request->resource->setAttribute("presetNameList", string(currentPresetNameList), false);
        g_free(currentPresetNameList);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpRenderingControl::getPresetNameList(UpnpRequest *request, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;

    if (! queryParams.empty()) {
        auto it = queryParams.find("instanceId");
        if (it != queryParams.end()) {
            try {
                instanceId = std::stoi(it->second);
                instanceId = std::max(0, instanceId);
                DEBUG_PRINT("getPresetNameList queryParam " << it->first << "=" << it->second);
            } catch (const std::invalid_argument& ia) {
                ERROR_PRINT("Invalid getPresetNameList queryParam " << it->first << "=" << it->second);
            }
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "ListPresets",
                                          getPresetNameListCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["presetNameList"].first;

    return true;
}

void UpnpRenderingControl::setPresetNameCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("SetPresetName failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpRenderingControl::setPresetName(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;
    string presetName = value->get< string >();

    if (! queryParams.empty()) {
        auto it = queryParams.find("instanceId");
        if (it != queryParams.end()) {
            try {
                instanceId = std::stoi(it->second);
                instanceId = std::max(0, instanceId);
                DEBUG_PRINT("setPresetName queryParam " << it->first << "=" << it->second);
            } catch (const std::invalid_argument& ia) {
                ERROR_PRINT("Invalid setPresetName queryParam " << it->first << "=" << it->second);
            }
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "SelectPreset",
                                          setPresetNameCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          "PresetName",
                                          G_TYPE_STRING,
                                          presetName.c_str(),
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["presetName"].first;

    return true;
}

void UpnpRenderingControl::getMuteCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;
    bool currentMute;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "CurrentMute",
                                                  G_TYPE_BOOLEAN,
                                                  &currentMute,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetMute failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    if (status)
    {
        DEBUG_PRINT("GetMute mute=" << currentMute);

        request->resource->setAttribute("mute", currentMute, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpRenderingControl::getMute(UpnpRequest *request, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;
    string channel = "Master";

    if (! queryParams.empty()) {
        auto it = queryParams.find("instanceId");
        if (it != queryParams.end()) {
            try {
                instanceId = std::stoi(it->second);
                instanceId = std::max(0, instanceId);
                DEBUG_PRINT("getMute queryParam " << it->first << "=" << it->second);
            } catch (const std::invalid_argument& ia) {
                ERROR_PRINT("Invalid getMute queryParam " << it->first << "=" << it->second);
            }
        }

        it = queryParams.find("channel");
        if (it != queryParams.end()) {
            channel = it->second;
            DEBUG_PRINT("getMute queryParam " << it->first << "=" << it->second);
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "GetMute",
                                          getMuteCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          "Channel",
                                          G_TYPE_STRING,
                                          channel.c_str(),
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["mute"].first;

    return true;
}

void UpnpRenderingControl::setMuteCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("SetMute failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpRenderingControl::setMute(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;
    string channel = "Master";
    bool mute = value->get< bool >();

    DEBUG_PRINT("setMute mute=" << mute);

    if (! queryParams.empty()) {
        auto it = queryParams.find("instanceId");
        if (it != queryParams.end()) {
            try {
                instanceId = std::stoi(it->second);
                instanceId = std::max(0, instanceId);
                DEBUG_PRINT("setMute queryParam " << it->first << "=" << it->second);
            } catch (const std::invalid_argument& ia) {
                ERROR_PRINT("Invalid setMute queryParam " << it->first << "=" << it->second);
            }
        }

        it = queryParams.find("channel");
        if (it != queryParams.end()) {
            channel = it->second;
            DEBUG_PRINT("setMute queryParam " << it->first << "=" << it->second);
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "SetMute",
                                          setMuteCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          "Channel",
                                          G_TYPE_STRING,
                                          channel.c_str(),
                                          "DesiredMute",
                                          G_TYPE_BOOLEAN,
                                          mute,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["mute"].first;

    return true;
}

void UpnpRenderingControl::getVolumeCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;
    int currentVolume;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "CurrentVolume",
                                                  G_TYPE_UINT,
                                                  &currentVolume,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetVolume failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    if (status)
    {
        DEBUG_PRINT("GetVolume volume=" << currentVolume);

        request->resource->setAttribute("volume", currentVolume, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpRenderingControl::getVolume(UpnpRequest *request, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;
    string channel = "Master";

    if (! queryParams.empty()) {
        auto it = queryParams.find("instanceId");
        if (it != queryParams.end()) {
            try {
                instanceId = std::stoi(it->second);
                instanceId = std::max(0, instanceId);
                DEBUG_PRINT("getVolume queryParam " << it->first << "=" << it->second);
            } catch (const std::invalid_argument& ia) {
                ERROR_PRINT("Invalid getVolume queryParam " << it->first << "=" << it->second);
            }
        }

        it = queryParams.find("channel");
        if (it != queryParams.end()) {
            channel = it->second;
            DEBUG_PRINT("getVolume queryParam " << it->first << "=" << it->second);
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "GetVolume",
                                          getVolumeCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          "Channel",
                                          G_TYPE_STRING,
                                          channel.c_str(),
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["volume"].first;

    return true;
}

void UpnpRenderingControl::setVolumeCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("SetVolume failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpRenderingControl::setVolume(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;
    string channel = "Master";
    int volume = value->get< int >();;

    DEBUG_PRINT("volume = " << volume);

    if (!queryParams.empty()) {
        auto it = queryParams.find("instanceId");
        if (it != queryParams.end()) {
            try {
                instanceId = std::stoi(it->second);
                instanceId = std::max(0, instanceId);
                DEBUG_PRINT("setVolume queryParam " << it->first << "=" << it->second);
            } catch (const std::invalid_argument& ia) {
                ERROR_PRINT("Invalid setVolume queryParam " << it->first << "=" << it->second);
            }
        }

        it = queryParams.find("channel");
        if (it != queryParams.end()) {
            channel = it->second;
            DEBUG_PRINT("setVolume queryParam " << it->first << "=" << it->second);
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "SetVolume",
                                          setVolumeCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          "Channel",
                                          G_TYPE_STRING,
                                          channel.c_str(),
                                          "DesiredVolume",
                                          G_TYPE_UINT,
                                          volume,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["volume"].first;

    return true;
}

bool UpnpRenderingControl::getAttributesRequest(UpnpRequest *request,
                                                const map< string, string > &queryParams)
{
    bool status = false;

    for (auto it = m_attributeMap.begin(); it != m_attributeMap.end(); ++it)
    {
        DEBUG_PRINT(" \"" << it->first << "\"");
        // Check the request
        if (!UpnpAttribute::isValidRequest(&m_attributeMap, it->first, UPNP_ACTION_GET))
        {
            request->done++;
            continue;
        }

        UpnpAttributeInfo *attrInfo = it->second.first;
        bool result = false;

        // Check if custom GET needs to be called
        auto attr = this->GetAttributeActionMap.find(it->first);
        if (attr != this->GetAttributeActionMap.end())
        {
            GetAttributeHandler fp = attr->second;
            result = (this->*fp)(request, queryParams);
        }
        else if ((attrInfo != NULL) && string(attrInfo->actions[0].name) != "")
        {
            result = UpnpAttribute::get(m_proxy, request, attrInfo);
        }

        status |= result;
        if (!result)
        {
            request->done++;
        }
    }

    return status;
}

bool UpnpRenderingControl::setAttributesRequest(const RCSResourceAttributes &value,
                                                UpnpRequest *request,
                                                const map< string, string > &queryParams)
{
    bool status = false;

    for (auto it = value.begin(); it != value.end(); ++it)
    {
        const std::string attrName = it->key();

        DEBUG_PRINT(" \"" << attrName << "\"");

        // Check the request
        if (!UpnpAttribute::isValidRequest(&m_attributeMap, attrName, UPNP_ACTION_POST))
        {
            request->done++;
            continue;
        }
        RCSResourceAttributes::Value attrValue = it->value();

        UpnpAttributeInfo *attrInfo = m_attributeMap[attrName].first;
        bool result = false;

        // Check if custom SET needs to be called
        auto attr = this->SetAttributeActionMap.find(attrName);
        if (attr != this->SetAttributeActionMap.end())
        {
            SetAttributeHandler fp = attr->second;
            result = (this->*fp)(request, &attrValue, queryParams);
        }
        else if ((attrInfo != NULL) && string(attrInfo->actions[0].name) != "")
        {
            result = UpnpAttribute::set(m_proxy, request, attrInfo, &attrValue);
        }

        status |= result;
        if (!result)
        {
            request->done++;
        }
    }

    return status;
}
