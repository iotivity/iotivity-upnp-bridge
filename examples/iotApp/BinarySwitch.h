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

#ifndef BINARYSWITCH_H_
#define BINARYSWITCH_H_

#include <OCResource.h>
#include <OCRepresentation.h>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>

class BinarySwitch
{
public:
    BinarySwitch();
    BinarySwitch(std::shared_ptr<OC::OCResource> resource);
    virtual ~BinarySwitch();
    BinarySwitch( const BinarySwitch& other );
    BinarySwitch& operator=(const BinarySwitch& other);

    /**
     * Used to check is the BinarySwitch is off or on
     *
     * \return true if switch is on false if switch is off
     */
    bool isOn();

    /**
     * This is a helper function that calls isOn() if the switch is on
     * it will use turnOn(false) to turn it off otherwise it will use
     * turnOn(true) to turn the light off.
     *
     * This function will not return till the switch state is changed.
     * If you want async behavoir then use the isOnAsyn() and
     * turnOnAsync() member functions.
     */
    void toggle();

    /**
     * Used to turn the switch on or off
     *
     * \param isOn set to true to turn switch on set to false to turn switch off
     * return true if the
     */
    bool turnOn(bool isOn);

    void isOnAsync(OC::GetCallback isOnCB);
    void turnOnAsync(bool isOn, OC::PostCallback turnOnCB) const;


    //Overloaded operator used for putting into a 'set'
    bool operator<(const BinarySwitch &other) const;
    const std::shared_ptr<OC::OCResource> getResource() const {return m_resource;}
private:
    void onGetPowerSwitch(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
    void onPostPowerSwitch(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);

    OC::GetCallback m_getCB;
    OC::PostCallback m_postCB;
    std::shared_ptr<OC::OCResource> m_resource;
    bool m_powerState;
    int m_eCode;
    //used to turn the async get call to a sync method call
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

#endif /* BINARYSWITCH_H_ */
