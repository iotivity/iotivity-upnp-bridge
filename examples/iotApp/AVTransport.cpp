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

#include "AVTransport.h"

#include "IotivityUtility.h"
#include <UpnpBridgeAttributes.h>
AVTransport::AVTransport() :
    m_resource(nullptr)
{
}

AVTransport::AVTransport(OC::OCResource::Ptr resource) :
    m_resource(resource)
{
}


AVTransport::~AVTransport()
{
}

void AVTransport::setAVTransportURI(unsigned int instanceId, std::string currentURI,
                                    std::string currentUriMetadata)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_setAVTransportURICb = bind(&AVTransport::onSetAVTransportURI, this, std::placeholders::_1,
                                 std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)},
        {"currentUri", currentURI},
        {"currentUriMetadata", currentUriMetadata}
    };
    m_resource->get(param, m_setAVTransportURICb);
    m_cv.wait(avTranportLock);
}

void AVTransport::onSetAVTransportURI(const OC::HeaderOptions &headerOptions,
                                      const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

void AVTransport::setNextAVTransportURI(unsigned int instanceId, std::string nextUri,
                                        std::string nextUriMetadata)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_setNextAVTransportURICb = bind(&AVTransport::onSetNextAVTranportURI, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)},
        {"nextUri", nextUri},
        {"nextUriMetadata", nextUriMetadata}
    };
    m_resource->get(param, m_setNextAVTransportURICb);
    m_cv.wait(avTranportLock);
}

void AVTransport::onSetNextAVTranportURI(const OC::HeaderOptions &headerOptions,
        const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

AVTransport::MediaInfo AVTransport::getMediaInfo(unsigned int instanceId)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_mediaInfo = {"", "", "", "", "", "", "", "", 0};
    m_getMediaInfoCb = bind(&AVTransport::onGetMediaInfo, this, std::placeholders::_1,
                            std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)},
    };
    m_resource->get(param, m_getMediaInfoCb);
    m_cv.wait(avTranportLock);
    return m_mediaInfo;
}

void AVTransport::onGetMediaInfo(const OC::HeaderOptions &headerOptions,
                                 const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OC::OCRepresentation mediaInfoRep;
            if (rep.getValue("mediaInfo", mediaInfoRep))
            {
                mediaInfoRep.getValue("nrTracks", m_mediaInfo.nrTracks);
                mediaInfoRep.getValue("mediaDuration", m_mediaInfo.mediaDuration);
                mediaInfoRep.getValue("currentUri", m_mediaInfo.currentUri);
                mediaInfoRep.getValue("currentUriMetadata", m_mediaInfo.currentUriMetadata);
                mediaInfoRep.getValue("nextUri", m_mediaInfo.nextUri);
                mediaInfoRep.getValue("nextUriMetadata", m_mediaInfo.currentUriMetadata);
                mediaInfoRep.getValue("playMedium", m_mediaInfo.playMedium);
                mediaInfoRep.getValue("recordMedium", m_mediaInfo.recordMedium);
                mediaInfoRep.getValue("writeStatus", m_mediaInfo.writeStatus);
            }
        }
    }
    m_cv.notify_one();
}

AVTransport::TransportInfo AVTransport::getTransportInfo(unsigned int instanceId)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_transportInfo = {"", "", ""};
    m_getTransportInfoCb = bind(&AVTransport::onGetTransportInfo, this, std::placeholders::_1,
                                std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)},
    };
    m_resource->get(param, m_getTransportInfoCb);
    m_cv.wait(avTranportLock);
    return m_transportInfo;
}

void AVTransport::onGetTransportInfo(const OC::HeaderOptions &headerOptions,
                                     const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OC::OCRepresentation transportInfoRep;
            if (rep.getValue("transportInfo", transportInfoRep))
            {
                transportInfoRep.getValue("transportState", m_transportInfo.transportState);
                transportInfoRep.getValue("transportStatus", m_transportInfo.transportStatus);
                transportInfoRep.getValue("speed", m_transportInfo.speed);
            }
        }
    }
    m_cv.notify_one();
}

AVTransport::PositionInfo AVTransport::getPositionInfo(unsigned int instanceId)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_positionInfo = {0, "", 0, "", 0, "", "", ""};
    m_getPositionInfoCb = bind(&AVTransport::onGetPositionInfo, this, std::placeholders::_1,
                               std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)},
    };
    m_resource->get(param, m_getPositionInfoCb);
    m_cv.wait(avTranportLock);
    return m_positionInfo;
}

void AVTransport::onGetPositionInfo(const OC::HeaderOptions &headerOptions,
                                    const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OC::OCRepresentation positionInfoRep;
            if (rep.getValue("positionInfo", positionInfoRep))
            {
                positionInfoRep.getValue("track", m_positionInfo.track);
                positionInfoRep.getValue("trackDuration", m_positionInfo.trackDuration);
                positionInfoRep.getValue("trackMetadata", m_positionInfo.trackMetadata);
                positionInfoRep.getValue("trackUri", m_positionInfo.trackUri);
                positionInfoRep.getValue("relTime", m_positionInfo.relTime);
                positionInfoRep.getValue("absTime", m_positionInfo.absTime);
                positionInfoRep.getValue("relCount", m_positionInfo.relCount);
                positionInfoRep.getValue("absCount", m_positionInfo.absCount);
            }
        }
    }
    m_cv.notify_one();
}

AVTransport::DeviceCapabilities AVTransport::getDeviceCapabilities(unsigned int instanceId)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_deviceCapabilities = {"", "", ""};
    m_getDeviceCapabilitiesCb = bind(&AVTransport::onGetDeviceCapabilities, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)},
    };
    m_resource->get(param, m_getDeviceCapabilitiesCb);
    m_cv.wait(avTranportLock);
    return m_deviceCapabilities;
}

void AVTransport::onGetDeviceCapabilities(const OC::HeaderOptions &headerOptions,
        const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OC::OCRepresentation deviceCapabilitiesRep;
            if (rep.getValue("deviceCapabilities", deviceCapabilitiesRep))
            {
                deviceCapabilitiesRep.getValue("playMedia", m_deviceCapabilities.playMedia);
                deviceCapabilitiesRep.getValue("recMedia", m_deviceCapabilities.recMedia);
                deviceCapabilitiesRep.getValue("recQualityModes", m_deviceCapabilities.recQualityModes);
            }
        }
    }
    m_cv.notify_one();
}

AVTransport::TransportSettings AVTransport::getTransportSettings(unsigned int instanceId)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_transportSettings = {"", ""};
    m_getTransportSettingsCb = bind(&AVTransport::onGetTransportSettings, this, std::placeholders::_1,
                                    std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)},
    };
    m_resource->get(param, m_getTransportSettingsCb);
    m_cv.wait(avTranportLock);
    return m_transportSettings;
}

void AVTransport::onGetTransportSettings(const OC::HeaderOptions &headerOptions,
        const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OC::OCRepresentation transportSettingsRep;
            if (rep.getValue("transportSettings", transportSettingsRep))
            {
                transportSettingsRep.getValue("playMode", m_transportSettings.playMode);
                transportSettingsRep.getValue("recQualityMode", m_transportSettings.recQualityMode);
            }
        }
    }
    m_cv.notify_one();
}

void AVTransport::stop(unsigned int instanceId)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_stopCb = bind(&AVTransport::onStop, this, std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)}
    };
    m_resource->get(param, m_stopCb);
    m_cv.wait(avTranportLock);
}

void AVTransport::onStop(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                         const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

void AVTransport::play(unsigned int instanceId, std::string speed)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_playCb = bind(&AVTransport::onPlay, this, std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)},
        {"speed", speed}
    };
    m_resource->get(param, m_playCb);
    m_cv.wait(avTranportLock);
}

void AVTransport::onPlay(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                         const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

void AVTransport::pause(unsigned int instanceId)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_pauseCb = bind(&AVTransport::onPause, this, std::placeholders::_1, std::placeholders::_2,
                     std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)}
    };
    m_resource->get(param, m_pauseCb);
    m_cv.wait(avTranportLock);
}

void AVTransport::onPause(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                          const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

void AVTransport::seek(unsigned int instanceId, std::string unit, std::string target)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_seekCb = bind(&AVTransport::onSeek, this, std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)},
        {"unit", unit},
        {"target", target}
    };
    m_resource->get(param, m_seekCb);
    m_cv.wait(avTranportLock);
}

void AVTransport::onSeek(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                         const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

void AVTransport::next(unsigned int instanceId)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_nextCb = bind(&AVTransport::onNext, this, std::placeholders::_1, std::placeholders::_2,
                    std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)}
    };
    m_resource->get(param, m_nextCb);
    m_cv.wait(avTranportLock);
}

void AVTransport::onNext(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                         const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

void AVTransport::previous(unsigned int instanceId)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_previouseCb = bind(&AVTransport::onPrevious, this, std::placeholders::_1, std::placeholders::_2,
                         std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)}
    };
    m_resource->get(param, m_previouseCb);
    m_cv.wait(avTranportLock);
}

void AVTransport::onPrevious(const OC::HeaderOptions &headerOptions,
                             const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

void AVTransport::setPlayMode(unsigned int instanceId, std::string newPlayMode)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_setPlayModeCb = bind(&AVTransport::onSetPlayMode, this, std::placeholders::_1,
                           std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)},
        {"newPlayMode", newPlayMode}
    };
    m_resource->get(param, m_setPlayModeCb);
    m_cv.wait(avTranportLock);
}

void AVTransport::onSetPlayMode(const OC::HeaderOptions &headerOptions,
                                const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

std::string AVTransport::getCurrentTransportActions(unsigned int instanceId)
{
    std::unique_lock<std::mutex> avTranportLock(m_mutex);
    m_getCurrentTransportActionsCb = bind(&AVTransport::onGetCurrentTransportActions, this,
                                          std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)}
    };
    m_resource->get(param, m_getCurrentTransportActionsCb);
    m_cv.wait(avTranportLock);
    return m_currentTransportActions;
}

void AVTransport::onGetCurrentTransportActions(const OC::HeaderOptions &headerOptions,
        const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("currentTransportActions", m_currentTransportActions);
        }
    }
    m_cv.notify_one();
}

bool AVTransport::operator<(const AVTransport &other) const
{
    return ((*m_resource) < (*(other.m_resource)));
}
