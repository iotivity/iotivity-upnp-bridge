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

#include "UpnpDimmingService.h"

static const string MODULE = "UpnpDimmingService";

// Brightness service

static const char* brightnessLevelName = "brightness";

void UpnpDimming::processGetRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    DEBUG_PRINT("request->resource->name=" << oc_string(request->resource->name));
    GUPnPServiceInfo *serviceInfo = (GUPnPServiceInfo *)user_data;
    GUPnPServiceProxy *proxy = GUPNP_SERVICE_PROXY(serviceInfo);

    int64_t brightnessLevelValue = 0;
    GError *error = NULL;
    if (! gupnp_service_proxy_send_action(proxy, "GetLoadLevelStatus", &error,
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
        oc_send_response(request, OC_STATUS_INTERNAL_SERVER_ERROR);
        return;
    }

    oc_rep_start_root_object();
    oc_process_baseline_interface(request->resource);
    DEBUG_PRINT("oc_rep_set_int " << brightnessLevelName << ": "  << brightnessLevelValue);
    oc_rep_set_int(root, brightness, brightnessLevelValue);
    oc_rep_end_root_object();
    oc_send_response(request, OC_STATUS_OK);
}

void UpnpDimming::processPostRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    DEBUG_PRINT("request->resource->name=" << oc_string(request->resource->name));
    GUPnPServiceInfo *serviceInfo = (GUPnPServiceInfo *)user_data;
    GUPnPServiceProxy *proxy = GUPNP_SERVICE_PROXY(serviceInfo);

    int64_t brightnessLevelValue = 0;
    oc_rep_t *rep = request->request_payload;
    while (rep) {
        DEBUG_PRINT("key=" << oc_string(rep->name) << ", rep type=" << rep->type);
        char *key = (char *)(rep->name).ptr;
//        DEBUG_PRINT("char *key=" << key);
        if (strncmp(key, brightnessLevelName, (rep->name).size) == 0) {
            switch (rep->type)
            {
                case OC_REP_INT:
                    brightnessLevelValue = rep->value.integer;
                    DEBUG_PRINT("New " << brightnessLevelName << ": " << brightnessLevelValue);
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
    if (! gupnp_service_proxy_send_action(proxy, "SetLoadLevelTarget", &error,
            // IN args
            "newLoadlevelTarget", G_TYPE_BOOLEAN, brightnessLevelValue,
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
        oc_send_response(request, OC_STATUS_INTERNAL_SERVER_ERROR);
        return;
    }

    DEBUG_PRINT("Set " << brightnessLevelName << ": " << brightnessLevelValue);
    oc_send_response(request, OC_STATUS_CHANGED);
}

void UpnpDimming::processPutRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    return processPostRequest(request, interface, user_data);
}
