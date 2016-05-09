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

// Root device
static const std::string UPNP_ROOT_DEVICE = "upnp:rootdevice";

// Search patterns for UPnP Devices and services matched by resource type
static std::map<std::string, std::string > UpnpSearchPatternMap =
{
// Lighting Control
    {UPNP_OIC_TYPE_DEVICE_LIGHT,            UPNP_PREFIX_DEVICE + ":.*(?:[Ll]ight).*"},
    {UPNP_OIC_TYPE_BRIGHTNESS,              UPNP_PREFIX_SERVICE + ":.*(?:[Dd]imming).*"},
    {UPNP_OIC_TYPE_POWER_SWITCH,            UPNP_PREFIX_SERVICE + ":.*(?:[Ss]witch[Pp]ower).*"},
// Internet Gateway
    {UPNP_OIC_TYPE_DEVICE_INET_GATEWAY,     UPNP_PREFIX_DEVICE + ":.*(?:[Ii]nternet[Gg]ateway[Dd]evice).*"},
    {UPNP_OIC_TYPE_DEVICE_WAN,              UPNP_PREFIX_DEVICE + ":.*(?:[Ww][Aa][Nn][Dd]evice).*"},
    {UPNP_OIC_TYPE_WAN_IF_CONFIG,           UPNP_PREFIX_SERVICE + ".*(?:[Ww][Aa][Nn][Cc]ommon[Ii]nterface[Cc]onfig).*"},
    {UPNP_OIC_TYPE_LAYER3_FORWARDING,       UPNP_PREFIX_SERVICE + ".*(?:[Ll]ayer3[Ff]orwarding).*"},
    {UPNP_OIC_TYPE_DEVICE_WAN_CONNECTION,   UPNP_PREFIX_DEVICE + ":.*(?:[Ww][Aa][Nn][Cc]onnection[Dd]evice).*"},
    {UPNP_OIC_TYPE_DEVICE_LAN,              UPNP_PREFIX_DEVICE + ":.*(?:[Ll][Aa][Nn][Dd]evice).*"},
// Media Control
    {UPNP_OIC_TYPE_DEVICE_MEDIA_RENDERER,   UPNP_PREFIX_DEVICE + ":.*(?:[Mm]edia[Rr]enderer).*"},
    {UPNP_OIC_TYPE_DEVICE_MEDIA_SERVER,     UPNP_PREFIX_DEVICE + ":.*(?:[Mm]edia[Ss]erver).*"},
    {UPNP_OIC_TYPE_CONNECTION_MANAGER,      UPNP_PREFIX_SERVICE + ":.*(?:[Cc]onnection[Mm]anager).*"},
    {UPNP_OIC_TYPE_AV_TRANSPORT,            UPNP_PREFIX_SERVICE + ":.*(?:[Aa][Vv][Tt]ransport).*"},
    {UPNP_OIC_TYPE_CONTENT_DIRECTORY,       UPNP_PREFIX_SERVICE + ":.*(?:[Cc]ontent[Dd]irectory).*"},
    {UPNP_OIC_TYPE_RENDERING_CONTROL,       UPNP_PREFIX_SERVICE + ":.*(?:[Rr]endering[Cc]ontrol).*"},
    {UPNP_OIC_TYPE_SCHEDULED_RECORDING,     UPNP_PREFIX_SERVICE + ":.*(?:[Ss]cheduled[Rr]ecording).*"}
};

// TODO: discuss if the following maps should be moved to external header file like UpnpConstants.h

// Interface map
// TODO: the mapping needs TBD and expanded
static std::map<std::string, std::string > UpnpInterfaceMap =
{
// Lighting Control
    {UPNP_OIC_TYPE_DEVICE_LIGHT,             OC_RSRVD_INTERFACE_LL},
    {UPNP_OIC_TYPE_DIMMING,                  "oic.if.a"},
    {UPNP_OIC_TYPE_POWER_SWITCH,             "oic.if.a"},
    {UPNP_OIC_TYPE_BRIGHTNESS,               "oic.if.a"},
// Internet Gateway
    {UPNP_OIC_TYPE_DEVICE_INET_GATEWAY,      OC_RSRVD_INTERFACE_LL},
    {UPNP_OIC_TYPE_DEVICE_WAN,               OC_RSRVD_INTERFACE_LL},
    {UPNP_OIC_TYPE_WAN_IF_CONFIG,            OC_RSRVD_INTERFACE_DEFAULT},
    {UPNP_OIC_TYPE_LAYER3_FORWARDING,        OC_RSRVD_INTERFACE_DEFAULT},
    {UPNP_OIC_TYPE_DEVICE_WAN_CONNECTION,    OC_RSRVD_INTERFACE_LL},
    {UPNP_OIC_TYPE_DEVICE_LAN,               OC_RSRVD_INTERFACE_LL},
// Media Control
    {UPNP_OIC_TYPE_DEVICE_MEDIA_RENDERER,    OC_RSRVD_INTERFACE_LL},
    {UPNP_OIC_TYPE_DEVICE_MEDIA_SERVER,      OC_RSRVD_INTERFACE_LL},
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
    {UPNP_OIC_TYPE_DEVICE_WAN,              UPNP_OIC_URI_PREFIX_WAN},
    {UPNP_OIC_TYPE_WAN_IF_CONFIG,           UPNP_OIC_URI_PREFIX_WAN_IF_CONFIG},
    {UPNP_OIC_TYPE_LAYER3_FORWARDING,       UPNP_OIC_URI_PREFIX_LAYER3_FORWARDING},
    {UPNP_OIC_TYPE_DEVICE_WAN_CONNECTION,   UPNP_OIC_URI_PREFIX_WAN_CONNECTION},
    {UPNP_OIC_TYPE_DEVICE_LAN,              UPNP_OIC_URI_PREFIX_LAN},
    {UPNP_OIC_TYPE_DEVICE_MEDIA_RENDERER,   UPNP_OIC_URI_PREFIX_MEDIA_RENDERER},
    {UPNP_OIC_TYPE_DEVICE_MEDIA_SERVER,     UPNP_OIC_URI_PREFIX_MEDIA_SERVER},
    {UPNP_OIC_TYPE_CONNECTION_MANAGER,      UPNP_OIC_URI_PREFIX_CONNECTION_MANAGER},
    {UPNP_OIC_TYPE_AV_TRANSPORT,            UPNP_OIC_URI_PREFIX_AV_TRANSPORT},
    {UPNP_OIC_TYPE_CONTENT_DIRECTORY,       UPNP_OIC_URI_PREFIX_CONTENT_DIRECTORY},
    {UPNP_OIC_TYPE_RENDERING_CONTROL,       UPNP_OIC_URI_PREFIX_RENDERING_CONTROL},
    {UPNP_OIC_TYPE_SCHEDULED_RECORDING,     UPNP_OIC_URI_PREFIX_SCHEDULED_RECORDING}
};

typedef std::vector<RCSResourceAttributes> CompositeAttribute;

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

typedef struct _EmbeddedAttribute
{
    std::string name;
    const char* varName;
    GType       type;
    bool        evented;
} EmbeddedAttribute;

typedef struct _UpnpAttributeInfo
{
    std::string name;      // OCF Attribute name
    const char* varName;   // UPnP state variale name
    GType       type;      // UPnP state variale type
    bool        evented;   // Flag to indicate evented state variable
    std::vector <UpnpAction> actions;      // UPnP Actions
    std::vector <EmbeddedAttribute> attrs; // Embedded atttributes (composite attribute case)
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
