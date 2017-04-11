//******************************************************************
//
// Copyright 2017 Intel Corporation All Rights Reserved.
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
vector <UpnpAttributeInfo> UpnpDimming::Attributes =
{
    {
        "brightness",
        "LoadLevelStatus", G_TYPE_UINT, true,
        {   {"GetLoadLevelStatus", UPNP_ACTION_GET, "retLoadlevelStatus", G_TYPE_UINT},
            {"SetLoadLevelTarget", UPNP_ACTION_POST, "newLoadlevelTarget", G_TYPE_UINT}
        },
        {}
    }
};

static const char* brightnessLevelName = "brightness";

OCEntityHandlerResult UpnpDimming::processGetRequest(OCRepPayload *payload)
{
    if (payload == NULL)
    {
        throw "payload is null";
    }

    //TODO use async version with callback
    int64_t brightnessLevelValue = 0;
    GError *error = NULL;
    if (! gupnp_service_proxy_send_action(m_proxy, "GetLoadLevelStatus", &error,
            // IN args (none)
            NULL,
            // OUT args
            "retLoadlevelStatus", G_TYPE_UINT, &brightnessLevelValue,
            NULL))
    {
        ERROR_PRINT("GetLoadLevelStatus action failed");
        if (error)
        {
            DEBUG_PRINT("Error message: " << error->message);
            g_error_free(error);
        }
        return OC_EH_ERROR;
    }

    if (!OCRepPayloadSetPropInt(payload, brightnessLevelName, brightnessLevelValue))
    {
        throw "Failed to set brightness value in payload";
    }
    DEBUG_PRINT(brightnessLevelName << ": " << brightnessLevelValue);

    return UpnpService::processGetRequest(payload);
}

OCEntityHandlerResult UpnpDimming::processPutRequest(OCEntityHandlerRequest *ehRequest,
        string uri, string resourceType, OCRepPayload *payload)
{
    (void) uri;
    if (!ehRequest || !ehRequest->payload ||
            ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        throw "Incoming payload is NULL or not a representation";
    }

    OCRepPayload *input = reinterpret_cast<OCRepPayload *>(ehRequest->payload);
    if (!input)
    {
        throw "PUT payload is null";
    }

    if (UPNP_OIC_TYPE_BRIGHTNESS == resourceType)
    {
        int64_t brightnessLevelValue = 0;
        if (!OCRepPayloadGetPropInt(input, brightnessLevelName, &brightnessLevelValue))
        {
            throw "No brightness value in request payload";
        }
        DEBUG_PRINT("New " << brightnessLevelName << ": " << brightnessLevelValue);

        //TODO use async version with callback
        GError *error = NULL;
        if (! gupnp_service_proxy_send_action(m_proxy, "SetLoadLevelTarget", &error,
                // IN args
                "newLoadlevelTarget", G_TYPE_UINT, brightnessLevelValue,
                NULL,
                // OUT args (none)
                NULL))
        {
            ERROR_PRINT("SetLoadLevelTarget action failed");
            if (error)
            {
                DEBUG_PRINT("Error message: " << error->message);
                g_error_free(error);
            }
            return OC_EH_ERROR;
        }

        if (!OCRepPayloadSetPropInt(payload, brightnessLevelName, brightnessLevelValue))
        {
            throw "Failed to set brightness value in payload";
        }
        DEBUG_PRINT(brightnessLevelName << ": " << brightnessLevelValue);
    }
    else
    {
        throw "Failed due to unknown resource type";
    }

    return OC_EH_OK;
}
