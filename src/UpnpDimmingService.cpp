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

#include "UpnpDimmingService.h"

static const string MODULE = "UpnpDimmingService";

// Service specific attribute initialization

// Brightness service
    // TODO the following actions are not yet mapped: (note all unmapped actions are optional)
    //      SetOnEffectLevel
    //      SetOnEffect
    //      GetOnEffectParameters
    //      StepUp
    //      StepDown
    //      StartRampUp
    //      StartRampDown
    //      StopRamp
    //      StartRampToLevel
    //      SetStepDelta
    //      GetStepDelta
    //      SetRampRate
    //      GetRampRate
    //      PauseRamp
    //      ResumeRamp
    //      GetRampPaused
    //      GetRampTime
    //      GetIsRamping

// Organization:
// Attribute Name,
// State Variable Name (can be empty string), State variable type, "evented" flag
// Actions:
//    0: "GET" action name, action type, optional out parameters: var_name,var_type
//    1: "SET" action name, action type, optional in parameters: var_name,var_type
// Vector of embedded attributes (if present)
vector <UpnpAttributeInfo> UpnpDimming::Attributes = {
    {"brightness",
     "LoadLevelStatus", G_TYPE_UINT, true,
     {{"GetLoadLevelStatus", UPNP_ACTION_GET, "retLoadlevelStatus", G_TYPE_UINT},
      {"SetLoadLevelTarget", UPNP_ACTION_POST, "newLoadlevelTarget", G_TYPE_UINT}},
     {}
    }
};

bool UpnpDimming::getAttributesRequest(UpnpRequest *request)
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
        GUPnPServiceProxyAction *action = UpnpAttribute::get(m_proxy, request, attrInfo);

        status |= (action != NULL);
        if (action == NULL)
        {
            request->done++;
        }
    }
    return status;
}

bool UpnpDimming::setAttributesRequest(const RCSResourceAttributes &value, UpnpRequest *request)
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
        GUPnPServiceProxyAction *action = UpnpAttribute::set(m_proxy, request, attrInfo, &attrValue);
        status |= (action != NULL);
        if (action == NULL)
        {
            request->done++;
        }
    }

    return status;
}
