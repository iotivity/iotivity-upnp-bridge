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

RenderingControl::RenderingControl() :
    m_resource(nullptr)
{
}

RenderingControl::RenderingControl(OC::OCResource::Ptr resource) :
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

std::string RenderingControl::listPresets(int instanceId)
{
    std::unique_lock<std::mutex> renderingControlLock(m_mutex);
    m_presetNameList = "";
    m_listPresetsCb = bind(&RenderingControl::onListPresets, this,
                           std::placeholders::_1,
                           std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)}
    };
    m_resource->get(param, m_listPresetsCb);
    m_cv.wait(renderingControlLock);
    return m_presetNameList;
}

void RenderingControl::onListPresets(const OC::HeaderOptions &headerOptions,
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
            rep.getValue("presetNameList", m_presetNameList);
        }
    }
    m_cv.notify_one();
}

void RenderingControl::selectPreset(int instanceId, std::string presetName)
{
    std::unique_lock<std::mutex> renderingControlLock(m_mutex);
    OC::OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("presetName", presetName);

    m_selectPresetsCb = bind(&RenderingControl::onSelectPresets, this,
                             std::placeholders::_1,
                             std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)},
    };
    m_resource->post(rep, param, m_selectPresetsCb);
    m_cv.wait(renderingControlLock);
}

void RenderingControl::onSelectPresets(const OC::HeaderOptions &headerOptions,
                                       const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

bool RenderingControl::getMute(int instanceId, std::string channel)
{
    std::unique_lock<std::mutex> renderingControlLock(m_mutex);
    m_currentMute = false;
    m_getMuteCb = bind(&RenderingControl::onGetMute, this,
                       std::placeholders::_1,
                       std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap params =
    {
        {"instanceId", std::to_string(instanceId)},
        {"channel", channel}
    };
    m_resource->get(params, m_getMuteCb);
    m_cv.wait(renderingControlLock);
    return m_currentMute;
}

void RenderingControl::onGetMute(const OC::HeaderOptions &headerOptions,
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
            rep.getValue("mute", m_currentMute);
        }
    }
    m_cv.notify_one();
}

void RenderingControl::setMute(int instanceId, std::string channel, bool desiredMute)
{
    std::unique_lock<std::mutex> renderingControlLock(m_mutex);
    OC::OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("mute", desiredMute);
    m_setMuteCb = bind(&RenderingControl::onSetMute, this,
                       std::placeholders::_1,
                       std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap params =
    {
        {"instanceId", std::to_string(instanceId)},
        {"channel", channel}
    };
    m_resource->post(rep, params, m_setMuteCb);
    m_cv.wait(renderingControlLock);
}

void RenderingControl::onSetMute(const OC::HeaderOptions &headerOptions,
                                 const OC::OCRepresentation &rep, const int eCode)
{
    onGetMute(headerOptions, rep, eCode);
}

int RenderingControl::getVolume(int instanceId, std::string channel)
{
    std::unique_lock<std::mutex> renderingControlLock(m_mutex);
    m_currentVolume = 0;
    m_getVolumeCb = bind(&RenderingControl::onGetVolume, this,
                         std::placeholders::_1,
                         std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap param =
    {
        {"instanceId", std::to_string(instanceId)},
        {"channel", channel}
    };
    m_resource->get(param, m_getVolumeCb);
    m_cv.wait(renderingControlLock);
    return m_currentVolume;
}

void RenderingControl::onGetVolume(const OC::HeaderOptions &headerOptions,
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
            rep.getValue("volume", m_currentVolume);
        }
    }
    m_cv.notify_one();
}

void RenderingControl::setVolume(int instanceId, std::string channel,
                                 int desiredVolume)
{
    std::unique_lock<std::mutex> renderingControlLock(m_mutex);
    OC::OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("instanceId", instanceId);
    rep.setValue("channel", channel);
    rep.setValue("volume", desiredVolume);
    m_setVolumeCb = bind(&RenderingControl::onSetVolume, this,
                         std::placeholders::_1,
                         std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap params =
    {
        {"instanceId", std::to_string(instanceId)},
        {"channel", channel}
    };
    m_resource->post(rep, params, m_setVolumeCb);
    m_cv.wait(renderingControlLock);
}

void RenderingControl::onSetVolume(const OC::HeaderOptions &headerOptions,
                                   const OC::OCRepresentation &rep, const int eCode)
{
    onGetVolume(headerOptions, rep, eCode);
}


//Overloaded operator used for putting into a 'set'
bool RenderingControl::operator<(const RenderingControl &other) const
{
    return ((*m_resource) < (*(other.m_resource)));
}
