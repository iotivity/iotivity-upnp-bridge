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

#ifndef UPNP_INTERNAL_H_
#define UPNP_INTERNAL_H_

#include <gupnp.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <oc_api.h>

#include <UpnpConstants.h>

#define MAX_URI_LENGTH (256)

//IoTivity Link Relationships
static const std::string LINK_REL_CONTAINS = "contains";
static const std::string LINK_REL_HOSTS = "hosts";
static const std::string LINK_REL_ICON = "icon";

//From UPNP spec
static const std::string UPNP_PREFIX_DEVICE = "urn:schemas-upnp-org:device";
static const std::string UPNP_PREFIX_SERVICE = "urn:schemas-upnp-org:service";
static const std::string UPNP_PREFIX_SERVICE_ID = "urn:upnp-org:serviceId:";
static const std::string UPNP_PREFIX_UDN = "uuid:";

// Root device
static const std::string UPNP_ROOT_DEVICE = "upnp:rootdevice";

// Search patterns for UPnP Devices and services matched by resource type
static std::map<std::string, std::string > UpnpSearchPatternMap =
{
// Lighting Control
    {UPNP_OIC_TYPE_DEVICE_LIGHT,            UPNP_PREFIX_DEVICE  + ":.*(?:[Ll]ight).*"},
    {UPNP_OIC_TYPE_BRIGHTNESS,              UPNP_PREFIX_SERVICE + ":.*(?:[Dd]imming).*"},
    {UPNP_OIC_TYPE_POWER_SWITCH,            UPNP_PREFIX_SERVICE + ":.*(?:[Ss]witch[Pp]ower).*"},
// Media Control
    {UPNP_OIC_TYPE_AUDIO,                   UPNP_PREFIX_SERVICE + ":.*(?:[Rr]endering[Cc]ontrol).*"},
    {UPNP_OIC_TYPE_MEDIA_CONTROL,           UPNP_PREFIX_SERVICE + ":.*(?:[Aa][Vv][Tt]ransport).*"}
};

// TODO: discuss if the following maps should be moved to external header file like UpnpConstants.h

// Interface map
// TODO: the mapping needs TBD and expanded
static std::map<std::string, std::string > UpnpInterfaceMap =
{
// Lighting Control
    {UPNP_OIC_TYPE_DEVICE_LIGHT,             "oic.if.baseline"},
    {UPNP_OIC_TYPE_POWER_SWITCH,             "oic.if.a"},
    {UPNP_OIC_TYPE_BRIGHTNESS,               "oic.if.a"},
// Media Control
    {UPNP_OIC_TYPE_DEVICE_MEDIA_RENDERER,    "oic.if.baseline"},
    {UPNP_OIC_TYPE_DEVICE_MEDIA_SERVER,      "oic.if.baseline"},
};

// URI prexfix map
// TODO and TBD
static std::map<std::string, std::string > UpnpUriPrefixMap =
{
    {UPNP_OIC_TYPE_DEVICE_LIGHT,            UPNP_OIC_URI_PREFIX_LIGHT},
    {UPNP_OIC_TYPE_BRIGHTNESS,              UPNP_OIC_URI_PREFIX_BRIGHTNESS},
    {UPNP_OIC_TYPE_POWER_SWITCH,            UPNP_OIC_URI_PREFIX_POWER_SWITCH},

    {UPNP_OIC_TYPE_AV_TRANSPORT,            UPNP_OIC_URI_PREFIX_AV_TRANSPORT},
    {UPNP_OIC_TYPE_RENDERING_CONTROL,       UPNP_OIC_URI_PREFIX_RENDERING_CONTROL}
};

#define ERROR_PRINT(x) do { std::cerr << MODULE << ":" << __func__ << "(): ERROR: " << x << std::endl; } while (0)

#ifndef NDEBUG
#define DEBUG_PRINT(x) do { std::cout << MODULE << ":" << __func__ << "(): " << x << std::endl; } while (0)
#else
#define DEBUG_PRINT(x)
#endif
#endif
