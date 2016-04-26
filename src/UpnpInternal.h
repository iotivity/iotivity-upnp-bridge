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
static const std::string UPNP_PATTERN_DEVICE_INET_GATEWAY  = UPNP_PREFIX_DEVICE + ":.*(?:[Ii][Nn][Tt][Ee][Rr][Nn][Ee][Tt][Gg][Aa][Tt][Ee][Ww][Aa][Yy][Dd][Ee][Vv][Ii][Cc][Ee]).*";
static const std::string UPNP_PATTERN_DEVICE_LIGHT = UPNP_PREFIX_DEVICE + ":.*(?:[Ll][Ii][Gg][Hh][Tt]).*";

// Service Patterns
static const std::string UPNP_PATTERN_SERVICE_SWITCH_POWER = UPNP_PREFIX_SERVICE + ":.*(?:[Ss]witch[Pp]ower).*";
static const std::string UPNP_PATTERN_SERVICE_DIMMING = UPNP_PREFIX_SERVICE + ":.*(?:[Dd]imming).*";

// Root device
static const std::string UPNP_ROOT_DEVICE = "upnp:rootdevice";


// TODO: discuss if the following maps should be moved to external header file like UpnpConstants.h

// Interface map
// TODO: the mapping needs TBD and expanded
static std::map<std::string, std::string > UpnpInterfaceMap =
{
    {UPNP_OIC_TYPE_DEVICE_LIGHT,             "oic.if.ll"},
    {UPNP_OIC_TYPE_DEVICE_INET_GATEWAY,      "oic.if.ll"},
    {UPNP_OIC_TYPE_DIMMING,                  "oic.if.a"},
    {UPNP_OIC_TYPE_POWER_SWITCH,             "oic.if.a"},
    {UPNP_OIC_TYPE_BRIGHTNESS,               "oic.if.a"}
};

// URI prexfix map
// TODO and TBD
static std::map<std::string, std::string > UpnpUriPrefixMap =
{
    {UPNP_OIC_TYPE_DEVICE_LIGHT,        UPNP_OIC_URI_PREFIX_LIGHT},
    {UPNP_OIC_TYPE_DIMMING,             UPNP_OIC_URI_PREFIX_DIMMING},
    {UPNP_OIC_TYPE_BRIGHTNESS,          UPNP_OIC_IRI_PREFIX_BRIGHTNESS},
    {UPNP_OIC_TYPE_POWER_SWITCH,        UPNP_OIC_URI_PREFIX_POWER_SWITCH},
    {UPNP_OIC_TYPE_DEVICE_INET_GATEWAY,  UPNP_OIC_URI_PREFIX_INET_GATEWAY},
};


typedef enum
{
    UPNP_ACTION_GET    = 1,
    UPNP_ACTION_POST   = 2,
    UPNP_ACTION_PUT    = 4,
    UPNP_ACTION_DELETE = 8
} UpnpActionType;

typedef struct _UpnpAction
{
    const char*    name;
    UpnpActionType type;

    // Argument descriptors for a simple UPnP action:
    // number of input parameters <= 1 xor
    // number of output parameters <= 1
    // Anything more complex than this would require custom approach
    const char* varName;
    GType       varType;
} UpnpAction;

typedef struct _UpnpAttributeInfo
{
    std::string name;
    const char* varName;
    GType type;
    // Action position in the vector correspond to GET/SET
    // TODO: include DELETE?
    std::vector <UpnpAction> actions;
} UpnpAttributeInfo;

typedef union _UpnpVar
{
    gboolean     var_boolean;
    guint        var_uint;
    gint         var_int;
    guint64      var_uint64;
    gint64       var_int64;
    const gchar* var_pchar;
} UpnpVar;

#define ERROR_PRINT(x) do { std::cerr << MODULE << ":" << __func__ << "(): ERROR: " << x << std::endl; } while (0)

#ifndef NDEBUG
#define DEBUG_PRINT(x) do { std::cout << MODULE << ":" << __func__ << "(): " << x << std::endl; } while (0)
#else
#define DEBUG_PRINT(x)
#endif
#endif
