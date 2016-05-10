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
#ifndef COMMANDLINEMODE_H_
#define COMMANDLINEMODE_H_

#include <OCApi.h>
#include <OCPlatform.h>
#include <unistd.h>

#include <functional>
#include <memory>
#include <string>

#include "IotivityUtility.h"

class Commandline {
public:
    Commandline();
    void process(int argc, char *argv[]);
private:
    void foundResource(std::shared_ptr< OC::OCResource > resource);
    void testBrightness(std::shared_ptr< OC::OCResource > resource);
    void testSwitch(std::shared_ptr< OC::OCResource > resource);
    bool listAllDevices;
    bool listAllServices;
    bool runTest;
    int waitTime;

    std::shared_ptr<OC::OCResource> m_devices;
    std::shared_ptr<OC::OCResource> m_services;
    std::shared_ptr<OC::OCResource> m_other;

    OC::FindCallback onFoundCB;
    OC::FindCallback testBrightnessCB;
    OC::FindCallback testSwitchCB;
};

Commandline::Commandline() :
    listAllDevices(false),
    listAllServices(false),
    runTest(false),
    waitTime(-1){

    onFoundCB        = std::bind(&Commandline::foundResource, this, std::placeholders::_1);
    testBrightnessCB = std::bind(&Commandline::testBrightness, this, std::placeholders::_1);
    testSwitchCB     = std::bind(&Commandline::testSwitch, this, std::placeholders::_1);

}

void Commandline::process(int argc, char *argv[])
{
    bool listAllDevices = false;
    bool listAllServices = false;
    /* Parse command line args */
    for (int i = 1; i < argc; ++i) {
        if(0 == strcmp("--list", argv[i])) {
            listAllDevices = true;
            listAllServices = true;
        }
        if(0 == strcmp("--time", argv[i])) {
            if(i < argc) {
                ++i;
                waitTime = std::stoi(argv[i]);
                //std::cout << "Waiting " << waitTime << " seconds before exiting." << std::endl;
            }
        }
        if(0 == strcmp("--test", argv[i])) {
            runTest = true;
        }

    }
    if(listAllDevices || listAllServices) {
        std::cout << "Finding all Resources."  << std::endl;
        OC::OCPlatform::findResource("", OC_RSRVD_WELL_KNOWN_URI, CT_DEFAULT, onFoundCB);
    }
    // Run pre-programmed test code that is used to verify functionality of found resource
    if(runTest) {
        OC::OCPlatform::findResource("", std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_BRIGHTNESS, CT_DEFAULT, testBrightnessCB);
        OC::OCPlatform::findResource("", std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_POWER_SWITCH, CT_DEFAULT, testSwitchCB);
    }
    if (-1 == waitTime) {
        std::cout << "Enter 'q' to exit program." << std::endl;
        while (getchar() != 'q');
    } else {
        std::cout << "Waiting " << waitTime << " seconds for all actions to complete before exiting." << std::endl;
        sleep(waitTime); //wait x seconds then exit
        std::cout << "EXITING" << std::endl;
    }
}

void Commandline::foundResource(std::shared_ptr< OC::OCResource > resource) {
    //printResourceCompact(resource);
    printResourceInformation(resource);
}

void Commandline::testBrightness(std::shared_ptr< OC::OCResource > resource) {
    Brightness b(resource);
    b.getBrightnessAsync([b](const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode)
            {
        if (eCode == OC_STACK_OK)
        {
            int brightness;
            std::string uri;
            rep.getValue("uri", uri);
            if(uri == b.getResource()->uri()) {
                rep.getValue("brightness", brightness);
            }
            brightness -= 20;
            brightness = ((brightness < 20) ? 100 : brightness); //don't drop below 20% so we can tell light is on
            b.setBrgithnessAsync(brightness, [](const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode){});
        }
            });
}

void Commandline::testSwitch(std::shared_ptr< OC::OCResource > resource) {
    BinarySwitch s(resource);
    s.isOnAsync([s](const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode)
            {
        if (eCode == OC_STACK_OK)
        {
            bool powerState;
            std::string uri;
            rep.getValue("uri", uri);
            if(uri == s.getResource()->uri()) {
                rep.getValue("value", powerState);
            }
            s.turnOnAsync(!powerState, [](const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode){});
        }
            });
}

#endif
