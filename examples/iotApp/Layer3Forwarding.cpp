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

#include "Layer3Forwarding.h"
#include "IotivityUtility.h"
#include <chrono>

using namespace std;
using namespace OC;

Layer3Forwarding::Layer3Forwarding() :
    m_resource(nullptr),
    m_defaultConnectionService(""),
    m_eCode(0)
{
}

Layer3Forwarding::Layer3Forwarding(std::shared_ptr<OC::OCResource> resource) :
    m_resource(resource),
    m_defaultConnectionService(""),
    m_eCode(0)
{
}

Layer3Forwarding::~Layer3Forwarding()
{
}

Layer3Forwarding::Layer3Forwarding( const Layer3Forwarding &other )
{
    *this = other;
}

Layer3Forwarding &Layer3Forwarding::operator=(const Layer3Forwarding &other)
{
    // check for self-assignment
    if (&other == this)
        return *this;
    m_resource = other.m_resource;
    m_defaultConnectionService = other.m_defaultConnectionService;
    // do not copy the mutex or condition_variable
    return *this;
}

//Overloaded operator used for putting into a 'set'
bool Layer3Forwarding::operator<(const Layer3Forwarding &other) const
{
    return ((*m_resource) < (*(other.m_resource)));
}

string Layer3Forwarding::getDefaultConnectionService()
{
    std::unique_lock<std::mutex> l3fLock(m_mutex);
    m_defaultConnectionService = "";
    m_getDefaultConnectionServiceCB = bind(&Layer3Forwarding::onGetDefaultConnectionService, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getDefaultConnectionServiceCB);
    if (m_cv.wait_for(l3fLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the browse request." << endl;
    }
    return m_defaultConnectionService;
}

void Layer3Forwarding::onGetDefaultConnectionService(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("defaultConnectionService", m_defaultConnectionService);
        }
    }
    m_cv.notify_one();
}

void Layer3Forwarding::setDefaultConnectionService(string newDefaultConnectionService)
{
    std::unique_lock<std::mutex> l3fLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("defaultConnectionService", newDefaultConnectionService);
    m_setDefaultconnectionServiceCB = bind(&Layer3Forwarding::onSetDefaultConnectionService, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_setDefaultconnectionServiceCB);
    if (m_cv.wait_for(l3fLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the browse request." << endl;
    }
    return;
}

void Layer3Forwarding::onSetDefaultConnectionService(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    m_eCode = eCode;
    if (eCode != OC_STACK_OK)
    {
        std::cerr << "Erorr setting the default connection service." << std::endl;
    }
    m_cv.notify_one();
}
