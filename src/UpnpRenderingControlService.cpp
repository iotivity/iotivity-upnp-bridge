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
        "presetNamelist",
        "PresetNameList", G_TYPE_STRING, false,
        {   {"ListPresets", UPNP_ACTION_GET, "CurrentPresetNameList", G_TYPE_STRING},
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

// TODO Implement additional OCF attributes/UPnP Actions as necessary

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
        bool result = UpnpRenderingControlAttribute::get(m_proxy, request, attrInfo);

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
        bool result = UpnpRenderingControlAttribute::set(m_proxy, request, attrInfo, &attrValue);

        status |= result;
        if (!result)
        {
            request->done++;
        }
    }

    return status;
}
