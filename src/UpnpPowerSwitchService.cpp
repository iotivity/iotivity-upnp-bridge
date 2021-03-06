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

#include "UpnpPowerSwitchService.h"

using namespace OIC::Service;

static const string MODULE = "UpnpPowerSwitchService";

// Power switch service

// Organization:
// Attribute Name,
// State Variable Name (can be empty string), State variable type, "evented" flag
// Actions:
//    0: "GET" action name, action type, optional out parameters: var_name,var_type
//    1: "SET" action name, action type, optional in parameters: var_name,var_type
// Vector of embedded attributes (if present)
vector <UpnpAttributeInfo> UpnpPowerSwitch::Attributes =
{
    {
        "value",
        "Status", G_TYPE_BOOLEAN, true,
        {   {"GetTarget", UPNP_ACTION_GET, "RetTargetValue", G_TYPE_BOOLEAN},
            {"SetTarget", UPNP_ACTION_POST, "newTargetValue", G_TYPE_BOOLEAN}
        },
        {}
    }
};

bool UpnpPowerSwitch::getAttributesRequest(UpnpRequest *request,
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
        bool result = UpnpAttribute::get(m_proxy, request, attrInfo);

        status |= result;
        if (!result)
        {
            request->done++;
        }
    }
    return status;
}

bool UpnpPowerSwitch::setAttributesRequest(const RCSResourceAttributes &value,
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
        bool result = UpnpAttribute::set(m_proxy, request, attrInfo, &attrValue);
        status |= result;
        if (!result)
        {
            request->done++;
        }
    }

    return status;
}
