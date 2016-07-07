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

#include "RenderingControl.h"

#include <chrono>
#include "IotivityUtility.h"

using namespace std;
using namespace OC;

RenderingControl::RenderingControl() :
    m_presetNameList(""),
    m_currentMute(false),
    m_currentVolume(50),
    m_resource(nullptr)
{
}

RenderingControl::RenderingControl(OCResource::Ptr resource) :
    m_presetNameList(""),
    m_currentMute(false),
    m_currentVolume(50),
    m_resource(resource)
{
}

RenderingControl::~RenderingControl()
{
}

RenderingControl::RenderingControl(const RenderingControl &other)
{
    *this = other;
}

RenderingControl &RenderingControl::operator=(const RenderingControl &other)
{
    // check for self-assignment
    if (&other == this)
    {
        return *this;
    }
    m_presetNameList = other.m_presetNameList;
    m_currentMute = other.m_currentMute;
    m_currentVolume = other.m_currentVolume;
    m_resource = other.m_resource;
    // do not copy the mutex or condition_variable
    return *this;
}

string RenderingControl::listPresets(int instanceId)
{
    unique_lock<mutex> renderingControlLock(m_mutex);
    m_presetNameList = "";
    m_listPresetsCb = bind(&RenderingControl::onListPresets, this, placeholders::_1, placeholders::_2,
                           placeholders::_3);
    QueryParamsMap param =
    {
        {"instanceId", to_string(instanceId)}
    };
    m_resource->get(param, m_listPresetsCb);
    if (m_cv.wait_for(renderingControlLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_presetNameList;
}

void RenderingControl::onListPresets(const HeaderOptions &headerOptions,
                                     const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation positionInfoRep;
            rep.getValue("presetNameList", m_presetNameList);
        }
    }
    m_cv.notify_one();
}

void RenderingControl::selectPreset(int instanceId, string presetName)
{
    unique_lock<mutex> renderingControlLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("instanceId", instanceId);
    attributes.setValue("selectedPresetName", presetName);
    rep.setValue("presetName", attributes);

    m_selectPresetsCb = bind(&RenderingControl::onSelectPresets, this,
                             placeholders::_1,
                             placeholders::_2, placeholders::_3);

    m_resource->post(rep, QueryParamsMap(), m_selectPresetsCb);
    if (m_cv.wait_for(renderingControlLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void RenderingControl::onSelectPresets(const HeaderOptions &headerOptions,
                                       const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

bool RenderingControl::getMute(int instanceId, string channel)
{
    unique_lock<mutex> renderingControlLock(m_mutex);
    m_currentMute = false;
    m_getMuteCb = bind(&RenderingControl::onGetMute, this, placeholders::_1, placeholders::_2,
                       placeholders::_3);
    QueryParamsMap params =
    {
        {"instanceId", to_string(instanceId)},
        {"channel", channel}
    };
    m_resource->get(params, m_getMuteCb);
    if (m_cv.wait_for(renderingControlLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_currentMute;
}

void RenderingControl::onGetMute(const HeaderOptions &headerOptions, const OCRepresentation &rep,
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
            rep.getValue("mute", m_currentMute);
        }
    }
    m_cv.notify_one();
}

void RenderingControl::setMute(int instanceId, string channel, bool desiredMute)
{
    unique_lock<mutex> renderingControlLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("instanceId", instanceId);
    attributes.setValue("channel", channel);
    attributes.setValue("desiredMute", desiredMute);
    rep.setValue("mute", attributes);

    m_setMuteCb = bind(&RenderingControl::onSetMute, this, placeholders::_1, placeholders::_2,
                       placeholders::_3);

    m_resource->post(rep, QueryParamsMap(), m_setMuteCb);
    if (m_cv.wait_for(renderingControlLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void RenderingControl::onSetMute(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                                 const int eCode)
{
    onGetMute(headerOptions, rep, eCode);
}

int RenderingControl::getVolume(int instanceId, string channel)
{
    unique_lock<mutex> renderingControlLock(m_mutex);
    m_currentVolume = 0;
    m_getVolumeCb = bind(&RenderingControl::onGetVolume, this, placeholders::_1, placeholders::_2,
                         placeholders::_3);
    QueryParamsMap param =
    {
        {"instanceId", to_string(instanceId)},
        {"channel", channel}
    };
    m_resource->get(param, m_getVolumeCb);
    if (m_cv.wait_for(renderingControlLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_currentVolume;
}

void RenderingControl::onGetVolume(const HeaderOptions &headerOptions, const OCRepresentation &rep,
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
            rep.getValue("volume", m_currentVolume);
        }
    }
    m_cv.notify_one();
}

void RenderingControl::setVolume(int instanceId, string channel, int desiredVolume)
{
    unique_lock<mutex> renderingControlLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("instanceId", instanceId);
    attributes.setValue("channel", channel);
    attributes.setValue("desiredVolume", desiredVolume);
    rep.setValue("volume", attributes);

    m_setVolumeCb = bind(&RenderingControl::onSetVolume, this, placeholders::_1, placeholders::_2,
                         placeholders::_3);

    m_resource->post(rep, QueryParamsMap(), m_setVolumeCb);
    if (m_cv.wait_for(renderingControlLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void RenderingControl::onSetVolume(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                                   const int eCode)
{
    onGetVolume(headerOptions, rep, eCode);
}


//Overloaded operator used for putting into a 'set'
bool RenderingControl::operator<(const RenderingControl &other) const
{
    return ((*m_resource) < (*(other.m_resource)));
}
