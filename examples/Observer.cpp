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
#include <mutex>

#include <OCApi.h>
#include <OCPlatform.h>

#include "UpnpConstants.h"

using namespace OC;

static std::vector< std::string> s_foundURIs;
std::mutex s_mutexFind;

void onObserveResource(const HeaderOptions &headerOptions,
                           const OCRepresentation &rep,
                           const int eCode,
                           const int sequenceNumber)
{
    try
    {
        if (eCode == OC_STACK_OK && sequenceNumber != OC_OBSERVE_NO_OPTION)
        {
            if (sequenceNumber == OC_OBSERVE_REGISTER)
            {
                std::cout << "Observe registration action is successful for " << rep.getUri() << std::endl;
            }
            else if (sequenceNumber == OC_OBSERVE_DEREGISTER)
            {
                std::cout << "Observe deregistration action is successful for " << rep.getUri() << std::endl;
            }

            std::cout << "OBSERVE result for: " << rep.getUri() << std::endl;
            std::cout << "\tsequenceNumber: " << sequenceNumber << std::endl;
            // PowerSwitch values
            if(rep.hasAttribute("value")) {
                bool newState;
                rep.getValue("value", newState);
                std::cout << "\tNew power value: " << ((newState) ? "ON" : "OFF") << std::endl;
            }
            // Dimming/Brightness values
            if(rep.hasAttribute("brightness")) {
                int newBrightness = 101;
                rep.getValue("brightness", newBrightness);
                std::cout << "\tNew brightness status: " << newBrightness << "%" << std::endl;
            }
        }
        else
        {
            if (sequenceNumber == OC_OBSERVE_NO_OPTION)
            {
                std::cout << "Observe registration or deregistration action is failed" << std::endl;
            }
            else
            {
                std::cout << "onObserveResource response error: " << eCode << std::endl;
            }
        }
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << " in onObserveResource" << std::endl;
    }
}

void foundResource(std::shared_ptr< OCResource > resource)
{
    std::string resourceUri;
    std::string hostAddress;
    std::lock_guard<std::mutex> lock(s_mutexFind);

    try
    {
        if (resource)
        {
            resourceUri = resource->uri();
            hostAddress = resource->host();

            if (find(s_foundURIs.begin(), s_foundURIs.end(), resourceUri) == s_foundURIs.end())
            {
                std::cout << "Resource uri: " << resourceUri << std::endl;
                std::cout << "\t address: " << hostAddress << std::endl;

                s_foundURIs.push_back(resourceUri);
                if((resourceUri != "/oic/p") && (resourceUri != "/oic/d"))
                {
                    for (auto &resourceType : resource->getResourceTypes())
                    {
                        std::cout << "\t\t" << resourceType << std::endl;
                        if ((resourceType == UPNP_OIC_TYPE_DEVICE_LIGHT) || (resourceType == UPNP_OIC_TYPE_POWER_SWITCH) || (resourceType == UPNP_OIC_TYPE_BRIGHTNESS))
                        {
                            resource->observe(ObserveType::Observe, QueryParamsMap(),
                                              &onObserveResource);
                        }
                    }
                }
            }
        }
    }
    catch (std::exception &e)
    {
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

        while (true)
        {
            OCPlatform::findResource("", OC_RSRVD_WELL_KNOWN_URI, CT_DEFAULT, &foundResource);
            usleep(5000000); // sleep for 5 seconds
        }

    }
    catch (OCException &e)
    {
        oclog() << "Exception in main: " << e.what();
    }
    return 0;
}
