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

#include "UpnpPowerSwitchService.h"

static const string MODULE = "UpnpPowerSwitchService";

// Power switch service

static const char* powerSwitchStateName = "value";

void UpnpPowerSwitch::processGetRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    DEBUG_PRINT("request->resource->name=" << oc_string(request->resource->name));
    GUPnPServiceInfo *serviceInfo = (GUPnPServiceInfo *)user_data;
    GUPnPServiceProxy *proxy = GUPNP_SERVICE_PROXY(serviceInfo);

    bool powerSwitchStateValue = false;
    GError *error = NULL;
    if (! gupnp_service_proxy_send_action(proxy, "GetTarget", &error,
            // IN args (none)
            NULL,
            // OUT args
            "RetTargetValue", G_TYPE_BOOLEAN, &powerSwitchStateValue,
            NULL))
    {
        ERROR_PRINT("GetTarget action failed");
        if (error)
        {
            DEBUG_PRINT("Error message: " << error->message);
            g_error_free(error);
        }
        oc_send_response(request, OC_STATUS_INTERNAL_SERVER_ERROR);
        return;
    }

    oc_rep_start_root_object();
    oc_process_baseline_interface(request->resource);
    DEBUG_PRINT("oc_rep_set_boolean " << powerSwitchStateName << " "  << powerSwitchStateValue);
    oc_rep_set_boolean(root, value, powerSwitchStateValue);
    oc_rep_end_root_object();
    oc_send_response(request, OC_STATUS_OK);
}

void UpnpPowerSwitch::processPostRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    DEBUG_PRINT("request->resource->name=" << oc_string(request->resource->name));
    GUPnPServiceInfo *serviceInfo = (GUPnPServiceInfo *)user_data;
    GUPnPServiceProxy *proxy = GUPNP_SERVICE_PROXY(serviceInfo);

    bool powerSwitchStateValue = false;
    oc_rep_t *rep = request->request_payload;
    while (rep) {
        DEBUG_PRINT("key=" << oc_string(rep->name) << ", rep type=" << rep->type);
        char *key = (char *)(rep->name).ptr;
//        DEBUG_PRINT("char *key=" << key);
        if (strncmp(key, powerSwitchStateName, (rep->name).size) == 0) {
            switch (rep->type)
            {
                case OC_REP_BOOL:
                    powerSwitchStateValue = rep->value.boolean;
                    DEBUG_PRINT("New " << powerSwitchStateName << ": " << (powerSwitchStateValue ? "true" : "false"));
                    break;
                default:
                    DEBUG_PRINT("Unexpected rep type: " << rep->type);
                    oc_send_response(request, OC_STATUS_BAD_REQUEST);
                    return;
            }
        }
        rep = rep->next;
    }

    GError *error = NULL;
    if (! gupnp_service_proxy_send_action(proxy, "SetTarget", &error,
            // IN args
            "newTargetValue", G_TYPE_BOOLEAN, powerSwitchStateValue,
            NULL,
            // OUT args (none)
            NULL))
    {
        ERROR_PRINT("SetTarget action failed");
        if (error)
        {
            DEBUG_PRINT("Error message: " << error->message);
            g_error_free(error);
        }
        oc_send_response(request, OC_STATUS_INTERNAL_SERVER_ERROR);
        return;
    }

    DEBUG_PRINT("Set " << powerSwitchStateName << ": " << (powerSwitchStateValue ? "true" : "false"));
    oc_send_response(request, OC_STATUS_CHANGED);
}

void UpnpPowerSwitch::processPutRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    return processPostRequest(request, interface, user_data);
}
