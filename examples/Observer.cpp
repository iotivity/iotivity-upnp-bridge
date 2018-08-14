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

#include <UpnpConstants.h>
#include <UpnpBridgeAttributes.h>

using namespace OC;

static std::vector< std::string> s_foundURIs;
std::mutex s_mutexFind;

static std::map <std::string, std::vector <std::string>> resourceTypeMap;

static void processAttributes(const OCRepresentation &rep,
                              const std::map <std::string, AttrDesc> *attrMap,
                              std::string prefix)
{
    for (auto &attr : *attrMap)
    {
        if (rep.hasAttribute(attr.first))
        {
            std::cout << prefix << attr.first << "";
        }
        else
        {
            continue;
        }

        switch (attr.second.type)
        {
            case ATTR_TYPE_BOOL:
                {
                    bool value;
                    rep.getValue(attr.first, value);
                    std::cout << " (bool):\t " << ((value) ? "TRUE" : "FALSE") << std::endl;
                    break;
                }
            case ATTR_TYPE_INT:
                {
                    int value;
                    rep.getValue(attr.first, value);
                    std::cout << " (int):\t " << value << std::endl;
                    break;
                }
            case ATTR_TYPE_INT64:
                {
                    double value;
                    rep.getValue(attr.first, value);
                    std::cout << " (int64): \t " << value << std::endl;
                    break;
                }
            case ATTR_TYPE_STRING:
                {
                    std::string value;
                    rep.getValue(attr.first, value);
                    std::cout << " (string): \t " << value << std::endl;
                    break;
                }
            case ATTR_TYPE_VECTOR:
                {
                    OCRepresentation internal;
                    rep.getValue(attr.first, internal);
                    std::cout << std::endl;
                    processAttributes(internal, attr.second.composite, prefix + "\t");
                    break;
                }
            default:
                {
                    std::cout << "not handled yet" << std::endl;
                    break;
                }
        }
    }

}

static void onObserveResource(const HeaderOptions &headerOptions,
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
            std::cout << "\t sequenceNumber: " << sequenceNumber << std::endl;

            for (auto &resourceType : resourceTypeMap[rep.getUri()])
            {
                std::cout << "\t\t" << resourceType << std::endl;
                auto it = ResourceAttrMap.find(resourceType);

                if (it == ResourceAttrMap.end())
                {
                    std::cout << "RESOURCE MAP NOT FOUND" << std::endl;
                    return;
                }

                const std::map <std::string, AttrDesc> &attrMap = it->second;

                processAttributes(rep, &attrMap, "\t");

                std::cout << "\n***********************************************************\n";
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
    catch (std::exception &e)
    {
        std::cout << "Exception: " << e.what() << " in onObserveResource" << std::endl;
    }
}

static void foundResource(std::shared_ptr< OCResource > resource)
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
                if ((resourceUri != "/oic/p") && (resourceUri != "/oic/d"))
                {
                    for (auto &resourceType : resource->getResourceTypes())
                    {
                        std::cout << "\t\t" << resourceType << std::endl;
                        auto it = ResourceAttrMap.find(resourceType);
                        resourceTypeMap[resourceUri].push_back(resourceType);

                        if (it != ResourceAttrMap.end())
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
    {
        OC::ServiceType::InProc, OC::ModeType::Both, OC_DEFAULT_ADAPTER, OC::QualityOfService::LowQos,
        NULL
    };
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
