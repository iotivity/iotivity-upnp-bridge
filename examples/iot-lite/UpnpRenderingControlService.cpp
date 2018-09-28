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

#include "UpnpRenderingControlService.h"

static const string MODULE = "UpnpRenderingControlService";

// Rendering Control Service

static const char *mutePropertyName = "mute";
static const char *volumePropertyName = "volume";

static const char *getMuteAction = "GetMute";
static const char *setMuteAction = "SetMute";
static const char *getVolumeAction = "GetVolume";
static const char *setVolumeAction = "SetVolume";

static const char *instanceIdParamName = "InstanceID";
static const char *channelParamName = "Channel";
static const char *currentMuteParamName = "CurrentMute";
static const char *desiredMuteParamName = "DesiredMute";
static const char *currentVolumeParamName = "CurrentVolume";
static const char *desiredVolumeParamName = "DesiredVolume";

static const int defaultInstanceID = 0;
static const char *defaultChannel = "Master";

void UpnpRenderingControl::processGetRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    DEBUG_PRINT("request->resource->name=" << oc_string(request->resource->name));
    GUPnPServiceInfo *serviceInfo = (GUPnPServiceInfo *)user_data;
    GUPnPServiceProxy *proxy = GUPNP_SERVICE_PROXY(serviceInfo);

    bool muteValue = false;
    int volumeValue = 0;
    GError *error = NULL;

    // get mute
    if (! gupnp_service_proxy_send_action(proxy, getMuteAction, &error,
        // IN args
        instanceIdParamName, G_TYPE_UINT, defaultInstanceID,
        channelParamName, G_TYPE_STRING, defaultChannel,
        NULL,
        // OUT args
        currentMuteParamName, G_TYPE_BOOLEAN, &muteValue,
        NULL))
    {
        ERROR_PRINT(getMuteAction << " action failed");
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
    DEBUG_PRINT("oc_rep_set_boolean " << mutePropertyName << ": "  << muteValue);
    oc_rep_set_boolean(root, mute, muteValue);

    // get volume
    if (! gupnp_service_proxy_send_action(proxy, getVolumeAction, &error,
        // IN args
        instanceIdParamName, G_TYPE_UINT, defaultInstanceID,
        channelParamName, G_TYPE_STRING, defaultChannel,
        NULL,
        // OUT args
        currentVolumeParamName, G_TYPE_UINT, &volumeValue,
        NULL))
    {
        ERROR_PRINT(getVolumeAction << " action failed");
        if (error)
        {
            DEBUG_PRINT("Error message: " << error->message);
            g_error_free(error);
        }
        oc_send_response(request, OC_STATUS_INTERNAL_SERVER_ERROR);
        return;
    }

    DEBUG_PRINT("oc_rep_set_int " << volumePropertyName << ": " << volumeValue);
    oc_rep_set_int(root, volume, volumeValue);
    oc_rep_end_root_object();
    oc_send_response(request, OC_STATUS_OK);
}

void UpnpRenderingControl::processPostRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    DEBUG_PRINT("request->resource->name=" << oc_string(request->resource->name));
    GUPnPServiceInfo *serviceInfo = (GUPnPServiceInfo *)user_data;
    GUPnPServiceProxy *proxy = GUPNP_SERVICE_PROXY(serviceInfo);

    bool muteValue = false;
    int volumeValue = 0;
    oc_rep_t *rep = request->request_payload;
    while (rep) {
        DEBUG_PRINT("key=" << oc_string(rep->name) << ", rep type=" << rep->type);
        char *key = (char *)(rep->name).ptr;
//        DEBUG_PRINT("char *key=" << key);
        if (strncmp(key, mutePropertyName, (rep->name).size) == 0) {
            switch (rep->type)
            {
                case OC_REP_BOOL:
                    muteValue = rep->value.boolean;
                    DEBUG_PRINT("New " << mutePropertyName << ": " << muteValue);
                    break;
                default:
                    DEBUG_PRINT("Unexpected rep type: " << rep->type);
                    oc_send_response(request, OC_STATUS_BAD_REQUEST);
                    return;
            }
        }
        if (strncmp(key, volumePropertyName, (rep->name).size) == 0) {
            switch (rep->type)
            {
                case OC_REP_INT:
                    volumeValue = rep->value.integer;
                    DEBUG_PRINT("New " << volumePropertyName << ": " << volumeValue);
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

    // set mute
    if (!gupnp_service_proxy_send_action(proxy, setMuteAction, &error,
            // IN args
            instanceIdParamName, G_TYPE_UINT, defaultInstanceID,
            channelParamName, G_TYPE_STRING, defaultChannel,
            desiredMuteParamName, G_TYPE_BOOLEAN, muteValue,
            NULL,
            // OUT args (none)
            NULL))
    {
        ERROR_PRINT(setMuteAction << " action failed");
        if (error)
        {
            DEBUG_PRINT("Error message: " << error->message);
            g_error_free(error);
        }
        oc_send_response(request, OC_STATUS_INTERNAL_SERVER_ERROR);
        return;
    }
    DEBUG_PRINT("Set " << mutePropertyName << ": " << muteValue);

    // set volume
    if (!gupnp_service_proxy_send_action(proxy, setVolumeAction, &error,
            // IN args
            instanceIdParamName, G_TYPE_UINT, defaultInstanceID,
            channelParamName, G_TYPE_STRING, defaultChannel,
            desiredVolumeParamName, G_TYPE_UINT, volumeValue,
            NULL,
            // OUT args (none)
            NULL))
    {
        ERROR_PRINT(setVolumeAction << " action failed");
        if (error)
        {
            DEBUG_PRINT("Error message: " << error->message);
            g_error_free(error);
        }
        oc_send_response(request, OC_STATUS_INTERNAL_SERVER_ERROR);
        return;
    }
    DEBUG_PRINT("Set " << volumePropertyName << ": " << volumeValue);

    oc_send_response(request, OC_STATUS_CHANGED);
}


void UpnpRenderingControl::processPutRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    return processPostRequest(request, interface, user_data);
}
