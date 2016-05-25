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

using namespace std;
using namespace OC;

Light::Light() {}
Light::Light(std::shared_ptr<OC::OCResource> resource) :
    m_resource(resource), m_supported_services_known(false), m_powerState(false)
{
    onGetServicesCB = std::bind(&Light::onGetServices, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    m_resource->get(OC::QueryParamsMap(), onGetServicesCB);
}
Light::~Light() {
    std::unique_lock<std::mutex> powerChangeLock(m_mutex);
}

Light::Light( const Light& other ){
    *this = other;
}

Light& Light::operator=(const Light& other)
{
    std::unique_lock<std::mutex> powerChangeLock(m_mutex);
    // check for self-assignment
    if(this != &other)
    {
        onGetServicesCB = std::bind(&Light::onGetServices, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        m_resource = other.m_resource;
        m_supported_services_known = other.m_supported_services_known;
        m_powerState = other.m_powerState;
        m_binarySwitchUri = other.m_binarySwitchUri;
        m_brightnessUri = other.m_brightnessUri;
        m_binarySwitch = other.m_binarySwitch;
        m_brightness = other.m_brightness;
        // do not copy the mutex or condition_variable
        if(!m_supported_services_known) {
            m_resource->get(OC::QueryParamsMap(), onGetServicesCB);
        }
    }
    return *this;
}

void Light::toggle()
{
    if(isOn()) {
        turnOn(false);
    } else {
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

void Light::isOnAsync(OC::GetCallback isOnCB)
{
    m_binarySwitch.isOnAsync(isOnCB);
}
void Light::turnOnAsync(bool isOn, OC::PostCallback turnOnCB) const
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


void Light::getBrightnessAsync(OC::GetCallback getBrightnessCB)
{
        m_brightness.getBrightnessAsync(getBrightnessCB);
}
void Light::setBrightnessAsync(int brightness, OC::PostCallback setBrightnessCB) const
{
    m_brightness.setBrightnessAsync(brightness, setBrightnessCB);
}

//Overloaded operator used for putting into a 'set'
bool Light::operator<(const Light &other) const
{
    return ((*m_resource) < (*(other.m_resource)));
}

void Light::onGetServices(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode)
{
    try {
        std::unique_lock<std::mutex> lock(m_mutex);
    } catch ( std::system_error se) {
        // class destructor has already been called return with no action;
        return;
    }
    OC::AttributeValue links;
    if(rep.hasAttribute("links")) {
        if(rep.getAttributeValue("links", links)) {
            try{
                auto linkRepresentation = boost::get<std::vector<OC::OCRepresentation> >(links);
                for(auto l: linkRepresentation) {
                    if(l.hasAttribute("rel")) {
                        std::string rel;
                        if(l.getValue("rel", rel)) {
                            std::cout << "rel: " << rel << std::endl;
                            if("contains" == rel) {
                                if(l.hasAttribute("rt")) {
                                    std::string rt;
                                    if(l.getValue("rt", rt)) {
                                        std::cout << "rt: " << rt << std::endl;
                                        if("oic.r.switch.binary" == rt) {
                                            if(l.hasAttribute("href")) {
                                                std::string href;
                                                l.getValue("href", href);
                                                std::cout << "href: " << href << std::endl;
                                                m_binarySwitchUri = href;
                                                onFoundBinarySwitchCB = std::bind(&Light::onFoundBinarySwitch, this, std::placeholders::_1);
                                                OC::OCPlatform::findResource("", std::string(OC_RSRVD_WELL_KNOWN_URI) + "?rt=" + rt, CT_DEFAULT, onFoundBinarySwitchCB);
                                            }
                                        } else if("oic.r.light.brightness" == rt) {
                                            if(l.hasAttribute("href")) {
                                                std::string href;
                                                l.getValue("href", href);
                                                std::cout << "href: " << href << std::endl;
                                                m_brightnessUri = href;
                                                onFoundBrightnessCB = std::bind(&Light::onFoundBrightness, this, std::placeholders::_1);
                                                OC::OCPlatform::findResource("", std::string(OC_RSRVD_WELL_KNOWN_URI) + "?rt=" + rt, CT_DEFAULT, onFoundBrightnessCB);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } catch(boost::bad_get &e) {
                std::cout << e.what() << std::endl;
                std::cout << "falure to getValue for links" << std::endl;
                std::cout << rep.getValueToString("links");
            }
        } else {
            std::cout << "falure to getValue for links" << std::endl;
            std::cout << rep.getValueToString("links");
        }
    }
    m_supported_services_known = true;
}

void Light::onFoundBinarySwitch(std::shared_ptr< OC::OCResource > resource) {
    try {
        std::unique_lock<std::mutex> lock(m_mutex);
    } catch ( std::system_error se) {
        // class destructor has already been called return with no action;
        return;
    }
    if (resource->uri() == m_binarySwitchUri) {
        m_binarySwitch = BinarySwitch(resource);
    }
}
void Light::onFoundBrightness(std::shared_ptr< OC::OCResource > resource) {
    try {
        std::unique_lock<std::mutex> lock(m_mutex);
    } catch ( std::system_error se) {
        // class destructor has already been called return with no action;
        return;
    }
    if (resource->uri() == m_brightnessUri) {
        m_brightness= Brightness(resource);
    }
}
