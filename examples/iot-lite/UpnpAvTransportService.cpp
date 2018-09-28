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

#include <algorithm>
#include <cmath>

#include "UpnpAvTransportService.h"

static const string MODULE = "UpnpAvTransportService";

// AV Transport Service

static const char *playStatePropertyName = "play_state";
static const char *mediaSpeedPropertyName = "media_speed";
static const char *mediaLocationPropertyName = "media_location";
static const char *lastActionPropertyName = "action";
static const char *actionsPropertyName = "actions";
static const char *actionPropertyName = "action";

static const char *getCurrentTransportActionsAction = "GetCurrentTransportActions";
static const char *getPositionInfoAction = "GetPositionInfo";
static const char *getTransportInfoAction = "GetTransportInfo";

static const char *instanceIdParamName = "InstanceID";

static const char *actionsParamName = "Actions";

static const char *trackParamName = "Track";
static const char *trackDurationParamName = "TrackDuration";
static const char *trackMetadataParamName = "TrackMetaData";
static const char *trackUriParamName = "TrackURI";
static const char *relTimeParamName = "RelTime";
static const char *absTimeParamName = "AbsTime";
static const char *relCountParamName = "RelCount";
static const char *absCountParamName = "AbsCount";

static const char *currentTransportStateParamName = "CurrentTransportState";
static const char *currentTransportStatusParamName = "CurrentTransportStatus";
static const char *currentSpeedParamName = "CurrentSpeed";

static const char *stopAction = "stop";
static const char *playAction = "play";
static const char *pauseAction = "pause";
static const char *fastforwardAction = "fastforward";
static const char *rewindAction = "rewind";
static const char *stepforwardAction = "stepforward";
static const char *stepbackwardAction = "stepbackward";
static const char *seekAction = "seek";

static const char *upnpStopAction = "Stop";
static const char *upnpPlayAction = "Play";
static const char *upnpPauseAction = "Pause";
static const char *upnpSeekAction = "Seek";

static const char *speedParamName = "Speed";
static const char *unitParamName = "Unit";
static const char *targetParamName = "Target";

static const char *unitAbsTime = "ABS_TIME";
static const char *unitRelTime = "REL_TIME";

static const int defaultInstanceID = 0;

void UpnpAvTransport::processGetRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data, oc_status_t return_status)
{
    DEBUG_PRINT("request->resource->name=" << oc_string(request->resource->name));
    GUPnPServiceInfo *serviceInfo = (GUPnPServiceInfo *)user_data;
    GUPnPServiceProxy *proxy = GUPNP_SERVICE_PROXY(serviceInfo);

    char *currentTransportStateValue = NULL;
    char *currentTransportStatusValue = NULL;
    char *currentSpeedValue = NULL;

    int64_t trackValue = 0;
    char *trackDurationValue = NULL;
    char *trackMetadataValue = NULL;
    char *trackUriValue = NULL;
    char *relTimeValue = NULL;
    char *absTimeValue = NULL;
    int64_t relCountValue = 0;
    int64_t absCountValue = 0;

    char *currentActionsValue = NULL;

    bool playStateValue = false;
    double mediaSpeedValue = 0;
    char *mediaLocationValue = NULL;
    const char *lastActionValue = NULL;

    GError *error = NULL;

    // get playState, mediaSpeed (from Upnp TransportInfo)
    if (! gupnp_service_proxy_send_action(proxy, getTransportInfoAction, &error,
        // IN args
        instanceIdParamName, G_TYPE_UINT, defaultInstanceID,
        NULL,
        // OUT args
        currentTransportStateParamName, G_TYPE_STRING, &currentTransportStateValue,
        currentTransportStatusParamName, G_TYPE_STRING, &currentTransportStatusValue,
        currentSpeedParamName, G_TYPE_STRING, &currentSpeedValue,
        NULL))
    {
        ERROR_PRINT(getTransportInfoAction << " action failed");
        if (error)
        {
            DEBUG_PRINT("Error message: " << error->message);
            g_error_free(error);
        }
    }

    playStateValue = (strstr(currentTransportStateValue, playAction) ||
                      strstr(currentTransportStateValue, upnpPlayAction) ||
                      strstr(currentTransportStateValue, "PLAY"))
                     && (! strstr(currentTransportStateValue, "_PLAY"));

    oc_rep_start_root_object();
    oc_process_baseline_interface(request->resource);
    DEBUG_PRINT("oc_rep_set_boolean " << playStatePropertyName << ": "  << playStateValue);
    oc_rep_set_boolean(root, play_state, playStateValue);


    if (char *delim = strstr(currentSpeedValue, "/"))
    {
        // Upnp TransportPlaySpeed is expressed as a fraction
        char numerator[10];
        char denominator[10];
        memset(numerator, '\0', sizeof(numerator));
        memset(denominator, '\0', sizeof(denominator));
        strncpy(numerator, currentSpeedValue, delim-currentSpeedValue);
        strncpy(denominator, delim+1, strlen(currentSpeedValue)-strlen(numerator)-1);
        mediaSpeedValue = atof(numerator) / atof(denominator);
    }
    else
    {
        mediaSpeedValue = atof(currentSpeedValue);
    }

    DEBUG_PRINT("oc_rep_set_double " << mediaSpeedPropertyName << ": " << mediaSpeedValue);
    oc_rep_set_double(root, media_speed, mediaSpeedValue);

    // get mediaLocation (from Upnp PositionInfo)
    if (! gupnp_service_proxy_send_action(proxy, getPositionInfoAction, &error,
        // IN args
        instanceIdParamName, G_TYPE_UINT, defaultInstanceID,
        NULL,
        // OUT args
        trackParamName, G_TYPE_UINT, &trackValue,
        trackDurationParamName, G_TYPE_STRING, &trackDurationValue,
        trackMetadataParamName, G_TYPE_STRING, &trackMetadataValue,
        trackUriParamName, G_TYPE_STRING, &trackUriValue,
        relTimeParamName, G_TYPE_STRING, &relTimeValue,
        absTimeParamName, G_TYPE_STRING, &absTimeValue,
        relCountParamName, G_TYPE_UINT, &relCountValue,
        absCountParamName, G_TYPE_UINT, &absCountValue,
        NULL))
    {
        ERROR_PRINT(getPositionInfoAction << " action failed");
        if (error)
        {
            DEBUG_PRINT("Error message: " << error->message);
            g_error_free(error);
        }
    }

    mediaLocationValue = relTimeValue;

    DEBUG_PRINT("oc_rep_set_text_string " << mediaLocationPropertyName << ": " << mediaLocationValue);
    oc_rep_set_text_string(root, media_location, mediaLocationValue);

    // get actions (from Upnp CurrentTransportActions)
    if (! gupnp_service_proxy_send_action(proxy, getCurrentTransportActionsAction, &error,
        // IN args
        instanceIdParamName, G_TYPE_UINT, defaultInstanceID,
        NULL,
        // OUT args
        actionsParamName, G_TYPE_STRING, &currentActionsValue,
        NULL))
    {
        ERROR_PRINT(getCurrentTransportActionsAction << " action failed");
        if (error)
        {
            DEBUG_PRINT("Error message: " << error->message);
            g_error_free(error);
        }
    }

    // Upnp returns csv list of action names, needs to be array of oic.r.media.action
    vector<char*> actions;
    char *token;
    token = strtok(currentActionsValue, ",");
    while (token) {
        actions.push_back(token);
        token = strtok(NULL, ",");
    }

    if (!actions.empty())
    {
        oc_string_array_t array;
        oc_new_string_array(&array, actions.size());
        for (unsigned int i = 0; i < actions.size(); ++i) {
            DEBUG_PRINT(actionsPropertyName << "[" << i << "]");
            DEBUG_PRINT("\t" << actionPropertyName << "=" << actions[i]);
            oc_string_array_add_item(array, actions[i]);
        }

        DEBUG_PRINT("oc_string_array allocated size = " << oc_string_array_get_allocated_size(array));
        for (unsigned int i = 0; i < oc_string_array_get_allocated_size(array); ++i) {
            DEBUG_PRINT("array[" << i << "] = " << oc_string_array_get_item(array, i));
        }
        oc_rep_set_string_array(root, actions, array);
        oc_free_string_array(&array);
    }

    // get lastAction (derived from playStateValue, mediaSpeedValue, and currentTransportState)
    if (playStateValue)
    {
        if (mediaSpeedValue > 1)
        {
            lastActionValue = fastforwardAction;
        }
        else if (mediaSpeedValue == 1)
        {
            lastActionValue = playAction;
        }
        else if (mediaSpeedValue > 0)
        {
            lastActionValue = stepforwardAction;
        }
        else if (mediaSpeedValue <= -1)
        {
            lastActionValue = rewindAction;
        }
        else if (mediaSpeedValue < 0)
        {
            lastActionValue = stepbackwardAction;
        }
        else // (mediaSpeedValue == 0)
        {
            // unexpected play with zero speed
            lastActionValue = pauseAction;
        }
    }
    else
    {
        if (strstr(currentTransportStateValue, stopAction) ||
                strstr(currentTransportStateValue, upnpStopAction) || strstr(currentTransportStateValue, "STOP") ||
                strstr(currentTransportStateValue, "NO_MEDIA")) {
            lastActionValue = stopAction;
        }
        else
        {
            lastActionValue = pauseAction;
        }
    }

    DEBUG_PRINT("oc_rep_set_text_string " << lastActionPropertyName << ": " << lastActionValue);
    oc_rep_set_text_string(root, action, lastActionValue);

    g_free(currentTransportStateValue);
    g_free(currentTransportStatusValue);
    g_free(currentSpeedValue);

    g_free(trackDurationValue);
    g_free(trackMetadataValue);
    g_free(trackUriValue);
    g_free(relTimeValue);
    g_free(absTimeValue);

    g_free(currentActionsValue);

    oc_rep_end_root_object();
    oc_send_response(request, return_status);
}

void UpnpAvTransport::processGetRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
	processGetRequest(request, interface, user_data, OC_STATUS_OK);
}

void UpnpAvTransport::processPostRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    DEBUG_PRINT("request->resource->name=" << oc_string(request->resource->name));
    GUPnPServiceInfo *serviceInfo = (GUPnPServiceInfo *)user_data;
    GUPnPServiceProxy *proxy = GUPNP_SERVICE_PROXY(serviceInfo);

    bool playStateValue = false;
    bool playStateValueIsPresent = false;

    double mediaSpeedValue = 0;
    bool mediaSpeedValueIsPresent = false;

    char *mediaLocationValue = NULL;
    bool mediaLocationValueIsPresent = false;

    char *lastActionValue = NULL;
    bool lastActionValueIsPresent = false;

    bool doUpnpStop = false;
    bool doUpnpPlay = false;
    bool doUpnpPause = false;
    bool doUpnpSeek = false;

    int upnpPlaySpeedAsInt = 1;
    int upnpPlaySpeedAsFract = 0;
    char *upnpPlaySpeed = NULL;
    char *upnpMediaLocation = NULL;

    // determine the incoming parameters
    oc_rep_t *rep = request->request_payload;
    while (rep) {
        DEBUG_PRINT("key=" << oc_string(rep->name) << ", rep type=" << rep->type);
        char *key = (char *)(rep->name).ptr;
//        DEBUG_PRINT("char *key=" << key);
        if (strncmp(key, playStatePropertyName, (rep->name).size) == 0) {
            switch (rep->type)
            {
                case OC_REP_BOOL:
                    playStateValue = rep->value.boolean;
                    DEBUG_PRINT("New " << playStatePropertyName << ": " << playStateValue);
                    playStateValueIsPresent = true;
                    break;
                default:
                    DEBUG_PRINT("Unexpected rep type: " << rep->type);
                    oc_send_response(request, OC_STATUS_BAD_REQUEST);
                    return;
            }
        }
        if (strncmp(key, mediaSpeedPropertyName, (rep->name).size) == 0) {
            switch (rep->type)
            {
                case OC_REP_DOUBLE:
                    mediaSpeedValue = rep->value.double_p;
                    DEBUG_PRINT("New " << mediaSpeedPropertyName << ": " << mediaSpeedValue);
                    mediaSpeedValueIsPresent = true;
                    break;
                default:
                    DEBUG_PRINT("Unexpected rep type: " << rep->type);
                    oc_send_response(request, OC_STATUS_BAD_REQUEST);
                    return;
            }
        }
        if (strncmp(key, mediaLocationPropertyName, (rep->name).size) == 0) {
            switch (rep->type)
            {
                case OC_REP_STRING:
                    mediaLocationValue = (char *)(rep->value.string).ptr;
                    DEBUG_PRINT("New " << mediaLocationPropertyName << ": " << mediaLocationValue);
                    mediaLocationValueIsPresent = true;
                    break;
                default:
                    DEBUG_PRINT("Unexpected rep type: " << rep->type);
                    oc_send_response(request, OC_STATUS_BAD_REQUEST);
                    return;
            }
        }
        if (strncmp(key, lastActionPropertyName, (rep->name).size) == 0) {
            switch (rep->type)
            {
                case OC_REP_STRING:
                    lastActionValue = (char *)(rep->value.string).ptr;
                    DEBUG_PRINT("New " << lastActionPropertyName << ": " << lastActionValue);
                    mediaLocationValueIsPresent = true;
                    break;
                default:
                    DEBUG_PRINT("Unexpected rep type: " << rep->type);
                    oc_send_response(request, OC_STATUS_BAD_REQUEST);
                    return;
            }
        }

        rep = rep->next;
    }

    if (playStateValueIsPresent || mediaSpeedValueIsPresent || mediaLocationValueIsPresent || lastActionValueIsPresent)
    {
        // determine upnp action from action parameter (if present)
        if (lastActionValueIsPresent)
        {
            string lowerCase = string(lastActionValue);
            transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(), ::tolower);
            const char *lowerCaseAction = lowerCase.c_str();

            if (strstr(lowerCaseAction, stopAction))
            {
                doUpnpStop = true;
            }
            else if (strstr(lowerCaseAction, playAction))
            {
                doUpnpPlay = true;
            }
            else if (strstr(lowerCaseAction, pauseAction))
            {
                doUpnpPause = true;
            }
            else if (strstr(lowerCaseAction, fastforwardAction))
            {
                doUpnpPlay = true;
                upnpPlaySpeedAsInt = 2;
                // upnpPlaySpeed may be reset below if mediaSpeed is present
            }
            else if (strstr(lowerCaseAction, rewindAction))
            {
                doUpnpPlay = true;
                upnpPlaySpeedAsInt = -1;
                // upnpPlaySpeed may be reset below if mediaSpeed is present
            }
            else if (strstr(lowerCaseAction, stepforwardAction))
            {
                doUpnpSeek = true;
                // TODO: how to handle?
            }
            else if (strstr(lowerCaseAction, stepbackwardAction))
            {
                doUpnpSeek = true;
                // TODO: how to handle?
            }
            else if (strstr(lowerCaseAction, seekAction))
            {
                doUpnpSeek = true;
                // upnp seek location is expected to be set below
            }
            else
            {
                // Unknown action
                ERROR_PRINT("Unknown action " << lastActionValue);
            }
        }

        // determine upnp action from playState parameter (if present)
        if (playStateValueIsPresent)
        {
            if (playStateValue)
            {
                doUpnpPlay = true;
            }
            else
            {
                doUpnpPause = true;
            }
        }

        // determine upnp play speed from mediaSpeed parameter (if present)
        if (mediaSpeedValueIsPresent)
        {
            if ((mediaSpeedValue >= 1) || (mediaSpeedValue <= -1))
            {
                upnpPlaySpeedAsInt = lround(mediaSpeedValue);
                doUpnpPlay = true;
            }
            else if ((mediaSpeedValue > 0) || (mediaSpeedValue < 0))
            {
                upnpPlaySpeedAsFract = lround(10.0 / (mediaSpeedValue * 10.0)); // approx
                doUpnpPlay = true;
            }
            else
            {
                // media play speed is zero
                doUpnpPause = true;
            }
        }

        // determine upnp media location from mediaLocation parameter (if present)
        if (mediaLocationValueIsPresent)
        {
            doUpnpSeek = true;
            upnpMediaLocation = mediaLocationValue;
        }

        // convert mediaSpeed double to upnp play speed string
        char playSpeedString[10];
        memset(playSpeedString, '\0', sizeof(playSpeedString));
        upnpPlaySpeed = playSpeedString;

        if (upnpPlaySpeedAsFract)
        {
            char denominator[10];
            memset(denominator, '\0', sizeof(denominator));
            if (upnpPlaySpeedAsFract < 0)
            {
                char *denomOffset = &denominator[3];
                sprintf(denominator, "%s", "-1/");
                sprintf(denomOffset, "%d", abs(upnpPlaySpeedAsFract));
            }
            else
            {
                char *denomOffset = &denominator[2];
                sprintf(denominator, "%s", "1/");
                sprintf(denomOffset, "%d", upnpPlaySpeedAsFract);
            }
            strcpy(upnpPlaySpeed, denominator);
        }
        else
        {
            char integerValue[10];
            memset(integerValue, '\0', sizeof(integerValue));
            sprintf(integerValue, "%d", upnpPlaySpeedAsInt);
            strcpy(upnpPlaySpeed, integerValue);
        }

        DEBUG_PRINT(speedParamName << ": " << upnpPlaySpeed);
        if (upnpMediaLocation)
        {
            DEBUG_PRINT(targetParamName << ": " << upnpMediaLocation);
        }

        // execute upnp action
        GError *error = NULL;
        if (doUpnpStop)
        {
            if (! gupnp_service_proxy_send_action(proxy, upnpStopAction, &error,
                // IN args
                instanceIdParamName, G_TYPE_UINT, defaultInstanceID,
                NULL,
                // OUT args
                NULL))
            {
                ERROR_PRINT(upnpStopAction << " action failed");
                if (error)
                {
                    DEBUG_PRINT("Error message: " << error->message);
                    g_error_free(error);
                }
            }
        }
        else if (doUpnpPause)
        {
            if (! gupnp_service_proxy_send_action(proxy, upnpPauseAction, &error,
                // IN args
                instanceIdParamName, G_TYPE_UINT, defaultInstanceID,
                NULL,
                // OUT args
                NULL))
            {
                ERROR_PRINT(upnpPauseAction << " action failed");
                if (error)
                {
                    DEBUG_PRINT("Error message: " << error->message);
                    g_error_free(error);
                }
            }
        }
        else if (doUpnpPlay)
        {
            if (! gupnp_service_proxy_send_action(proxy, upnpPlayAction, &error,
                // IN args
                instanceIdParamName, G_TYPE_UINT, defaultInstanceID,
                speedParamName, G_TYPE_STRING, upnpPlaySpeed,
                NULL,
                // OUT args
                NULL))
            {
                ERROR_PRINT(upnpPlayAction << " action failed");
                if (error)
                {
                    DEBUG_PRINT("Error message: " << error->message);
                    g_error_free(error);
                }
            }
        }
        else if (doUpnpSeek)
        {
            if (! gupnp_service_proxy_send_action(proxy, upnpSeekAction, &error,
                // IN args
                instanceIdParamName, G_TYPE_UINT, defaultInstanceID,
                unitParamName, G_TYPE_STRING, unitAbsTime,
                targetParamName, G_TYPE_STRING, upnpMediaLocation,
                NULL,
                // OUT args
                NULL))
            {
                // absolute time failed, try again as relative time
                if (! gupnp_service_proxy_send_action(proxy, upnpSeekAction, &error,
                    // IN args
                    instanceIdParamName, G_TYPE_UINT, defaultInstanceID,
                    unitParamName, G_TYPE_STRING, unitRelTime,
                    targetParamName, G_TYPE_STRING, upnpMediaLocation,
                    NULL,
                    // OUT args
                    NULL))
                {
                    ERROR_PRINT(upnpSeekAction << " action failed");
                    if (error)
                    {
                        DEBUG_PRINT("Error message: " << error->message);
                        g_error_free(error);
                    }
                }
            }
        }
    }

    // load return payload with all values
    processGetRequest(request, interface, user_data, OC_STATUS_CHANGED);
}

void UpnpAvTransport::processPutRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
    return processPostRequest(request, interface, user_data);
}
