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

#include <OCApi.h>
#include <OCPlatform.h>
#include <mutex>
#include <thread>
#include <set>
#include "UpnpConstants.h"
#include "BinarySwitch.h"
#include "Brightness.h"

using namespace OC;
using namespace std;

std::mutex print_mutex;

struct BinarySwitchComp
{
    bool operator()(const BinarySwitch& lhs, const BinarySwitch& rhs) const
    {
        return lhs < rhs;
    }
};

struct BrightnessComp
{
    bool operator()(const Brightness& lhs, const Brightness& rhs) const
    {
        return lhs < rhs;
    }
};

set<BinarySwitch, BinarySwitchComp> g_binarySwitchSet;
set<Brightness, BrightnessComp> g_brightnessSet;

void printResourceInformation(std::shared_ptr< OCResource > resource) {
    // lock_guard used to prevent multiple items printing at the same time making the out unreadable
    std::lock_guard<std::mutex> lock(print_mutex);
    try {
            std::cout << "------------------------------------------------------" << endl;
            std::cout << "DISCOVERED Resource:" << std::endl;
            std::cout << "\tResource uri: " << resource->uri() << std::endl;
            std::cout << "\tResource address: " << resource->host() << std::endl;
            std::cout << "\tList of resource interfaces: " << std::endl;
            for (auto &resourceInterface : resource->getResourceInterfaces())
            {
                std::cout << "\t\t" << resourceInterface << std::endl;
            }

            std::cout << "\tList of resource types: " << std::endl;
            for (auto &resourceType : resource->getResourceTypes())
            {
                std::cout << "\t\t" << resourceType << std::endl;
            }
        } catch (std::exception &e) {
            std::cerr << "Caught exception in printResourceInformation: " << e.what() << std::endl;
        }
}
void foundResource(std::shared_ptr< OCResource > resource)
{
    try {
        //Ignore resources with uri /oic/d and /oic/p
        if (resource->uri() == "/oic/d" || resource->uri() == "/oic/p") {
            return;
        }
        printResourceInformation(resource);

        for (auto &resourceType : resource->getResourceTypes())
        {
            if(resourceType == UPNP_OIC_TYPE_POWER_SWITCH) {
                g_binarySwitchSet.insert(resource);
            } else if(resourceType == UPNP_OIC_TYPE_BRIGHTNESS) {
                g_brightnessSet.insert(resource);
            }
        }
    } catch (std::exception &e) {
        std::cerr << "Caught exception in foundResource: " << e.what() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    PlatformConfig cfg =
    { OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos,
    NULL };
    OCPlatform::Configure(cfg);
    try
    {
        std::cout << "Finding Resource... " << std::endl;
        OCPlatform::findResource("", std::string(OC_RSRVD_WELL_KNOWN_URI), CT_DEFAULT, &foundResource);
        if (argc > 1)
        {
            while (true)
            {
                usleep(5000000); // sleep for 5 seconds
                OCPlatform::findResource("", OC_RSRVD_WELL_KNOWN_URI, CT_DEFAULT, &foundResource);
            }
        }
        else
        {
            usleep(10000000); // run for 10s.
        }

    }
    catch (OCException &e)
    {
        oclog() << "Exception in main: " << e.what();
    }

    //Use the sync methods
    if(!g_binarySwitchSet.empty()) {
        for (auto s: g_binarySwitchSet) {
            // this is a blocking call so it can only toggle one switch at a time.
            s.toggle();
        }
    }
    if(!g_brightnessSet.empty()) {
        for (auto b: g_brightnessSet) {
            int brightness = b.getBrightness() - 20;
            brightness = ((brightness < 20) ? 100 : brightness); //don't drop below 20% so we can tell light is on
            b.setBrightness(brightness);
        }
    }
    usleep(1000000); //wait a second then toggle the lights again

    //Use the Async methods
    if(!g_brightnessSet.empty()) {
        for (auto b: g_brightnessSet) {
            b.getBrightnessAsync([b](const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
                    {
                if (eCode == OC_STACK_OK)
                {
                    int brightness;
                    string uri;
                    rep.getValue("uri", uri);
                    if(uri == b.getResource()->uri()) {
                        rep.getValue("brightness", brightness);
                    }
                    brightness -= 20;
                    brightness = ((brightness < 20) ? 100 : brightness); //don't drop below 20% so we can tell light is on
                    b.setBrgithnessAsync(brightness, [](const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode){});
                }
                    });
        }
    }
    if (!g_binarySwitchSet.empty()) {
        for(auto s: g_binarySwitchSet) {
            s.isOnAsync([s](const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
                    {
                if (eCode == OC_STACK_OK)
                {
                    bool powerState;
                    string uri;
                    rep.getValue("uri", uri);
                    if(uri == s.getResource()->uri()) {
                        rep.getValue("value", powerState);
                    }
                    s.turnOnAsync(!powerState, [](const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode){});
                }
                    });
        }
    }
    usleep(5000000); //sleep 2 sec to let async calls complete
    return 0;
}
