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

#include <OCResource.h>
#include <iostream>
#include <mutex>
#include <condition_variable>

#include "IotivityUtility.h"

// mutex used to prevent multiple items printing at the same time making the output unreadable
static std::mutex g_iotivity_utility_print_mutex;
static std::condition_variable g_iotivity_utility_condition_variable;

void on_iotivity_utility_get(const OC::HeaderOptions &headerOptions,
                             const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        if (rep.hasAttribute("name"))
        {
            std::string name = "";
            rep.getValue("name", name);
            std::cout << "\tname " << name << std::endl;
        }
    }
    g_iotivity_utility_condition_variable.notify_one();
}

void printResourceInformation(std::shared_ptr< OC::OCResource > resource)
{
    std::lock_guard<std::mutex> lock(g_iotivity_utility_print_mutex);
    try
    {
        std::cout << "------------------------------------------------------" << std::endl;
        std::cout << "DISCOVERED Resource:" << std::endl;
        std::cout << "\tResource address: " << resource->host() << std::endl;
        std::cout << "\tResource uri: " << resource->uri() << std::endl;
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
    }
    catch (std::exception &e)
    {
        std::cerr << "Caught exception in printResourceInformation: " << e.what() << std::endl;
    }
}

void printResourceCompact(std::shared_ptr< OC::OCResource > resource)
{
    std::unique_lock<std::mutex> lock(g_iotivity_utility_print_mutex);
    try
    {
        resource->get(OC::QueryParamsMap(), &on_iotivity_utility_get);
        g_iotivity_utility_condition_variable.wait(lock);
        std::cout << "\t" << resource->host() << resource->uri() << std::endl // uri is links.href
                  << "\tresourceTypes ";
        for (auto rt : resource->getResourceTypes())
        {
            std::cout << rt << " ";
        }
        std::cout << std::endl;
        std::cout << "\tresourceInterfaces ";
        for (auto ri : resource->getResourceInterfaces())
        {
            std::cout << ri << " ";
        }
        std::cout << std::endl;
        std:: cout << "\tsid " << resource->sid() << std::endl;
    }
    catch (std::exception &e)
    {
        std::cerr << "Caught exception in printResourceInformation: " << e.what() << std::endl;
    }
}

void processAttributes(const OC::OCRepresentation &rep,
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
                    OC::OCRepresentation internal;
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
