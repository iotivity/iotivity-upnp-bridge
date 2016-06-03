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

#ifndef LIGHT_H_
#define LIGHT_H_

#include <OCResource.h>
#include <OCRepresentation.h>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "BinarySwitch.h"
#include "Brightness.h"

class Light
{
public:
    Light();
    Light(OC::OCResource::Ptr resource);
    virtual ~Light();
    Light( const Light& other );
    Light& operator=(const Light& other);

    /**
     * Used to check is the BinarySwitch is off or on
     *
     * \return true if switch is on false if switch is off
     */
    bool isOn();

    /**
     * This is a helper function that toggles the switch state.
     * Internally, it usues isOn() and turnOn() to check and change the switch state.
     *
     * The function is synchronous, i.e., it will not return till the switch state is changed.
     *
     * If you want async behavoir then use the isOnAsyn() and
     * turnOnAsync() member functions.
     */
    void toggle();

    /**
     * Used to turn the switch on or off
     *
     * \param isOn set to true to turn switch on set to false to turn switch off
     * \return true if the
     */
    bool turnOn(bool isOn);

    void isOnAsync(OC::GetCallback isOnCB);
    void turnOnAsync(bool isOn, OC::PostCallback turnOnCB) const;

    /**
     * Used to get the Brightness 0-100
     *
     * The integer is the percentage of of brightness 100 is max value.
     *
     * \return brightness level between 0 and 100;
     */
    int getBrightness();

    /**
     * Used to set brightness percentage value is between 0 and 100
     *
     * \param brightness brightness from 0% to 100%
     * \return true if the brightness was set
     */
    bool setBrightness(int brightness);

    void getBrightnessAsync(OC::GetCallback isOnCB);
    void setBrightnessAsync(int brightness, OC::PostCallback turnOnCB);

    //Overloaded operator used for putting into a 'set'
    bool operator<(const Light &other) const;
    const OC::OCResource::Ptr getResource() const {return m_resource;}
private:
    /*
     * Intended to be called before using the remote service to verify remote methods that discover
     * services have completed.
     */
    bool init();
    void onGetServices(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
    void onFoundBinarySwitch(OC::OCResource::Ptr resource);
    void onFoundBrightness(OC::OCResource::Ptr resource);

    OC::GetCallback onGetServicesCB;
    OC::GetCallback m_getCB;
    OC::PostCallback m_postCB;

    OC::OCResource::Ptr m_resource;
    bool m_supported_services_known;
    bool m_lightSupportsBrightness;
    bool m_lightSupportsSwitch;
    BinarySwitch m_binarySwitch;
    Brightness m_brightness;

    bool m_powerState;
    int m_eCode;
    // used to turn the async get call to a sync method call as well as make sure services are
    // discovered before they are used.
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

#endif /* LIGHT_H_ */
