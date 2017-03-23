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

static const char* powerSwitchStateName = "value";

OCEntityHandlerResult UpnpPowerSwitch::processGetRequest(OCRepPayload *payload)
{
    if (payload == NULL)
    {
        throw "payload is null";
    }

    if (!OCRepPayloadSetPropBool(payload, powerSwitchStateName, powerSwitchStateValue))
    {
        throw "Failed to set power switch value in payload";
    }
    DEBUG_PRINT(powerSwitchStateName << ": " << (powerSwitchStateValue ? "true" : "false"));

    return OC_EH_OK;
}
