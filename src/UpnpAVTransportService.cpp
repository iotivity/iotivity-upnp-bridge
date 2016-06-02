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

#include "UpnpAVTransportService.h"

using namespace OIC::Service;

static const string MODULE = "UpnpAVTransportService";

// AV Transport Service

// Organization:
// Attribute Name,
// State Variable Name (can be empty string), State variable type, "evented" flag
// Actions:
//    0: "GET" action name, action type, optional out parameters: var_name,var_type
//    1: "SET" action name, action type, optional in parameters: var_name,var_type
// Vector of embedded attributes (if present)
vector <UpnpAttributeInfo> UpnpAVTransport::Attributes =
{
    {
        "lastChange",
        "LastChange", G_TYPE_STRING, true,
        {},
        {}
    },
    {
        "currentTransportActions",
        "CurrentTransportActions", G_TYPE_STRING, false,
        {{"GetCurrentTransportActions", UPNP_ACTION_GET, "CurrentTransportActions", G_TYPE_STRING}},
        {}
    },
    {
        "deviceCapabilities",
        "", G_TYPE_NONE, false,
        {{"GetDeviceCapabilities", UPNP_ACTION_GET, "", G_TYPE_NONE}},
        {
            {"playMedia", "PlayMedia", G_TYPE_STRING, false},
            {"recMedia", "RecMedia", G_TYPE_STRING, false},
            {"recQualityModes", "RecQualityModes", G_TYPE_STRING, false}
        }
    },
    {
        "mediaInfo",
        "", G_TYPE_NONE, false,
        {{"GetMediaInfo", UPNP_ACTION_GET, "", G_TYPE_NONE}},
        {
            {"nrTracks", "NrTracks", G_TYPE_UINT, false},
            {"mediaDuration", "MediaDuration", G_TYPE_STRING, false},
            {"currentUri", "CurrentURI", G_TYPE_STRING, false},
            {"currentUriMetadata", "CurrentURIMetaData", G_TYPE_STRING, false},
            {"nextUri", "NextURI", G_TYPE_STRING, false},
            {"nextUriMetadata", "NextURIMetaData", G_TYPE_STRING, false},
            {"playMedium", "PlayMedium", G_TYPE_STRING, false},
            {"recordMedium", "RecordMedium", G_TYPE_STRING, false},
            {"writeStatus", "WriteStatus", G_TYPE_STRING, false}
        }
    },
    {
        "positionInfo",
        "", G_TYPE_NONE, false,
        {{"GetPositionInfo", UPNP_ACTION_GET, "", G_TYPE_NONE}},
        {
            {"track", "Track", G_TYPE_UINT, false},
            {"trackDuration", "TrackDuration", G_TYPE_STRING, false},
            {"trackMetadata", "TrackMetaData", G_TYPE_STRING, false},
            {"trackUri", "TrackURI", G_TYPE_STRING, false},
            {"relTime", "RelTime", G_TYPE_STRING, false},
            {"absTime", "AbsTime", G_TYPE_STRING, false},
            {"relCount", "RelCount", G_TYPE_UINT, false},
            {"absCount", "AbsCount", G_TYPE_UINT, false}
        }
    },
    {
        "transportInfo",
        "", G_TYPE_NONE, false,
        {{"GetTransportInfo", UPNP_ACTION_GET, "", G_TYPE_NONE}},
        {
            {"transportState", "CurrentTransportState", G_TYPE_STRING, false},
            {"transportStatus", "CurrentTransportStatus", G_TYPE_STRING, false},
            {"speed", "CurrentSpeed", G_TYPE_STRING, false}
        }
    },
    {
        "transportSettings",
        "", G_TYPE_NONE, false,
        {{"GetTransportSettings", UPNP_ACTION_GET, "", G_TYPE_NONE}},
        {
            {"playMode", "PlayMode", G_TYPE_STRING, false},
            {"recQualityMode", "RecQualityMode", G_TYPE_STRING, false}
        }
    },
    {
        "avTransportUri",
        "", G_TYPE_NONE, false,
        {
            {"", UPNP_ACTION_GET, "", G_TYPE_NONE},
            {"SetAVTransportURI", UPNP_ACTION_POST, "", G_TYPE_NONE}
        },
        {
            {"instanceId", "InstanceID", G_TYPE_UINT, false},
            {"currentUri", "CurrentURI", G_TYPE_STRING, false},
            {"currentUriMetadata", "CurrentURIMetaData", G_TYPE_STRING, false}
        }
    },
    {
        "nextAvTransportUri",
        "", G_TYPE_NONE, false,
        {
            {"", UPNP_ACTION_GET, "", G_TYPE_NONE},
            {"SetNextAVTransportURI", UPNP_ACTION_POST, "", G_TYPE_NONE}
        },
        {
            {"instanceId", "InstanceID", G_TYPE_UINT, false},
            {"nextUri", "NextURI", G_TYPE_STRING, false},
            {"nextUriMetadata", "NextURIMetaData", G_TYPE_STRING, false}
        }
    },
    {
        "playMode",
        "", G_TYPE_NONE, false,
        {
            {"", UPNP_ACTION_GET, "", G_TYPE_NONE},
            {"SetPlayMode", UPNP_ACTION_POST, "", G_TYPE_NONE}
        },
        {
            {"instanceId", "InstanceID", G_TYPE_UINT, false},
            {"newPlayMode", "NewPlayMode", G_TYPE_STRING, false}
        }
    },
    {
        "next",
        "", G_TYPE_NONE, false,
        {
            {"", UPNP_ACTION_GET, "", G_TYPE_NONE},
            {"Next", UPNP_ACTION_POST, "", G_TYPE_NONE}
        },
        {
            {"instanceId", "InstanceID", G_TYPE_UINT, false}
        }
    },
    {
        "play",
        "", G_TYPE_NONE, false,
        {
            {"", UPNP_ACTION_GET, "", G_TYPE_NONE},
            {"Play", UPNP_ACTION_POST, "", G_TYPE_NONE}
        },
        {
            {"instanceId", "InstanceID", G_TYPE_UINT, false},
            {"speed", "Speed", G_TYPE_STRING, false}
        }
    },
    {
        "pause",
        "", G_TYPE_NONE, false,
        {
            {"", UPNP_ACTION_GET, "", G_TYPE_NONE},
            {"Pause", UPNP_ACTION_POST, "", G_TYPE_NONE}
        },
        {
            {"instanceId", "InstanceID", G_TYPE_UINT, false}
        }
    },
    {
        "previous",
        "", G_TYPE_NONE, false,
        {
            {"", UPNP_ACTION_GET, "", G_TYPE_NONE},
            {"Previous", UPNP_ACTION_POST, "", G_TYPE_NONE}
        },
        {
            {"instanceId", "InstanceID", G_TYPE_UINT, false}
        }
    },
    {
        "seek",
        "", G_TYPE_NONE, false,
        {
            {"", UPNP_ACTION_GET, "", G_TYPE_NONE},
            {"Seek", UPNP_ACTION_POST, "", G_TYPE_NONE}
        },
        {
            {"instanceId", "InstanceID", G_TYPE_UINT, false},
            {"unit", "Unit", G_TYPE_STRING, false},
            {"target", "Target", G_TYPE_STRING, false}
        }
    },
    {
        "stop",
        "", G_TYPE_NONE, false,
        {
            {"", UPNP_ACTION_GET, "", G_TYPE_NONE},
            {"Stop", UPNP_ACTION_POST, "", G_TYPE_NONE}
        },
        {
            {"instanceId", "InstanceID", G_TYPE_UINT, false}
        }
    },
};

// Custom action maps:
// "attribute name" -> GET request handlers
map <const string, UpnpAVTransport::GetAttributeHandler>
    UpnpAVTransport::GetAttributeActionMap =
{
    {"currentTransportActions", &UpnpAVTransport::getCurrentTransportActions},
    {"deviceCapabilities", &UpnpAVTransport::getDeviceCapabilities},
    {"mediaInfo", &UpnpAVTransport::getMediaInfo},
    {"positionInfo", &UpnpAVTransport::getPositionInfo},
    {"transportInfo", &UpnpAVTransport::getTransportInfo},
    {"transportSettings", &UpnpAVTransport::getTransportSettings},
};

// "attribute name" -> SET request handlers
map <const string, UpnpAVTransport::SetAttributeHandler>
    UpnpAVTransport::SetAttributeActionMap =
{
    {"avTransportUri", &UpnpAVTransport::setAvTransportUri},
    {"nextAvTransportUri", &UpnpAVTransport::setNextAvTransportUri},
    {"playMode", &UpnpAVTransport::setPlayMode},
    {"next", &UpnpAVTransport::next},
    {"pause", &UpnpAVTransport::pause},
    {"play", &UpnpAVTransport::play},
    {"previous", &UpnpAVTransport::previous},
    {"seek", &UpnpAVTransport::seek},
    {"stop", &UpnpAVTransport::stop},
};

// TODO Implement additional OCF attributes/UPnP Actions as necessary

void UpnpAVTransport::getCurrentTransportActionsCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;
    char *actions;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "Actions",
                                                  G_TYPE_STRING,
                                                  &actions,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetCurrentTransportActions failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes currentTransportActions;

        DEBUG_PRINT("Current Transport Actions actions=" << actions);

        currentTransportActions["actions"] = string(actions);

        g_free(actions);

        request->resource->setAttribute("currentTransportActions", currentTransportActions, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::getCurrentTransportActions(UpnpRequest *request, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;

    if (! queryParams.empty()) {
        auto it = queryParams.find("instanceId");
        if (it != queryParams.end()) {
            try {
                instanceId = std::stoi(it->second);
                instanceId = std::max(0, instanceId);
                DEBUG_PRINT("getCurrentTransportActions queryParam " << it->first << "=" << it->second);
            } catch (const std::invalid_argument& ia) {
                ERROR_PRINT("Invalid getCurrentTransportActions queryParam " << it->first << "=" << it->second);
            }
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "GetCurrentTransportActions",
                                          getCurrentTransportActionsCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["currentTransportActions"].first;
    return true;
}

void UpnpAVTransport::getDeviceCapabilitiesCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;
    char *playMedia;
    char *recMedia;
    char *recQualityModes;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "PlayMedia",
                                                  G_TYPE_STRING,
                                                  &playMedia,
                                                  "RecMedia",
                                                  G_TYPE_STRING,
                                                  &recMedia,
                                                  "RecQualityModes",
                                                  G_TYPE_STRING,
                                                  &recQualityModes,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetDeviceCapabilities failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes deviceCapabilities;

        DEBUG_PRINT("Device Capabilities playMedia=" << playMedia);
        DEBUG_PRINT("Device Capabilities recMedia=" << recMedia);
        DEBUG_PRINT("Device Capabilities recQualityModes=" << recQualityModes);

        deviceCapabilities["playMedia"] = string(playMedia);
        deviceCapabilities["recMedia"] = string(recMedia);
        deviceCapabilities["recQualityModes"] = string(recQualityModes);

        g_free(playMedia);
        g_free(recMedia);
        g_free(recQualityModes);

        request->resource->setAttribute("deviceCapabilities", deviceCapabilities, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::getDeviceCapabilities(UpnpRequest *request, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;

    if (! queryParams.empty()) {
        auto it = queryParams.find("instanceId");
        if (it != queryParams.end()) {
            try {
                instanceId = std::stoi(it->second);
                instanceId = std::max(0, instanceId);
                DEBUG_PRINT("getDeviceCapabilities queryParam " << it->first << "=" << it->second);
            } catch (const std::invalid_argument& ia) {
                ERROR_PRINT("Invalid getDeviceCapabilities queryParam " << it->first << "=" << it->second);
            }
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "GetDeviceCapabilities",
                                          getDeviceCapabilitiesCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["deviceCapabilities"].first;
    return true;
}

void UpnpAVTransport::getMediaInfoCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;
    int nrTracks;
    char *mediaDuration;
    char *currentUri;
    char *currentUriMetadata;
    char *nextUri;
    char *nextUriMetadata;
    char *playMedium;
    char *recordMedium;
    char *writeStatus;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "NrTracks",
                                                  G_TYPE_UINT,
                                                  &nrTracks,
                                                  "MediaDuration",
                                                  G_TYPE_STRING,
                                                  &mediaDuration,
                                                  "CurrentURI",
                                                  G_TYPE_STRING,
                                                  &currentUri,
                                                  "CurrentURIMetaData",
                                                  G_TYPE_STRING,
                                                  &currentUriMetadata,
                                                  "NextURI",
                                                  G_TYPE_STRING,
                                                  &nextUri,
                                                  "NextURIMetaData",
                                                  G_TYPE_STRING,
                                                  &nextUriMetadata,
                                                  "PlayMedium",
                                                  G_TYPE_STRING,
                                                  &playMedium,
                                                  "RecordMedium",
                                                  G_TYPE_STRING,
                                                  &recordMedium,
                                                  "WriteStatus",
                                                  G_TYPE_STRING,
                                                  &writeStatus,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetMediaInfo failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes mediaInfo;

        DEBUG_PRINT("Media Info nrTracks=" << nrTracks);
        DEBUG_PRINT("Media Info mediaDuration=" << mediaDuration);
        DEBUG_PRINT("Media Info currentUri=" << currentUri);
        DEBUG_PRINT("Media Info currentUriMetadata=" << currentUriMetadata);
        DEBUG_PRINT("Media Info nextUri=" << nextUri);
        DEBUG_PRINT("Media Info nextUriMetadata=" << nextUriMetadata);
        DEBUG_PRINT("Media Info playMedium=" << playMedium);
        DEBUG_PRINT("Media Info recordMedium=" << recordMedium);
        DEBUG_PRINT("Media Info writeStatus=" << writeStatus);

        mediaInfo["nrTracks"] = nrTracks;
        mediaInfo["mediaDuration"] = string(mediaDuration);
        mediaInfo["currentUri"] = string(currentUri);
        mediaInfo["currentUriMetadata"] = string(currentUriMetadata);
        mediaInfo["nextUri"] = string(nextUri);
        mediaInfo["nextUriMetadata"] = string(nextUriMetadata);
        mediaInfo["playMedium"] = string(playMedium);
        mediaInfo["recordMedium"] = string(recordMedium);
        mediaInfo["writeStatus"] = string(writeStatus);

        g_free(mediaDuration);
        g_free(currentUri);
        g_free(currentUriMetadata);
        g_free(nextUri);
        g_free(nextUriMetadata);
        g_free(playMedium);
        g_free(recordMedium);
        g_free(writeStatus);

        request->resource->setAttribute("mediaInfo", mediaInfo, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::getMediaInfo(UpnpRequest *request, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;

    if (! queryParams.empty()) {
        auto it = queryParams.find("instanceId");
        if (it != queryParams.end()) {
            try {
                instanceId = std::stoi(it->second);
                instanceId = std::max(0, instanceId);
                DEBUG_PRINT("getMediaInfo queryParam " << it->first << "=" << it->second);
            } catch (const std::invalid_argument& ia) {
                ERROR_PRINT("Invalid getMediaInfo queryParam " << it->first << "=" << it->second);
            }
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "GetMediaInfo",
                                          getMediaInfoCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["mediaInfo"].first;
    return true;
}

void UpnpAVTransport::getPositionInfoCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;
    int track;
    char *trackDuration;
    char *trackMetadata;
    char *trackUri;
    char *relTime;
    char *absTime;
    int relCount;
    int absCount;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "Track",
                                                  G_TYPE_UINT,
                                                  &track,
                                                  "TrackDuration",
                                                  G_TYPE_STRING,
                                                  &trackDuration,
                                                  "TrackMetaData",
                                                  G_TYPE_STRING,
                                                  &trackMetadata,
                                                  "TrackURI",
                                                  G_TYPE_STRING,
                                                  &trackUri,
                                                  "RelTime",
                                                  G_TYPE_STRING,
                                                  &relTime,
                                                  "AbsTime",
                                                  G_TYPE_STRING,
                                                  &absTime,
                                                  "RelCount",
                                                  G_TYPE_UINT,
                                                  &relCount,
                                                  "AbsCount",
                                                  G_TYPE_UINT,
                                                  &absCount,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetPositionInfo failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes positionInfo;

        DEBUG_PRINT("Position Info track=" << track);
        DEBUG_PRINT("Position Info trackDuration=" << trackDuration);
        DEBUG_PRINT("Position Info trackMetadata=" << trackMetadata);
        DEBUG_PRINT("Position Info trackUri=" << trackUri);
        DEBUG_PRINT("Position Info relTime=" << relTime);
        DEBUG_PRINT("Position Info absTime=" << absTime);
        DEBUG_PRINT("Position Info relCount=" << relCount);
        DEBUG_PRINT("Position Info absCount=" << absCount);

        positionInfo["track"] = track;
        positionInfo["trackDuration"] = string(trackDuration);
        positionInfo["trackMetadata"] = string(trackMetadata);
        positionInfo["trackUri"] = string(trackUri);
        positionInfo["relTime"] = string(relTime);
        positionInfo["absTime"] = string(absTime);
        positionInfo["relCount"] = relCount;
        positionInfo["absCount"] = absCount;

        g_free(trackDuration);
        g_free(trackMetadata);
        g_free(trackUri);
        g_free(relTime);
        g_free(absTime);

        request->resource->setAttribute("positionInfo", positionInfo, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::getPositionInfo(UpnpRequest *request, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;

    if (! queryParams.empty()) {
        auto it = queryParams.find("instanceId");
        if (it != queryParams.end()) {
            try {
                instanceId = std::stoi(it->second);
                instanceId = std::max(0, instanceId);
                DEBUG_PRINT("getPositionInfo queryParam " << it->first << "=" << it->second);
            } catch (const std::invalid_argument& ia) {
                ERROR_PRINT("Invalid getPositionInfo queryParam " << it->first << "=" << it->second);
            }
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "GetPositionInfo",
                                          getPositionInfoCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["positionInfo"].first;
    return true;
}

void UpnpAVTransport::getTransportInfoCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;
    char *transportState;
    char *transportStatus;
    char *speed;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "CurrentTransportState",
                                                  G_TYPE_STRING,
                                                  &transportState,
                                                  "CurrentTransportStatus",
                                                  G_TYPE_STRING,
                                                  &transportStatus,
                                                  "CurrentSpeed",
                                                  G_TYPE_STRING,
                                                  &speed,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetTransportInfo failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes transportInfo;

        DEBUG_PRINT("Transport Info transportState=" << transportState);
        DEBUG_PRINT("Transport Info transportStatus=" << transportStatus);
        DEBUG_PRINT("Transport Info speed=" << speed);

        transportInfo["transportState"] = string(transportState);
        transportInfo["transportStatus"] = string(transportStatus);
        transportInfo["speed"] = string(speed);

        g_free(transportState);
        g_free(transportStatus);
        g_free(speed);

        request->resource->setAttribute("transportInfo", transportInfo, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::getTransportInfo(UpnpRequest *request, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;

    if (! queryParams.empty()) {
        auto it = queryParams.find("instanceId");
        if (it != queryParams.end()) {
            try {
                instanceId = std::stoi(it->second);
                instanceId = std::max(0, instanceId);
                DEBUG_PRINT("getTransportInfo queryParam " << it->first << "=" << it->second);
            } catch (const std::invalid_argument& ia) {
                ERROR_PRINT("Invalid getTransportInfo queryParam " << it->first << "=" << it->second);
            }
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "GetTransportInfo",
                                          getTransportInfoCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["transportInfo"].first;
    return true;
}

void UpnpAVTransport::getTransportSettingsCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;
    char *playMode;
    char *recQualityMode;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "PlayMode",
                                                  G_TYPE_STRING,
                                                  &playMode,
                                                  "RecQualityMode",
                                                  G_TYPE_STRING,
                                                  &recQualityMode,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetTransportSettings failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes transportSettings;

        DEBUG_PRINT("Transport Settings playMode=" << playMode);
        DEBUG_PRINT("Transport Settings recQualityMode=" << recQualityMode);

        transportSettings["playMode"] = string(playMode);
        transportSettings["recQualityMode"] = string(recQualityMode);

        g_free(playMode);
        g_free(recQualityMode);

        request->resource->setAttribute("transportSettings", transportSettings, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::getTransportSettings(UpnpRequest *request, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;

    if (! queryParams.empty()) {
        auto it = queryParams.find("instanceId");
        if (it != queryParams.end()) {
            try {
                instanceId = std::stoi(it->second);
                instanceId = std::max(0, instanceId);
                DEBUG_PRINT("getTransportSettings queryParam " << it->first << "=" << it->second);
            } catch (const std::invalid_argument& ia) {
                ERROR_PRINT("Invalid getTransportSettings queryParam " << it->first << "=" << it->second);
            }
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "GetTransportSettings",
                                          getTransportSettingsCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["transportSettings"].first;
    return true;
}

void UpnpAVTransport::setAvTransportUriCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("setAvTransportUri failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::setAvTransportUri(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;
    string currentUri = "";
    string currentUriMetadata = "";

    const auto &attrs = value->get< RCSResourceAttributes >();

    for (const auto &kvPair : attrs)
    {
        DEBUG_PRINT("setAvTransportUri kvPair key=" << kvPair.key());
        DEBUG_PRINT("setAvTransportUri kvPair value=" << kvPair.value().toString());

        if (kvPair.key() == "instanceId")
        {
            instanceId = (kvPair.value()).get< int >();
            instanceId = std::max(0, instanceId);
            DEBUG_PRINT("setAvTransportUri instanceId=" << instanceId);
        }

        if (kvPair.key() == "currentUri")
        {
            currentUri = (kvPair.value()).get< string >();
            DEBUG_PRINT("setAvTransportUri currentUri=" << currentUri);
        }

        if (kvPair.key() == "currentUriMetadata")
        {
            currentUriMetadata = (kvPair.value()).get< string >();
            DEBUG_PRINT("setAvTransportUri currentUriMetadata=" << currentUriMetadata);
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "SetAvTransportUri",
                                          playCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          "CurrentURI",
                                          G_TYPE_STRING,
                                          currentUri.c_str(),
                                          "CurrentURIMetaData",
                                          G_TYPE_STRING,
                                          currentUriMetadata.c_str(),
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["setAvTransportUri"].first;

    return true;
}

void UpnpAVTransport::setNextAvTransportUriCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("SetNextAvTransportUri failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::setNextAvTransportUri(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;
    string nextUri = "";
    string nextUriMetadata = "";

    const auto &attrs = value->get< RCSResourceAttributes >();

    for (const auto &kvPair : attrs)
    {
        DEBUG_PRINT("setNextAvTransportUri kvPair key=" << kvPair.key());
        DEBUG_PRINT("setNextAvTransportUri kvPair value=" << kvPair.value().toString());

        if (kvPair.key() == "instanceId")
        {
            instanceId = (kvPair.value()).get< int >();
            instanceId = std::max(0, instanceId);
            DEBUG_PRINT("setNextAvTransportUri instanceId=" << instanceId);
        }

        if (kvPair.key() == "nextUri")
        {
            nextUri = (kvPair.value()).get< string >();
            DEBUG_PRINT("setNextAvTransportUri nextUri=" << nextUri);
        }

        if (kvPair.key() == "nextUriMetadata")
        {
            nextUriMetadata = (kvPair.value()).get< string >();
            DEBUG_PRINT("setNextAvTransportUri nextUriMetadata=" << nextUriMetadata);
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "SetNextAvTransportUri",
                                          playCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          "NextURI",
                                          G_TYPE_STRING,
                                          nextUri.c_str(),
                                          "NextURIMetaData",
                                          G_TYPE_STRING,
                                          nextUriMetadata.c_str(),
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["setNextAvTransportUri"].first;

    return true;
}

void UpnpAVTransport::setPlayModeCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("SetPlayMode failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::setPlayMode(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;
    string newPlayMode = "NORMAL";

    const auto &attrs = value->get< RCSResourceAttributes >();

    for (const auto &kvPair : attrs)
    {
        DEBUG_PRINT("setPlayMode kvPair key=" << kvPair.key());
        DEBUG_PRINT("setPlayMode kvPair value=" << kvPair.value().toString());

        if (kvPair.key() == "instanceId")
        {
            instanceId = (kvPair.value()).get< int >();
            instanceId = std::max(0, instanceId);
            DEBUG_PRINT("setPlayMode instanceId=" << instanceId);
        }

        if (kvPair.key() == "newPlayMode")
        {
            newPlayMode = (kvPair.value()).get< string >();
            DEBUG_PRINT("setPlayMode newPlayMode=" << newPlayMode);
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "SetPlayMode",
                                          playCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          "NewPlayMode",
                                          G_TYPE_STRING,
                                          newPlayMode.c_str(),
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["setPlayMode"].first;

    return true;
}

void UpnpAVTransport::nextCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("Next failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::next(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;

    const auto &attrs = value->get< RCSResourceAttributes >();

    for (const auto &kvPair : attrs)
    {
        DEBUG_PRINT("next kvPair key=" << kvPair.key());
        DEBUG_PRINT("next kvPair value=" << kvPair.value().toString());

        if (kvPair.key() == "instanceId")
        {
            instanceId = (kvPair.value()).get< int >();
            instanceId = std::max(0, instanceId);
            DEBUG_PRINT("next instanceId=" << instanceId);
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "Next",
                                          nextCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["next"].first;

    return true;
}

void UpnpAVTransport::playCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("Play failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::play(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;
    string speed = "1";

    const auto &attrs = value->get< RCSResourceAttributes >();

    for (const auto &kvPair : attrs)
    {
        DEBUG_PRINT("play kvPair key=" << kvPair.key());
        DEBUG_PRINT("play kvPair value=" << kvPair.value().toString());

        if (kvPair.key() == "instanceId")
        {
            instanceId = (kvPair.value()).get< int >();
            instanceId = std::max(0, instanceId);
            DEBUG_PRINT("play instanceId=" << instanceId);
        }

        if (kvPair.key() == "speed")
        {
            speed = (kvPair.value()).get< string >();
            DEBUG_PRINT("play speed=" << speed);
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "Play",
                                          playCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          "Speed",
                                          G_TYPE_STRING,
                                          speed.c_str(),
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["play"].first;

    return true;
}

void UpnpAVTransport::pauseCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("Pause failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::pause(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;

    const auto &attrs = value->get< RCSResourceAttributes >();

    for (const auto &kvPair : attrs)
    {
        DEBUG_PRINT("pause kvPair key=" << kvPair.key());
        DEBUG_PRINT("pause kvPair value=" << kvPair.value().toString());

        if (kvPair.key() == "instanceId")
        {
            instanceId = (kvPair.value()).get< int >();
            instanceId = std::max(0, instanceId);
            DEBUG_PRINT("pause instanceId=" << instanceId);
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "Pause",
                                          pauseCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["pause"].first;

    return true;
}

void UpnpAVTransport::previousCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("Previous failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::previous(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;

    const auto &attrs = value->get< RCSResourceAttributes >();

    for (const auto &kvPair : attrs)
    {
        DEBUG_PRINT("previous kvPair key=" << kvPair.key());
        DEBUG_PRINT("previous kvPair value=" << kvPair.value().toString());

        if (kvPair.key() == "instanceId")
        {
            instanceId = (kvPair.value()).get< int >();
            instanceId = std::max(0, instanceId);
            DEBUG_PRINT("previous instanceId=" << instanceId);
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "Previous",
                                          previousCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["previous"].first;

    return true;
}

void UpnpAVTransport::seekCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("Seek failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::seek(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;
    string unit = "TRACK_NR";
    string target = "0";

    const auto &attrs = value->get< RCSResourceAttributes >();

    for (const auto &kvPair : attrs)
    {
        DEBUG_PRINT("seek kvPair key=" << kvPair.key());
        DEBUG_PRINT("seek kvPair value=" << kvPair.value().toString());

        if (kvPair.key() == "instanceId")
        {
            instanceId = (kvPair.value()).get< int >();
            instanceId = std::max(0, instanceId);
            DEBUG_PRINT("seek instanceId=" << instanceId);
        }

        if (kvPair.key() == "unit")
        {
            unit = (kvPair.value()).get< string >();
            DEBUG_PRINT("seek unit=" << unit);
        }

        if (kvPair.key() == "target")
        {
            target = (kvPair.value()).get< string >();
            DEBUG_PRINT("seek target=" << target);
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "Seek",
                                          seekCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          "Unit",
                                          G_TYPE_STRING,
                                          unit.c_str(),
                                          "Target",
                                          G_TYPE_STRING,
                                          target.c_str(),
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["seek"].first;

    return true;
}

void UpnpAVTransport::stopCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("Stop failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAVTransport::stop(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int instanceId = 0;

    const auto &attrs = value->get< RCSResourceAttributes >();

    for (const auto &kvPair : attrs)
    {
        DEBUG_PRINT("stop kvPair key=" << kvPair.key());
        DEBUG_PRINT("stop kvPair value=" << kvPair.value().toString());

        if (kvPair.key() == "instanceId")
        {
            instanceId = (kvPair.value()).get< int >();
            instanceId = std::max(0, instanceId);
            DEBUG_PRINT("stop instanceId=" << instanceId);
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "Stop",
                                          stopCb,
                                          (gpointer *) request,
                                          "InstanceID",
                                          G_TYPE_UINT,
                                          instanceId,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["stop"].first;

    return true;
}

bool UpnpAVTransport::getAttributesRequest(UpnpRequest *request, const map< string, string > &queryParams)
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
        bool result = false;

        // Check if custom GET needs to be called
        auto attr = this->GetAttributeActionMap.find(it->first);
        if (attr != this->GetAttributeActionMap.end())
        {
            GetAttributeHandler fp = attr->second;
            result = (this->*fp)(request, queryParams);
        }
        else if (string(attrInfo->actions[0].name) != "")
        {
            result = UpnpAttribute::get(m_proxy, request, attrInfo);
        }

        status |= result;
        if (!result)
        {
            request->done++;
        }
    }

    return status;

}

bool UpnpAVTransport::setAttributesRequest(const RCSResourceAttributes &value,
                                           UpnpRequest *request,
                                           const map< string, string > &queryParams)
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
        bool result = false;

        // Check if custom SET needs to be called
        auto attr = this->SetAttributeActionMap.find(attrName);
        if (attr != this->SetAttributeActionMap.end())
        {
            SetAttributeHandler fp = attr->second;
            result = (this->*fp)(request, &attrValue, queryParams);
        }
        else if (string(attrInfo->actions[0].name) != "")
        {
            result = UpnpAttribute::set(m_proxy, request, attrInfo, &attrValue);
        }

        status |= result;
        if (!result)
        {
            request->done++;
        }
    }

    return status;
}
