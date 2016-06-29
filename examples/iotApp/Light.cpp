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

#include "Light.h"
#include <OCPlatform.h>
#include "IotivityUtility.h"

using namespace std;
using namespace OC;

Light::Light() :
    m_resource(),
    m_supported_services_known(false),
    m_lightSupportsBrightness(false),
    m_lightSupportsSwitch(false),
    m_powerState(false),
    m_eCode(0)
{
}

Light::Light(OCResource::Ptr resource) :
    m_resource(resource),
    m_supported_services_known(false),
    m_lightSupportsBrightness(false),
    m_lightSupportsSwitch(false),
    m_powerState(false),
    m_eCode(0)
{
    init();
}

Light::~Light()
{
    unique_lock<mutex> lock(m_mutex);
}

Light::Light( const Light &other )
{
    *this = other;
}

bool Light::init()
{
    unique_lock<mutex> lock(m_mutex);
    while (!m_supported_services_known)
    {
        onGetServicesCB = bind(&Light::onGetServices, this, placeholders::_1, placeholders::_2,
                               placeholders::_3);
        m_resource->get(QueryParamsMap(), onGetServicesCB);
        if (m_cv.wait_for(lock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
        {
            cerr << "Remote device failed to respond to the request." << endl;
        }
    }
    return true;
}
Light &Light::operator=(const Light &other)
{
    unique_lock<mutex> lock(m_mutex);
    // check for self-assignment
    if (this != &other)
    {
        onGetServicesCB = bind(&Light::onGetServices, this, placeholders::_1, placeholders::_2,
                               placeholders::_3);
        m_resource = other.m_resource;
        m_supported_services_known = other.m_supported_services_known;
        m_lightSupportsBrightness = other.m_lightSupportsBrightness;
        m_lightSupportsSwitch = other.m_lightSupportsSwitch;
        m_powerState = other.m_powerState;
        m_binarySwitch = other.m_binarySwitch;
        m_brightness = other.m_brightness;
        // do not copy the mutex or condition_variable
    }
    return *this;
}

void Light::toggle()
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

bool Light::turnOn(bool isOn)
{
    return m_binarySwitch.turnOn(isOn);
}

bool Light::isOn()
{
    return m_binarySwitch.isOn();
}

void Light::isOnAsync(GetCallback isOnCB)
{
    m_binarySwitch.isOnAsync(isOnCB);
}

void Light::turnOnAsync(bool isOn, PostCallback turnOnCB) const
{
    m_binarySwitch.turnOnAsync(isOn, turnOnCB);
}

int Light::getBrightness()
{
    return m_brightness.getBrightness();
}

bool Light::setBrightness(int brightness)
{
    return m_brightness.setBrightness(brightness);
}

void Light::getBrightnessAsync(GetCallback getBrightnessCB)
{
    m_brightness.getBrightnessAsync(getBrightnessCB);
}

void Light::setBrightnessAsync(int brightness, PostCallback setBrightnessCB)
{
    m_brightness.setBrightnessAsync(brightness, setBrightnessCB);
}

bool Light::operator<(const Light &other) const
{
    return ((*m_resource) < (*(other.m_resource)));
}

void Light::onGetServices(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                          const int eCode)
{
    AttributeValue links;
    if (rep.hasAttribute("links"))
    {
        if (rep.getAttributeValue("links", links))
        {
            try
            {
                auto linkRepresentation = boost::get<vector<OCRepresentation> >(links);
                for (auto l : linkRepresentation)
                {
                    if (l.hasAttribute("rel"))
                    {
                        string rel;
                        if (l.getValue("rel", rel))
                        {
                            cout << "rel: " << rel << endl;
                            if ("contains" == rel)
                            {
                                if (l.hasAttribute("rt"))
                                {
                                    string rt;
                                    if (l.getValue("rt", rt))
                                    {
                                        cout << "rt: " << rt << endl;
                                        if ("oic.r.switch.binary" == rt)
                                        {
                                            if (l.hasAttribute("href"))
                                            {
                                                string href;
                                                l.getValue("href", href);
                                                cout << "href: " << href << endl;
                                                m_lightSupportsSwitch = true;
                                                OCResource::Ptr resource = OCPlatform::constructResourceObject(m_resource->host(), href,
                                                                           m_resource->connectivityType(), true, {rt}, {"oic.if.baseline", "oic.if.a"});
                                                m_binarySwitch = BinarySwitch(resource);
                                            }
                                        }
                                        else if ("oic.r.light.brightness" == rt)
                                        {
                                            if (l.hasAttribute("href"))
                                            {
                                                string href;
                                                l.getValue("href", href);
                                                cout << "href: " << href << endl;
                                                m_lightSupportsBrightness = true;
                                                OCResource::Ptr resource = OCPlatform::constructResourceObject(m_resource->host(), href,
                                                                           m_resource->connectivityType(), true, {rt}, {"oic.if.baseline", "oic.if.a"});
                                                m_brightness = Brightness(resource);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            catch (boost::bad_get &e)
            {
                cout << e.what() << endl;
                cout << "falure to getValue for links" << endl;
                cout << rep.getValueToString("links");
            }
        }
        else
        {
            cout << "falure to getValue for links" << endl;
            cout << rep.getValueToString("links");
        }
    }
    m_supported_services_known = true;
    m_cv.notify_one();
}
