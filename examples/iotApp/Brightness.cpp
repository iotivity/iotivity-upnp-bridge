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

#include "Brightness.h"

using namespace std;
using namespace OC;

Brightness::Brightness() : m_resource(nullptr), m_brightness(100)
{
    m_getCB = bind(&Brightness::onGetBrightness, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_postCB = bind(&Brightness::onPostBrightness, this, placeholders::_1, placeholders::_2, placeholders::_3);
}

Brightness::Brightness(std::shared_ptr<OC::OCResource> resource) :
    m_resource(resource), m_brightness(100)
{
    m_getCB = bind(&Brightness::onGetBrightness, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_postCB = bind(&Brightness::onPostBrightness, this, placeholders::_1, placeholders::_2, placeholders::_3);
}
Brightness::~Brightness() {}
Brightness::Brightness( const Brightness& other ){
    *this = other;
}
Brightness& Brightness::operator=(const Brightness& other)
{
    // check for self-assignment
    if(&other == this)
        return *this;
    m_getCB = bind(&Brightness::onGetBrightness, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_postCB = bind(&Brightness::onPostBrightness, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource = other.m_resource;
    m_brightness = other.m_brightness;
    // do not copy the mutex or condition_variable
    return *this;
}

int Brightness::getBrightness()
{
    std::unique_lock<std::mutex> brightnessChangeLock(m_mutex);
    m_resource->get(QueryParamsMap(), m_getCB);
    m_cv.wait(brightnessChangeLock);
    return m_brightness;
}

bool Brightness::setBrightness(int brightness)
{
    std::unique_lock<std::mutex> brightnessChangeLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("brightness", brightness);
    //currently no callback is used in this code
    m_resource->post(rep, QueryParamsMap(), m_postCB);
    m_cv.wait(brightnessChangeLock);
    return (m_eCode == OC_STACK_OK);
}


void Brightness::getBrightnessAsync(OC::GetCallback getBrightnessCB)
{
        m_resource->get(QueryParamsMap(), getBrightnessCB);
}
void Brightness::setBrightnessAsync(int brightness, OC::PostCallback setBrightnessCB) const
{
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("brightness", brightness);
    //currently no callback is used in this code
    m_resource->post(rep, QueryParamsMap(), setBrightnessCB);
}

//Overloaded operator used for putting into a 'set'
bool Brightness::operator<(const Brightness &other) const
{
    return ((*m_resource) < (*(other.m_resource)));
}

void Brightness::onGetBrightness(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if(uri == m_resource->uri()) {
            rep.getValue("brightness", m_brightness);
        }
    }
    m_cv.notify_one();
}

void Brightness::onPostBrightness(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    m_eCode = eCode;
    if (eCode == OC_STACK_OK)
    {
        //std::cout << "POST request successful" << std::endl;
    }
    m_cv.notify_one();
}
