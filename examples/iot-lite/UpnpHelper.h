//******************************************************************
//
// Copyright 2018 Intel Corporation All Rights Reserved.
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

#ifndef UPNP_HELPER_H_
#define UPNP_HELPER_H_

#include <boost/regex.hpp>

#include <UpnpConstants.h>
#include "UpnpInternal.h"

static std::string findResourceType(std::string type)
{
    for (auto it : UpnpSearchPatternMap)
    {
        if (boost::regex_match(type, boost::regex(it.second)))
        {
            return it.first;
        }
    }

    //TODO: change to something more intelligent here, e.g. exeption and/or error code, etc.
    return "";
}

#endif
