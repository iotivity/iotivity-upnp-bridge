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

#include "UpnpAvTransportService.h"

static const string MODULE = "UpnpAvTransportService";

// AV Transport Service

static const char *playStatePropertyName = "playState";
static const char *mediaSpeedPropertyName = "mediaSpeed";
static const char *mediaLocationPropertyName = "mediaLocation";
static const char *lastActionPropertyName = "lastAction";
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

vector <UpnpAttributeInfo> UpnpAvTransport::Attributes =
{
};

OCEntityHandlerResult UpnpAvTransport::processGetRequest(string uri, OCRepPayload *payload, string resourceType)
{
    if (payload == NULL)
    {
        throw "payload is null";
    }

    //TODO use async version with callback
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
    if (! gupnp_service_proxy_send_action(m_proxy, getTransportInfoAction, &error,
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
    if (!OCRepPayloadSetPropBool(payload, playStatePropertyName, playStateValue))
    {
        ERROR_PRINT("Failed to set playState value in payload");
    }
    DEBUG_PRINT(playStatePropertyName << ": " << (playStateValue ? "true" : "false"));

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
    if (!OCRepPayloadSetPropDouble(payload, mediaSpeedPropertyName, mediaSpeedValue))
    {
        ERROR_PRINT("Failed to set mediaSpeed value in payload");
    }
    DEBUG_PRINT(mediaSpeedPropertyName << ": " << mediaSpeedValue);

    // get mediaLocation (from Upnp PositionInfo)
    if (! gupnp_service_proxy_send_action(m_proxy, getPositionInfoAction, &error,
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
    if (!OCRepPayloadSetPropString(payload, mediaLocationPropertyName, mediaLocationValue))
    {
        ERROR_PRINT("Failed to set mediaLocation value in payload");
    }
    DEBUG_PRINT(mediaLocationPropertyName << ": " << mediaLocationValue);

    // get actions (from Upnp CurrentTransportActions)
    if (! gupnp_service_proxy_send_action(m_proxy, getCurrentTransportActionsAction, &error,
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
        const OCRepPayload *actionsPayload[actions.size()];
        size_t dimensions[MAX_REP_ARRAY_DEPTH] = {actions.size(), 0, 0};
        for (unsigned int i = 0; i < actions.size(); ++i) {
            DEBUG_PRINT(actionsPropertyName << "[" << i << "]");
            DEBUG_PRINT("\t" << actionPropertyName << "=" << actions[i]);
            OCRepPayload *actionPayload = OCRepPayloadCreate();
            OCRepPayloadSetPropString(actionPayload, actionPropertyName, actions[i]);
            actionsPayload[i] = actionPayload;
        }
        if (! OCRepPayloadSetPropObjectArray(payload, actionsPropertyName, actionsPayload, dimensions))
        {
            ERROR_PRINT("Failed to set actions value in payload");
        }
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

    if (!OCRepPayloadSetPropString(payload, lastActionPropertyName, lastActionValue))
    {
        ERROR_PRINT("Failed to set lastAction value in payload");
    }
    DEBUG_PRINT(lastActionPropertyName << ": " << lastActionValue);

    g_free(currentTransportStateValue);
    g_free(currentTransportStatusValue);
    g_free(currentSpeedValue);

    g_free(trackDurationValue);
    g_free(trackMetadataValue);
    g_free(trackUriValue);
    g_free(relTimeValue);
    g_free(absTimeValue);

    g_free(currentActionsValue);

    return UpnpService::processGetRequest(uri, payload, resourceType);
}

OCEntityHandlerResult UpnpAvTransport::processPutRequest(OCEntityHandlerRequest *ehRequest,
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

    if (UPNP_OIC_TYPE_MEDIA_CONTROL == resourceType)
    {
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
        if (OCRepPayloadGetPropBool(input, playStatePropertyName, &playStateValue))
        {
            DEBUG_PRINT(playStatePropertyName << ": " << (playStateValue ? "true" : "false"));
            playStateValueIsPresent = true;
        }

        if (OCRepPayloadGetPropDouble(input, mediaSpeedPropertyName, &mediaSpeedValue))
        {
            DEBUG_PRINT(mediaSpeedPropertyName << ": " << mediaSpeedValue);
            mediaSpeedValueIsPresent = true;
        }

        if (OCRepPayloadGetPropString(input, mediaLocationPropertyName, &mediaLocationValue))
        {
            DEBUG_PRINT(mediaLocationPropertyName << ": " << mediaLocationValue);
            mediaLocationValueIsPresent = true;
        }

        if (OCRepPayloadGetPropString(input, lastActionPropertyName, &lastActionValue))
        {
            DEBUG_PRINT(lastActionPropertyName << ": " << lastActionValue);
            lastActionValueIsPresent = true;
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
                if (! gupnp_service_proxy_send_action(m_proxy, upnpStopAction, &error,
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
                if (! gupnp_service_proxy_send_action(m_proxy, upnpPauseAction, &error,
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
                if (! gupnp_service_proxy_send_action(m_proxy, upnpPlayAction, &error,
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
                if (! gupnp_service_proxy_send_action(m_proxy, upnpSeekAction, &error,
                    // IN args
                    instanceIdParamName, G_TYPE_UINT, defaultInstanceID,
                    unitParamName, G_TYPE_STRING, unitAbsTime,
                    targetParamName, G_TYPE_STRING, upnpMediaLocation,
                    NULL,
                    // OUT args
                    NULL))
                {
                    // absolute time failed, try again as relative time
                    if (! gupnp_service_proxy_send_action(m_proxy, upnpSeekAction, &error,
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
            else
            {
                // nothing to do
            }

            // load return payload with all values
            usleep(2000000); // wait 2 seconds for upnp transition
            if (OC_EH_OK != processGetRequest(uri, payload, resourceType))
            {
                ERROR_PRINT("Failed to get current values for return payload on put");
            }
        }
        else
        {
            ERROR_PRINT("Payload must have one of playState, mediaSpeed, mediaLocation, or action");
        }
    }
    else
    {
        ERROR_PRINT("Failed due to unknown resource type");
    }

    return OC_EH_OK;
}
