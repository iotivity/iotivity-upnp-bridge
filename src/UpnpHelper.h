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

#ifndef UPNP_HELPER_H_
#define UPNP_HELPER_H_

#include <boost/regex.hpp>

#include <UpnpConstants.h>
#include "UpnpInternal.h"

static std::string findResourceType(std::string type)
{
    //    if (regex_match(type, regex("urn:schemas-upnp-org:device:.*(?:[Ll][Ii][Gg][Hh][Tt]).*")))
    if (boost::regex_match(type, boost::regex(UPNP_PATTERN_DEVICE_LIGHT)))
    {
        return UPNP_OIC_TYPE_DEVICE_LIGHT;
    }
    else if (boost::regex_match(type, boost::regex(UPNP_PATTERN_SERVICE_SWITCH_POWER)))
    {
        return UPNP_OIC_TYPE_POWER_SWITCH;
    }
    else if (boost::regex_match(type, boost::regex(UPNP_PATTERN_SERVICE_DIMMING)))
    {
        return UPNP_OIC_TYPE_BRIGHTNESS;
    }
    else if (boost::regex_match(type, boost::regex(UPNP_PATTERN_DEVICE_INET_GATEWAY)))
    {
        return UPNP_OIC_TYPE_DEVICE_INET_GATEWAY;
    }
    else
    {
        //TODO: change to something more intelligent here, e.g. exeption and/or error code, etc.
        return "";
    }
}

#endif
