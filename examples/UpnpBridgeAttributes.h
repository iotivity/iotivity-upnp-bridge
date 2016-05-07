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

#ifndef UPNP_BRIDGE_ATTRIBUTES_H
#define UPNP_BRIDGE_ATTRIBUTES_H

#include <UpnpConstants.h>

typedef enum
{
    ATTR_TYPE_BOOL    = 1,
    ATTR_TYPE_INT,
    ATTR_TYPE_INT64,
    ATTR_TYPE_STRING,
    ATTR_TYPE_VECTOR
} AttrType;

typedef struct _AttrDesc
{
    AttrType   type;
    bool       observable; // ignore, if this is a sub-attribute and the hosting
                           // attribute is observable set to "true"
    std::map <std::string, _AttrDesc> * composite;
} AttrDesc;

// Light

// BinarySwitch
std::map <std::string, AttrDesc> BinarySwitchAttrs =
{
    {"value", {ATTR_TYPE_BOOL, true, NULL}}
};

// Brightness
std::map <std::string, AttrDesc> BrightnessAttrs =
{
    {"brightness", {ATTR_TYPE_INT, true, NULL}}
};

// Networking

// WANInterfaceConfig

std::map <std::string, AttrDesc>WanIfConfigConnInfoAttrs =
{
    {"deviceContainer", {ATTR_TYPE_STRING, false, NULL}},
    {"serviceId",       {ATTR_TYPE_STRING, false, NULL}},
};

std::map <std::string, AttrDesc> WanIfConfigConnLinkPropertiesAttrs =
{
    {"accessType",     {ATTR_TYPE_STRING, false, NULL}},
    {"linkStatus",     {ATTR_TYPE_STRING, false, NULL}},
    {"upMaxBitrate",   {ATTR_TYPE_INT,    false, NULL}},
    {"downMaxBitrate", {ATTR_TYPE_INT,    false, NULL}}
};

std::map <std::string, AttrDesc> WanIfConfigAttrs =
{
    {"inetEnabled",        {ATTR_TYPE_BOOL,   true, NULL}},
    {"linkProperties",     {ATTR_TYPE_VECTOR, true, &WanIfConfigConnLinkPropertiesAttrs}},
    {"wanAccesssProvider", {ATTR_TYPE_STRING, false, NULL}},
    {"maxConnections",     {ATTR_TYPE_INT,    false, NULL}},
    {"bytesSent",          {ATTR_TYPE_INT,    false, NULL}},
    {"bytesReceived",      {ATTR_TYPE_INT,    false, NULL}},
    {"packetsSent",        {ATTR_TYPE_INT,    false, NULL}},
    {"packetsReceived",    {ATTR_TYPE_INT,    false, NULL}},
    {"numConnections",     {ATTR_TYPE_INT,    true, NULL}},
    {"connectionInfo",     {ATTR_TYPE_VECTOR, true, &WanIfConfigConnInfoAttrs}}
};

//Layer3Forwarding
std::map <std::string, AttrDesc> L3FAttrs =
{
    {"defaultConnectionService",  {ATTR_TYPE_STRING,   true, NULL}}
};

std::map <std::string, std::map <std::string, AttrDesc> & > ResourceAttrMap =
{
    {UPNP_OIC_TYPE_POWER_SWITCH,   BinarySwitchAttrs},
    {UPNP_OIC_TYPE_BRIGHTNESS ,    BrightnessAttrs},
    {UPNP_OIC_TYPE_WAN_IF_CONFIG,  WanIfConfigAttrs},
    {UPNP_OIC_TYPE_LAYER3_FORWARDING,  L3FAttrs},
};

#endif
