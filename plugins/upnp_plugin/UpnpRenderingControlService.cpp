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

vector <UpnpAttributeInfo> UpnpRenderingControl::Attributes =
{
};

OCEntityHandlerResult UpnpRenderingControl::processGetRequest(string uri, OCRepPayload *payload, string resourceType)
{
    if (payload == NULL)
    {
        throw "payload is null";
    }
    (void) resourceType;

    //TODO use async version with callback
    bool muteValue = false;
    int64_t volumeValue = 0;
    GError *error = NULL;

    // get mute
    if (! gupnp_service_proxy_send_action(m_proxy, getMuteAction, &error,
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
        return OC_EH_ERROR;
    }

    if (!OCRepPayloadSetPropBool(payload, mutePropertyName, muteValue))
    {
        throw "Failed to set mute value in payload";
    }
    DEBUG_PRINT(mutePropertyName << ": " << (muteValue ? "true" : "false"));

    // get volume
    if (! gupnp_service_proxy_send_action(m_proxy, getVolumeAction, &error,
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
        return OC_EH_ERROR;
    }

    if (!OCRepPayloadSetPropInt(payload, volumePropertyName, volumeValue))
    {
        throw "Failed to set volume value in payload";
    }
    DEBUG_PRINT(volumePropertyName << ": " << volumeValue);

    return UpnpService::processGetRequest(uri, payload, resourceType);
}

OCEntityHandlerResult UpnpRenderingControl::processPutRequest(OCEntityHandlerRequest *ehRequest,
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

    if (UPNP_OIC_TYPE_AUDIO == resourceType)
    {
        //TODO use async version with callback
        GError *error = NULL;

        // set mute
        bool muteValue = false;
        if (OCRepPayloadGetPropBool(input, mutePropertyName, &muteValue))
        {
            DEBUG_PRINT("New " << mutePropertyName << ": " << (muteValue ? "true" : "false"));
            if (! gupnp_service_proxy_send_action(m_proxy, setMuteAction, &error,
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
                return OC_EH_ERROR;
            }

            if (!OCRepPayloadSetPropBool(payload, mutePropertyName, muteValue))
            {
                throw "Failed to set mute value in payload";
            }
            DEBUG_PRINT(mutePropertyName << ": " << (muteValue ? "true" : "false"));
        }

        // set volume
        int64_t volumeValue = 0;
        if (!OCRepPayloadGetPropInt(input, volumePropertyName, &volumeValue))
        {
            DEBUG_PRINT("New " << volumePropertyName << ": " << volumeValue);
            if (! gupnp_service_proxy_send_action(m_proxy, setVolumeAction, &error,
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
                return OC_EH_ERROR;
            }

            if (!OCRepPayloadSetPropBool(payload, volumePropertyName, volumeValue))
            {
                throw "Failed to set volume value in payload";
            }
            DEBUG_PRINT(volumePropertyName << ": " << volumeValue);
        }
    }
    else
    {
        throw "Failed due to unknown resource type";
    }

    return OC_EH_OK;
}
