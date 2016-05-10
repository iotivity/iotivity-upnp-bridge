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

#ifndef BRIGHTNESS_H_
#define BRIGHTNESS_H_

#include <OCResource.h>
#include <OCRepresentation.h>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>

class Brightness
{
public:
    Brightness();
    Brightness(std::shared_ptr<OC::OCResource> resource);
    virtual ~Brightness();
    Brightness( const Brightness& other );
    Brightness& operator=(const Brightness& other);

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
     * return true if the brightness was set
     */
    bool setBrightness(int brightness);

    void getBrightnessAsync(OC::GetCallback isOnCB);
    void setBrgithnessAsync(int brightness, OC::PostCallback turnOnCB) const;


    //Overloaded operator used for putting into a 'set'
    bool operator<(const Brightness &other) const;
    const std::shared_ptr<OC::OCResource> getResource() const {return m_resource;}
private:
    void onGetBrightness(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
    void onPostBrightness(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);

    OC::GetCallback m_getCB;
    OC::PostCallback m_postCB;
    std::shared_ptr<OC::OCResource> m_resource;
    int m_brightness;
    int m_eCode;
    //used to turn the async get call to a sync method call
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

#endif /* BRIGHTNESS_H_ */
