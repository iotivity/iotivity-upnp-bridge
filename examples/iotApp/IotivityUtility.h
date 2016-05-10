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

#ifndef IOTIVITYUTILITY_H_
#define IOTIVITYUTILITY_H_

#include <OCResource.h>
#include <iostream>
#include <mutex>

// mutex used in a lock_guard to prevent multiple items printing at the same time making the output
// unreadable
std::mutex print_mutex;

void printResourceInformation(std::shared_ptr< OC::OCResource > resource) {
    std::lock_guard<std::mutex> lock(print_mutex);
    try {
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
        } catch (std::exception &e) {
            std::cerr << "Caught exception in printResourceInformation: " << e.what() << std::endl;
        }
}

void printResourceCompact(std::shared_ptr< OC::OCResource > resource) {
    std::lock_guard<std::mutex> lock(print_mutex);
    try {
        std::cout << "\t" << resource->host() << resource->uri() << std::endl // uri is links.href
                << "\tresourceTypes ";
        for(auto rt : resource->getResourceTypes()) {
            std::cout << rt << " ";
        }
        std::cout << std::endl;
        std::cout << "\tresourceInterfaces ";
        for(auto ri : resource->getResourceInterfaces()) {
            std::cout << ri << " ";
        }
        std::cout << std::endl;
        std:: cout << "\tsid " << resource->sid() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Caught exception in printResourceInformation: " << e.what() << std::endl;
    }
}

#endif /* EXAMPLES_IOTAPP_IOTIVITYUTILITY_H_ */
