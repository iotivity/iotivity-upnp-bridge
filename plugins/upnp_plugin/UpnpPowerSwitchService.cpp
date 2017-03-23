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
#include <octypes.h>
#include <mpmErrorCode.h>
#include <ConcurrentIotivityUtils.h>

using namespace OC::Bridging;

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

// TODO move to location where more services have access to helper function
bool isSecureEnvironmentSet()
{
    char *non_secure_env = getenv("NONSECURE");

    if (non_secure_env != NULL && (strcmp(non_secure_env, "true") == 0))
    {
        DEBUG_PRINT("Creating NON SECURE resources");
        return false;
    }
    DEBUG_PRINT("Creating SECURE resources");
    return true;
}

OCEntityHandlerResult resourceEntityHandler(OCEntityHandlerFlag ,
        OCEntityHandlerRequest *request,
        void *cb)
{
    DEBUG_PRINT("");
    uintptr_t callBackParamResourceType = (uintptr_t) cb;
    OCEntityHandlerResult result = OC_EH_OK;
    MPMResult res = MPM_RESULT_OK;

    try
    {
        switch (request->method)
        {
            case OC_REST_GET:
                DEBUG_PRINT("OC_REST_GET");
//                OCRepPayload *responsePayload = processGetRequest(targetLight, callBackParamResourceType);
//                ConcurrentIotivityUtils::respondToRequest(request, responsePayload, result);
//                OCRepPayloadDestroy(responsePayload);
                break;

            case OC_REST_PUT:
            case OC_REST_POST:
                DEBUG_PRINT("OC_REST_POST/OIC_REST_POST");
//                res = (MPMResult)processPutRequest((OCRepPayload *) request->payload, targetLight,
//                                                   callBackParamResourceType);
                if (res != MPM_RESULT_OK)
                    result = OC_EH_ERROR;
                break;

            default:
                DEBUG_PRINT("default");
                DEBUG_PRINT("Unsupported method (" << request->method << ") recieved");
                ConcurrentIotivityUtils::respondToRequestWithError(request, "Unsupported method received",
                        OC_EH_METHOD_NOT_ALLOWED);
                return OC_EH_OK;
        }
    }
    catch (const std::exception &exp)
    {
        ConcurrentIotivityUtils::respondToRequestWithError(request, exp.what(), OC_EH_ERROR);
        return OC_EH_OK;
    }

    return OC_EH_OK;
}

void UpnpPowerSwitch::onAdd() {
    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);
    if (isSecureEnvironmentSet())
    {
        resourceProperties |= OC_SECURE;
    }

    ConcurrentIotivityUtils::queueCreateResource(m_uri, UPNP_OIC_TYPE_POWER_SWITCH, OC_RSRVD_INTERFACE_ACTUATOR,
            resourceEntityHandler,
            (void *) 0, resourceProperties);

}
