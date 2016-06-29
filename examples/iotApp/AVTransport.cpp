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

using namespace std;
using namespace OC;
AVTransport::AVTransport() :
    m_resource(nullptr)
{
}

AVTransport::AVTransport(OCResource::Ptr resource) :
    m_resource(resource)
{
}


AVTransport::~AVTransport()
{
}

void AVTransport::setAVTransportURI(int instanceId, string currentURI, string currentUriMetadata)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("instanceId", instanceId);
    attributes.setValue("currentUri", currentURI);
    attributes.setValue("currentUriMetadata", currentUriMetadata);
    rep.setValue("avTransportUri", attributes);
    m_setAVTransportURICb = bind(&AVTransport::onSetAVTransportURI, this, placeholders::_1,
                                 placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_setAVTransportURICb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void AVTransport::onSetAVTransportURI(const HeaderOptions &headerOptions,
                                      const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

void AVTransport::setNextAVTransportURI(int instanceId, string nextUri, string nextUriMetadata)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("instanceId", instanceId);
    attributes.setValue("nextUri", nextUri);
    attributes.setValue("nextUriMetadata", nextUriMetadata);
    rep.setValue("avTransportUri", attributes);
    m_setNextAVTransportURICb = bind(&AVTransport::onSetNextAVTranportURI, this, placeholders::_1,
                                     placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_setNextAVTransportURICb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void AVTransport::onSetNextAVTranportURI(const HeaderOptions &headerOptions,
        const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

AVTransport::MediaInfo AVTransport::getMediaInfo(int instanceId)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    m_mediaInfo = {"", "", "", "", "", "", "", "", 0};
    m_getMediaInfoCb = bind(&AVTransport::onGetMediaInfo, this, placeholders::_1, placeholders::_2,
                            placeholders::_3);
    QueryParamsMap param =
    {
        {"instanceId", to_string(instanceId)},
    };
    m_resource->get(param, m_getMediaInfoCb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_mediaInfo;
}

void AVTransport::onGetMediaInfo(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                                 const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation mediaInfoRep;
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

AVTransport::TransportInfo AVTransport::getTransportInfo(int instanceId)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    m_transportInfo = {"", "", ""};
    m_getTransportInfoCb = bind(&AVTransport::onGetTransportInfo, this, placeholders::_1,
                                placeholders::_2, placeholders::_3);
    QueryParamsMap param =
    {
        {"instanceId", to_string(instanceId)},
    };
    m_resource->get(param, m_getTransportInfoCb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_transportInfo;
}

void AVTransport::onGetTransportInfo(const HeaderOptions &headerOptions,
                                     const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation transportInfoRep;
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

AVTransport::PositionInfo AVTransport::getPositionInfo(int instanceId)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    m_positionInfo = {0, "", 0, "", 0, "", "", ""};
    m_getPositionInfoCb = bind(&AVTransport::onGetPositionInfo, this, placeholders::_1,
                               placeholders::_2, placeholders::_3);
    QueryParamsMap param =
    {
        {"instanceId", to_string(instanceId)},
    };
    m_resource->get(param, m_getPositionInfoCb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_positionInfo;
}

void AVTransport::onGetPositionInfo(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                                    const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation positionInfoRep;
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

AVTransport::DeviceCapabilities AVTransport::getDeviceCapabilities(int instanceId)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    m_deviceCapabilities = {"", "", ""};
    m_getDeviceCapabilitiesCb = bind(&AVTransport::onGetDeviceCapabilities, this, placeholders::_1,
                                     placeholders::_2, placeholders::_3);
    QueryParamsMap param =
    {
        {"instanceId", to_string(instanceId)},
    };
    m_resource->get(param, m_getDeviceCapabilitiesCb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_deviceCapabilities;
}

void AVTransport::onGetDeviceCapabilities(const HeaderOptions &headerOptions,
        const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation deviceCapabilitiesRep;
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

AVTransport::TransportSettings AVTransport::getTransportSettings(int instanceId)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    m_transportSettings = {"", ""};
    m_getTransportSettingsCb = bind(&AVTransport::onGetTransportSettings, this, placeholders::_1,
                                    placeholders::_2, placeholders::_3);
    QueryParamsMap param =
    {
        {"instanceId", to_string(instanceId)},
    };
    m_resource->get(param, m_getTransportSettingsCb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_transportSettings;
}

void AVTransport::onGetTransportSettings(const HeaderOptions &headerOptions,
        const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation transportSettingsRep;
            if (rep.getValue("transportSettings", transportSettingsRep))
            {
                transportSettingsRep.getValue("playMode", m_transportSettings.playMode);
                transportSettingsRep.getValue("recQualityMode", m_transportSettings.recQualityMode);
            }
        }
    }
    m_cv.notify_one();
}

void AVTransport::stop(int instanceId)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("instanceId", instanceId);
    rep.setValue("stop", attributes);
    m_stopCb = bind(&AVTransport::onStop, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_stopCb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void AVTransport::onStop(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                         const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

void AVTransport::play(int instanceId, string speed)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("instanceId", instanceId);
    attributes.setValue("speed", speed);
    rep.setValue("play", attributes);
    m_playCb = bind(&AVTransport::onPlay, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_playCb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void AVTransport::onPlay(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                         const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

void AVTransport::pause(int instanceId)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("instanceId", instanceId);
    rep.setValue("pause", attributes);
    m_pauseCb = bind(&AVTransport::onPause, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_pauseCb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void AVTransport::onPause(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                          const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

void AVTransport::seek(int instanceId, string unit, string target)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("instanceId", instanceId);
    attributes.setValue("unit", unit);
    attributes.setValue("target", target);
    rep.setValue("seek", attributes);
    m_seekCb = bind(&AVTransport::onSeek, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_seekCb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void AVTransport::onSeek(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                         const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

void AVTransport::next(int instanceId)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("instanceId", instanceId);
    rep.setValue("next", attributes);
    m_nextCb = bind(&AVTransport::onNext, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_nextCb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void AVTransport::onNext(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                         const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

void AVTransport::previous(int instanceId)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("instanceId", instanceId);
    rep.setValue("previous", attributes);
    m_previouseCb = bind(&AVTransport::onPrevious, this, placeholders::_1, placeholders::_2,
                         placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_previouseCb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void AVTransport::onPrevious(const HeaderOptions &headerOptions,
                             const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

void AVTransport::setPlayMode(int instanceId, string newPlayMode)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("instanceId", instanceId);
    attributes.setValue("newPlayMode", newPlayMode);
    rep.setValue("playMode", attributes);
    m_setPlayModeCb = bind(&AVTransport::onSetPlayMode, this, placeholders::_1, placeholders::_2,
                           placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_setPlayModeCb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void AVTransport::onSetPlayMode(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                                const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

string AVTransport::getCurrentTransportActions(int instanceId)
{
    unique_lock<mutex> avTranportLock(m_mutex);
    m_getCurrentTransportActionsCb = bind(&AVTransport::onGetCurrentTransportActions, this,
                                          placeholders::_1, placeholders::_2, placeholders::_3);
    QueryParamsMap param =
    {
        {"instanceId", to_string(instanceId)}
    };
    m_resource->get(param, m_getCurrentTransportActionsCb);
    if (m_cv.wait_for(avTranportLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_currentTransportActions;
}

void AVTransport::onGetCurrentTransportActions(const HeaderOptions &headerOptions,
        const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
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
