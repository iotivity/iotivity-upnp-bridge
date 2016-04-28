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

#ifndef UPNP_INTERNAL_H_
#define UPNP_INTERNAL_H_

#include <gupnp.h>

#include <map>
#include <string>

#include <RCSResourceAttributes.h>

#include <UpnpConstants.h>

using namespace OIC::Service;

//From UPNP spec
static const std::string UPNP_PREFIX_DEVICE = "urn:schemas-upnp-org:device";
static const std::string UPNP_PREFIX_SERVICE = "urn:schemas-upnp-org:service";
static const std::string UPNP_PREFIX_SERVICE_ID = "urn:upnp-org:serviceId:";

// Device patterns
static const std::string UPNP_PATTERN_DEVICE_LIGHT = UPNP_PREFIX_DEVICE + ":.*(?:[Ll][Ii][Gg][Hh][Tt]).*";
static const std::string UPNP_PATTERN_DEVICE_INET_GATEWAY  = UPNP_PREFIX_DEVICE + ":.*(?:[Ii]nternet[Gg]ateway[Dd]evice).*";
static const std::string UPNP_PATTERN_DEVICE_MEDIA_RENDERER  = UPNP_PREFIX_DEVICE + ":.*(?:[Mm]edia[Rr]enderer).*";
static const std::string UPNP_PATTERN_DEVICE_MEDIA_SERVER  = UPNP_PREFIX_DEVICE + ":.*(?:[Mm]edia[Ss]erver).*";

// Service Patterns
static const std::string UPNP_PATTERN_SERVICE_SWITCH_POWER = UPNP_PREFIX_SERVICE + ":.*(?:[Ss]witch[Pp]ower).*";
static const std::string UPNP_PATTERN_SERVICE_DIMMING = UPNP_PREFIX_SERVICE + ":.*(?:[Dd]imming).*";
static const std::string UPNP_PATTERN_SERVICE_CONNECTION_MANAGER = UPNP_PREFIX_SERVICE + ":.*(?:[Cc]onnection[Mm]anager).*";
static const std::string UPNP_PATTERN_SERVICE_AV_TRANSPORT = UPNP_PREFIX_SERVICE + ":.*(?:[Aa][Vv][Tt]ransport).*";
static const std::string UPNP_PATTERN_SERVICE_CONTENT_DIRECTORY = UPNP_PREFIX_SERVICE + ":.*(?:[Cc]ontent[Dd]irectory).*";
static const std::string UPNP_PATTERN_SERVICE_RENDERING_CONTROL = UPNP_PREFIX_SERVICE + ":.*(?:[Rr]endering[Cc]ontrol).*";
static const std::string UPNP_PATTERN_SERVICE_SCHEDULED_RECORDING = UPNP_PREFIX_SERVICE + ":.*(?:[Ss]chedule[Rr]ecording).*";

// Root device
static const std::string UPNP_ROOT_DEVICE = "upnp:rootdevice";


// TODO: discuss if the following maps should be moved to external header file like UpnpConstants.h

// Interface map
// TODO: the mapping needs TBD and expanded
static std::map<std::string, std::string > UpnpInterfaceMap =
{
    {UPNP_OIC_TYPE_DEVICE_LIGHT,             "oic.if.ll"},
    {UPNP_OIC_TYPE_DEVICE_INET_GATEWAY,      "oic.if.ll"},
    {UPNP_OIC_TYPE_DEVICE_MEDIA_RENDERER,    "oic.if.ll"},
    {UPNP_OIC_TYPE_DEVICE_MEDIA_SERVER,      "oic.if.ll"},
    {UPNP_OIC_TYPE_DIMMING,                  "oic.if.a"},
    {UPNP_OIC_TYPE_POWER_SWITCH,             "oic.if.a"},
    {UPNP_OIC_TYPE_BRIGHTNESS,               "oic.if.a"}
};

// URI prexfix map
// TODO and TBD
static std::map<std::string, std::string > UpnpUriPrefixMap =
{
    {UPNP_OIC_TYPE_DEVICE_LIGHT,            UPNP_OIC_URI_PREFIX_LIGHT},
    {UPNP_OIC_TYPE_DIMMING,                 UPNP_OIC_URI_PREFIX_DIMMING},
    {UPNP_OIC_TYPE_BRIGHTNESS,              UPNP_OIC_IRI_PREFIX_BRIGHTNESS},
    {UPNP_OIC_TYPE_POWER_SWITCH,            UPNP_OIC_URI_PREFIX_POWER_SWITCH},
    {UPNP_OIC_TYPE_DEVICE_INET_GATEWAY,     UPNP_OIC_URI_PREFIX_INET_GATEWAY},
    {UPNP_OIC_TYPE_DEVICE_MEDIA_RENDERER,   UPNP_OIC_URI_PREFIX_MEDIA_RENDERER},
    {UPNP_OIC_TYPE_DEVICE_MEDIA_SERVER,     UPNP_OIC_URI_PREFIX_MEDIA_SERVER},
    {UPNP_OIC_TYPE_CONNECTION_MANAGER,      UPNP_OIC_URI_PREFIX_CONNECTION_MANAGER},
    {UPNP_OIC_TYPE_AV_TRANSPORT,            UPNP_OIC_URI_PREFIX_AV_TRANSPORT},
    {UPNP_OIC_TYPE_CONTENT_DIRECTORY,       UPNP_OIC_URI_PREFIX_CONTENT_DIRECTORY},
    {UPNP_OIC_TYPE_RENDERING_CONTROL,       UPNP_OIC_URI_PREFIX_RENDERING_CONTROL},
    {UPNP_OIC_TYPE_SCHEDULED_RECORDING,     UPNP_OIC_URI_PREFIX_SCHEDULED_RECORDING}
};


typedef enum
{
    UPNP_ACTION_GET = 1,
    UPNP_ACTION_PUT = 2,
    UPNP_ACTION_POST = 4,
    UPNP_ACTION_DELETE = 8
} UpnpActionType;

//typedef std::function< bool(void) > GetAttributeHandler;
//typedef std::function< bool(RCSResourceAttributes::Value&) > SetAttributeHandler;

typedef struct _UpnpAttributeInfo
{
    std::string name;
    const char* stateVar;
    GType type;
    std::vector < std::pair <std::string, UpnpActionType>> actions;
} UpnpAttributeInfo;

#define ERROR_PRINT(x) do { std::cerr << MODULE << ":" << __func__ << "(): ERROR: " << x << std::endl; } while (0)

#ifndef NDEBUG
#define DEBUG_PRINT(x) do { std::cout << MODULE << ":" << __func__ << "(): " << x << std::endl; } while (0)
#else
#define DEBUG_PRINT(x)
#endif
#endif
