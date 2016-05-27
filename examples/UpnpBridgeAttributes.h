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
    bool       observable; // Indicates whether an attribute is
                           // observable. In case of a composite
                           // attribute, the flag should be set to "true",
                           // if any of the sub attributes are
                           // observalble.
    std::map <std::string, _AttrDesc> *composite;
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
    {"linkStatus",     {ATTR_TYPE_STRING, true,  NULL}},
    {"upMaxBitrate",   {ATTR_TYPE_INT,    false, NULL}},
    {"downMaxBitrate", {ATTR_TYPE_INT,    false, NULL}}
};

std::map <std::string, AttrDesc> WanIfConfigAttrs =
{
    {"inetEnabled",        {ATTR_TYPE_BOOL,   true,  NULL}},
    {"linkProperties",     {ATTR_TYPE_VECTOR, true,  &WanIfConfigConnLinkPropertiesAttrs}},
    {"wanAccesssProvider", {ATTR_TYPE_STRING, false, NULL}},
    {"maxConnections",     {ATTR_TYPE_INT,    false, NULL}},
    {"bytesSent",          {ATTR_TYPE_INT,    false, NULL}},
    {"bytesReceived",      {ATTR_TYPE_INT,    false, NULL}},
    {"packetsSent",        {ATTR_TYPE_INT,    false, NULL}},
    {"packetsReceived",    {ATTR_TYPE_INT,    false, NULL}},
    {"numConnections",     {ATTR_TYPE_INT,    true,  NULL}},
    {"connectionInfo",     {ATTR_TYPE_VECTOR, false, &WanIfConfigConnInfoAttrs}}
};

// Layer3Forwarding
std::map <std::string, AttrDesc> L3FAttrs =
{
    {"defaultConnectionService",  {ATTR_TYPE_STRING,   true, NULL}}
};

// WANEthernetLinkConfig
std::map <std::string, AttrDesc> WanEthConfigAttrs =
{
    {"linkStatus",  {ATTR_TYPE_STRING,   true, NULL}}
};

// LANHostConfigManagement
std::map <std::string, AttrDesc> LanHostConfigAddrRangeAttrs =
{
    {"minAddr",  {ATTR_TYPE_STRING, false, NULL}},
    {"maxAddr",  {ATTR_TYPE_STRING, false, NULL}}
};

std::map <std::string, AttrDesc> LanHostConfigAttrs =
{
    {"configurable", {ATTR_TYPE_BOOL,    false, NULL}},
    {"dhcpRelay",    {ATTR_TYPE_BOOL,    false, NULL}},
    {"subnetMask",   {ATTR_TYPE_STRING,  false, NULL}},
    {"domainName",   {ATTR_TYPE_STRING,  false, NULL}},
    {"addrRange",    {ATTR_TYPE_VECTOR,  false, &LanHostConfigAddrRangeAttrs}},
    {"dnsServers",   {ATTR_TYPE_STRING,  false, NULL}},
    {"reservedAddr", {ATTR_TYPE_STRING,  false, NULL}},
    {"ipRouters",    {ATTR_TYPE_STRING,  false, NULL}},
};

// WANIPConnection
std::map <std::string, AttrDesc> WanIpConnectionTypeInfoAttrs =
{
    {"type",         {ATTR_TYPE_STRING, false, NULL}},
    {"allTypes",     {ATTR_TYPE_STRING, true,  NULL}}
};

std::map <std::string, AttrDesc> WanIpConnectionNatStatusAttrs =
{
    {"rsip",        {ATTR_TYPE_BOOL, false,  NULL}},
    {"enabled",     {ATTR_TYPE_BOOL, false,  NULL}}
};

std::map <std::string, AttrDesc> WanIpConnectionStateAttrs =
{
    {"status",      {ATTR_TYPE_STRING, true, NULL}},
    {"lastError",   {ATTR_TYPE_STRING, false,  NULL}},
    {"uptime",      {ATTR_TYPE_INT, false,  NULL}},
};

std::map <std::string, AttrDesc> WanIpConnectionAttrs =
{
    {"autoDiscoTime",      {ATTR_TYPE_INT,    false, NULL}},
    {"idleDiscoTime",      {ATTR_TYPE_INT,    false, NULL}},
    {"warnDiscoTime",      {ATTR_TYPE_INT,    false, NULL}},
    {"externAddr",         {ATTR_TYPE_STRING, true,  NULL}},
    {"connectionTypeInfo", {ATTR_TYPE_VECTOR, true,  &WanIpConnectionTypeInfoAttrs}},
    {"nat",                {ATTR_TYPE_VECTOR, false, &WanIpConnectionNatStatusAttrs}},
    {"updateId",           {ATTR_TYPE_INT,    true,  NULL}},
    {"sizePortMap",        {ATTR_TYPE_INT,    true,  NULL}},
    {"connectionState",    {ATTR_TYPE_VECTOR, true,  &WanIpConnectionStateAttrs}},
};

// Media Server / Media Renderer

// AV Tranport
std::map <std::string, AttrDesc> AvTransportAttrs =
{
};

// Connection Manager
std::map <std::string, AttrDesc> ProtocolInfoAttrs =
{
    {"source", {ATTR_TYPE_STRING, false, NULL}},
    {"sink",   {ATTR_TYPE_STRING, false, NULL}}
};

std::map <std::string, AttrDesc> ConnectionManagerAttrs =
{
    {"protocolInfo",         {ATTR_TYPE_VECTOR, false, &ProtocolInfoAttrs}},
    {"currentConnectionIds", {ATTR_TYPE_STRING, false, NULL}},
    {"featureList",          {ATTR_TYPE_STRING, false, NULL}},
};

// Content Directory
std::map <std::string, AttrDesc> ContentDirectoryAttrs =
{
    {"searchCaps",     {ATTR_TYPE_STRING, false, NULL}},
    {"searchCaps",     {ATTR_TYPE_STRING, false, NULL}},
    {"systemUpdateId", {ATTR_TYPE_INT,    true,  NULL}},
    {"resetToken",     {ATTR_TYPE_STRING, false, NULL}},
    {"featureList",    {ATTR_TYPE_STRING, false, NULL}},
};

// Rendering Control
std::map <std::string, AttrDesc> RenderingControlAttrs =
{
    {"presetNameList", {ATTR_TYPE_STRING, false, NULL}},
    {"mute",           {ATTR_TYPE_BOOL,   false, NULL}},
    {"volume",         {ATTR_TYPE_INT,    false, NULL}},
};

// Scheduled Recording
std::map <std::string, AttrDesc> ScheduledRecordingAttrs =
{
    {"stateUpdateId",  {ATTR_TYPE_INT,    false, NULL}},
};

std::map <std::string, std::map <std::string, AttrDesc> & > ResourceAttrMap =
{
    //{UPNP_OIC_TYPE_POWER_SWITCH,         BinarySwitchAttrs},
    //{UPNP_OIC_TYPE_BRIGHTNESS ,          BrightnessAttrs},
    //{UPNP_OIC_TYPE_WAN_IF_CONFIG,        WanIfConfigAttrs},
    //{UPNP_OIC_TYPE_LAYER3_FORWARDING,    L3FAttrs},
    //{UPNP_OIC_TYPE_WAN_ETHERNET_CONFIG,  WanEthConfigAttrs},
    {UPNP_OIC_TYPE_WAN_IP_CONNECTION,    WanIpConnectionAttrs},
    //{UPNP_OIC_TYPE_LAN_HOST_CONFIG,      LanHostConfigAttrs},
    //{UPNP_OIC_TYPE_AV_TRANSPORT,         AvTransportAttrs},
    // {UPNP_OIC_TYPE_CONNECTION_MANAGER,   ConnectionManagerAttrs},
    //{UPNP_OIC_TYPE_CONTENT_DIRECTORY,    ContentDirectoryAttrs},
    //{UPNP_OIC_TYPE_RENDERING_CONTROL,    RenderingControlAttrs},
    //{UPNP_OIC_TYPE_SCHEDULED_RECORDING,  ScheduledRecordingAttrs},
};

#endif
