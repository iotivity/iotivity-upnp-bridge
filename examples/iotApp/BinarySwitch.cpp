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

#include "BinarySwitch.h"

using namespace std;
using namespace OC;

BinarySwitch::BinarySwitch() {}
BinarySwitch::BinarySwitch(std::shared_ptr<OC::OCResource> resource) :
    m_resource(resource), m_powerState(false)
{
}
BinarySwitch::~BinarySwitch() {}
BinarySwitch::BinarySwitch( const BinarySwitch &other )
{
    *this = other;
}
BinarySwitch &BinarySwitch::operator=(const BinarySwitch &other)
{
    // check for self-assignment
    if (&other == this)
        return *this;
    m_resource = other.m_resource;
    m_powerState = other.m_powerState;
    // do not copy the mutex or condition_variable
    return *this;
}

void BinarySwitch::toggle()
{
    if (isOn())
    {
        turnOn(false);
    }
    else
    {
        turnOn(true);
    }
}
bool BinarySwitch::turnOn(bool isOn)
{
    std::unique_lock<std::mutex> powerChangeLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("value", isOn);
    m_postCB = bind(&BinarySwitch::onPostPowerSwitch, this, placeholders::_1, placeholders::_2,
                    placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_postCB);
    m_cv.wait(powerChangeLock);
    return (m_eCode == OC_STACK_OK);
}

bool BinarySwitch::isOn()
{
    std::unique_lock<std::mutex> powerChangeLock(m_mutex);
    m_getCB = bind(&BinarySwitch::onGetPowerSwitch, this, placeholders::_1, placeholders::_2,
                   placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getCB);
    m_cv.wait(powerChangeLock);
    return m_powerState;
}

void BinarySwitch::isOnAsync(OC::GetCallback isOnCB)
{
    m_resource->get(QueryParamsMap(), isOnCB);
}
void BinarySwitch::turnOnAsync(bool isOn, OC::PostCallback turnOnCB) const
{
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("value", isOn);
    //currently no callback is used in this code
    m_resource->post(rep, QueryParamsMap(), turnOnCB);
}

//Overloaded operator used for putting into a 'set'
bool BinarySwitch::operator<(const BinarySwitch &other) const
{
    return ((*m_resource) < (*(other.m_resource)));
}

void BinarySwitch::onGetPowerSwitch(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                                    const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("value", m_powerState);
        }
    }
    m_cv.notify_one();
}

void BinarySwitch::onPostPowerSwitch(const HeaderOptions &headerOptions,
                                     const OCRepresentation &rep, const int eCode)
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
