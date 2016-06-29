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
#include <OCRepresentation.h>
#include <iostream>
#include <mutex>

#include <UpnpBridgeAttributes.h>

const int MAX_WAIT_TIME_FOR_BLOCKING_CALL = 20;

void printResourceInformation(std::shared_ptr< OC::OCResource > resource);
void printResourceCompact(std::shared_ptr< OC::OCResource > resource);

void processAttributes(const OC::OCRepresentation &rep,
                       const std::map <std::string, AttrDesc> *attrMap,
                       std::string prefix);

// Comparison operator used to insert the OCResources into the sets.
struct OCResourceComp
{
    bool operator()(std::shared_ptr<OC::OCResource> lhs, std::shared_ptr<OC::OCResource> rhs) const
    {
        return (*lhs) < (*rhs);
    }
};

#endif /* IOTIVITYUTILITY_H_ */
