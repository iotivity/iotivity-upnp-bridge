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

#include <map>
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
    const std::map <std::string, _AttrDesc> *composite;
} AttrDesc;

// Light

// BinarySwitch
const std::map <std::string, AttrDesc> BinarySwitchAttrs =
{
    {"value", {ATTR_TYPE_BOOL, true, NULL}}
};

// Brightness
const std::map <std::string, AttrDesc> BrightnessAttrs =
{
    {"brightness", {ATTR_TYPE_INT, true, NULL}}
};

// Networking

// WANInterfaceConfig

const std::map <std::string, AttrDesc>WanIfConfigConnInfoAttrs =
{
    {"deviceContainer", {ATTR_TYPE_STRING, false, NULL}},
    {"serviceId",       {ATTR_TYPE_STRING, false, NULL}},
};

const std::map <std::string, AttrDesc> WanIfConfigConnLinkPropertiesAttrs =
{
    {"accessType",     {ATTR_TYPE_STRING, false, NULL}},
    {"linkStatus",     {ATTR_TYPE_STRING, true,  NULL}},
    {"upMaxBitrate",   {ATTR_TYPE_INT,    false, NULL}},
    {"downMaxBitrate", {ATTR_TYPE_INT,    false, NULL}}
};

const std::map <std::string, AttrDesc> WanIfConfigAttrs =
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
const std::map <std::string, AttrDesc> L3FAttrs =
{
    {"defaultConnectionService",  {ATTR_TYPE_STRING,   true, NULL}}
};

// DeviceProtection
const std::map <std::string, AttrDesc> DeviceProtectionSetupMessageAttrs =
{
    {"protocol",  {ATTR_TYPE_STRING,   false, NULL}},
    {"message",   {ATTR_TYPE_STRING,   false, NULL}}
};

const std::map <std::string, AttrDesc> DeviceProtectionAttrs =
{
    {"setupMessage",      {ATTR_TYPE_VECTOR, false, &DeviceProtectionSetupMessageAttrs}},
    {"setupReady",        {ATTR_TYPE_BOOL,   true,  NULL}},
    {"protocolList",      {ATTR_TYPE_STRING, false, NULL}},
    {"roleList",          {ATTR_TYPE_STRING, false, NULL}},
};

// WANEthernetLinkConfig
const std::map <std::string, AttrDesc> WanEthConfigAttrs =
{
    {"linkStatus",  {ATTR_TYPE_STRING,   true, NULL}}
};

// WANCableLinkConfig
const std::map <std::string, AttrDesc> WanCableConfigLinkInfoAttrs =
{
    {"linkState",  {ATTR_TYPE_STRING,   false, NULL}},
    {"linkType",   {ATTR_TYPE_STRING,   false, NULL}}
};

const std::map <std::string, AttrDesc> WanCableConfigAttrs =
{
    {"downFrequency",      {ATTR_TYPE_INT,    false, NULL}},
    {"downModulation",     {ATTR_TYPE_STRING, false, NULL}},
    {"upFrequency",        {ATTR_TYPE_INT,    false, NULL}},
    {"upModulation",       {ATTR_TYPE_STRING, false, NULL}},
    {"upChannelId",        {ATTR_TYPE_INT,    false, NULL}},
    {"upPowerLevel",       {ATTR_TYPE_INT,    false, NULL}},
    {"bpiEncrypt",         {ATTR_TYPE_BOOL,   false, NULL}},
    {"configFile",         {ATTR_TYPE_STRING, false, NULL}},
    {"tftp",               {ATTR_TYPE_STRING, false, NULL}},
    {"linkInfo",           {ATTR_TYPE_VECTOR, false, &WanCableConfigLinkInfoAttrs}}
};

// WANDSLLinkConfig
const std::map <std::string, AttrDesc> WanDslConfigLinkInfoAttrs =
{
    {"linkType",     {ATTR_TYPE_STRING,   false, NULL}},
    {"linkStatus",   {ATTR_TYPE_STRING,   true, NULL}}
};

const std::map <std::string, AttrDesc> WanDslConfigAttrs =
{
    {"autoConfig",         {ATTR_TYPE_BOOL,    true,  NULL}},
    {"modulationType",     {ATTR_TYPE_STRING,  false, NULL}},
    {"destAddr",           {ATTR_TYPE_STRING,  false, NULL}},
    {"atmEncapsulation",   {ATTR_TYPE_STRING,  false, NULL}},
    {"fcs",                {ATTR_TYPE_BOOL,    false, NULL}},
    {"linkInfo",           {ATTR_TYPE_VECTOR,  true,  &WanDslConfigLinkInfoAttrs}}
};

// WANDSLLinkConfig
const std::map <std::string, AttrDesc> WanPotsConfigIspAttrs =
{
    {"phoneNumber",  {ATTR_TYPE_STRING,   false, NULL}},
    {"info",         {ATTR_TYPE_STRING,   false, NULL}},
    {"linkType",     {ATTR_TYPE_STRING,   false, NULL}}
};

const std::map <std::string, AttrDesc> WanPotsConfigCallRetryAttrs =
{
    {"numRetries",  {ATTR_TYPE_INT,   false, NULL}},
    {"interval",    {ATTR_TYPE_INT,   false, NULL}}
};

const std::map <std::string, AttrDesc> WanPotsConfigAttrs =
{
    {"fclass",             {ATTR_TYPE_STRING,  false, NULL}},
    {"dataModulation",     {ATTR_TYPE_STRING,  false, NULL}},
    {"dataProtocol",       {ATTR_TYPE_STRING,  false, NULL}},
    {"dataCmpression",     {ATTR_TYPE_STRING,  false, NULL}},
    {"vtrCommand",         {ATTR_TYPE_BOOL,    false, NULL}},
    {"isp",                {ATTR_TYPE_VECTOR,  false, &WanPotsConfigIspAttrs}},
    {"callRetry",          {ATTR_TYPE_VECTOR,  false, &WanPotsConfigCallRetryAttrs}},
};

// LANHostConfigManagement
const std::map <std::string, AttrDesc> LanHostConfigAddrRangeAttrs =
{
    {"minAddr",  {ATTR_TYPE_STRING, false, NULL}},
    {"maxAddr",  {ATTR_TYPE_STRING, false, NULL}}
};

const std::map <std::string, AttrDesc> LanHostConfigAttrs =
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
const std::map <std::string, AttrDesc> WanIpConnectionTypeInfoAttrs =
{
    {"type",         {ATTR_TYPE_STRING, false, NULL}},
    {"allTypes",     {ATTR_TYPE_STRING, true,  NULL}}
};

const std::map <std::string, AttrDesc> WanIpConnectionNatStatusAttrs =
{
    {"rsip",        {ATTR_TYPE_BOOL, false,  NULL}},
    {"enabled",     {ATTR_TYPE_BOOL, false,  NULL}}
};

const std::map <std::string, AttrDesc> WanIpConnectionStateAttrs =
{
    {"status",      {ATTR_TYPE_STRING, true, NULL}},
    {"lastError",   {ATTR_TYPE_STRING, false,  NULL}},
    {"uptime",      {ATTR_TYPE_INT, false,  NULL}},
};

const std::map <std::string, AttrDesc> WanIpConnectionAttrs =
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

// WANPPPConnection
const std::map <std::string, AttrDesc> WanPppConnectionMaxBitRateAttrs =
{
    {"up",        {ATTR_TYPE_INT, false,  NULL}},
    {"down",      {ATTR_TYPE_INT, false,  NULL}}
};

const std::map <std::string, AttrDesc> WanPppConnectionAttrs =
{
    {"autoDiscoTime",      {ATTR_TYPE_INT,    false, NULL}},
    {"idleDiscoTime",      {ATTR_TYPE_INT,    false, NULL}},
    {"warnDiscoTime",      {ATTR_TYPE_INT,    false, NULL}},
    {"externAddr",         {ATTR_TYPE_STRING, true,  NULL}},
    {"sizePortMap",        {ATTR_TYPE_INT,    true,  NULL}},
    {"user",               {ATTR_TYPE_STRING, false, NULL}},
    {"pwd",                {ATTR_TYPE_STRING, false, NULL}},
    {"maxBitRate",         {ATTR_TYPE_VECTOR, false, &WanPppConnectionMaxBitRateAttrs}},
    {"connectionTypeInfo", {ATTR_TYPE_VECTOR, true,  &WanIpConnectionTypeInfoAttrs}},
    {"nat",                {ATTR_TYPE_VECTOR, false, &WanIpConnectionNatStatusAttrs}},
    {"connectionState",    {ATTR_TYPE_VECTOR, true,  &WanIpConnectionStateAttrs}},
};

// Media Server / Media Renderer

// AV Transport
const std::map <std::string, AttrDesc> DeviceCapabilitiesAttrs =
{
    {"playMedia",       {ATTR_TYPE_STRING, false, NULL}},
    {"recMedia",        {ATTR_TYPE_STRING, false, NULL}},
    {"recQualityModes", {ATTR_TYPE_STRING, false, NULL}},
};

const std::map <std::string, AttrDesc> MediaInfoAttrs =
{
    {"nrTracks",           {ATTR_TYPE_INT,    false, NULL}},
    {"mediaDuration",      {ATTR_TYPE_STRING, false, NULL}},
    {"currentUri",         {ATTR_TYPE_STRING, false, NULL}},
    {"currentUriMetadata", {ATTR_TYPE_STRING, false, NULL}},
    {"nextUri",            {ATTR_TYPE_STRING, false, NULL}},
    {"nextUriMetadata",    {ATTR_TYPE_STRING, false, NULL}},
    {"playMedium",         {ATTR_TYPE_STRING, false, NULL}},
    {"recordMedium",       {ATTR_TYPE_STRING, false, NULL}},
    {"writeStatus",        {ATTR_TYPE_STRING, false, NULL}},
};

const std::map <std::string, AttrDesc> PositionInfoAttrs =
{
    {"track",         {ATTR_TYPE_INT,    false, NULL}},
    {"trackDuration", {ATTR_TYPE_STRING, false, NULL}},
    {"trackMetadata", {ATTR_TYPE_STRING, false, NULL}},
    {"trackUri",      {ATTR_TYPE_STRING, false, NULL}},
    {"relTime",       {ATTR_TYPE_STRING, false, NULL}},
    {"absTime",       {ATTR_TYPE_STRING, false, NULL}},
    {"relCount",      {ATTR_TYPE_INT,    false, NULL}},
    {"absCount",      {ATTR_TYPE_INT,    false, NULL}},
};

const std::map <std::string, AttrDesc> TransportInfoAttrs =
{
    {"transportState",  {ATTR_TYPE_STRING, false, NULL}},
    {"transportStatus", {ATTR_TYPE_STRING, false, NULL}},
    {"speed",           {ATTR_TYPE_STRING, false, NULL}},
};

const std::map <std::string, AttrDesc> TransportSettingsAttrs =
{
    {"playMode",       {ATTR_TYPE_STRING, false, NULL}},
    {"recQualityMode", {ATTR_TYPE_STRING, false, NULL}},
};

const std::map <std::string, AttrDesc> AvTransportAttrs =
{
    {"lastChange",              {ATTR_TYPE_STRING, true,  NULL}},
    {"currentTransportActions", {ATTR_TYPE_STRING, false, NULL}},
    {"deviceCapabilities",      {ATTR_TYPE_VECTOR, false, &DeviceCapabilitiesAttrs}},
    {"mediaInfo",               {ATTR_TYPE_VECTOR, false, &MediaInfoAttrs}},
    {"positionInfo",            {ATTR_TYPE_VECTOR, false, &PositionInfoAttrs}},
    {"transportInfo",           {ATTR_TYPE_VECTOR, false, &TransportInfoAttrs}},
    {"transportSettings",       {ATTR_TYPE_VECTOR, false, &TransportSettingsAttrs}},
};

// Connection Manager
const std::map <std::string, AttrDesc> ProtocolInfoAttrs =
{
    {"source", {ATTR_TYPE_STRING, false, NULL}},
    {"sink",   {ATTR_TYPE_STRING, false, NULL}}
};

const std::map <std::string, AttrDesc> CurrentConnectionInfoAttrs =
{
    {"rcsId",                 {ATTR_TYPE_INT,    false, NULL}},
    {"avTransportId",         {ATTR_TYPE_INT, false, NULL}},
    {"protocolInfo",          {ATTR_TYPE_STRING, false, NULL}},
    {"peerConnectionManager", {ATTR_TYPE_STRING, false, NULL}},
    {"peerConnectionId",      {ATTR_TYPE_INT, false, NULL}},
    {"direction",             {ATTR_TYPE_STRING, false, NULL}},
    {"status",                {ATTR_TYPE_STRING, false, NULL}},
};

const std::map <std::string, AttrDesc> ConnectionManagerAttrs =
{
    {"protocolInfo",          {ATTR_TYPE_VECTOR, false, &ProtocolInfoAttrs}},
    {"currentConnectionIds",  {ATTR_TYPE_STRING, false, NULL}},
    {"currentConnectionInfo", {ATTR_TYPE_VECTOR, false, &CurrentConnectionInfoAttrs}},
    {"featureList",           {ATTR_TYPE_STRING, false, NULL}},
};

// Content Directory
const std::map <std::string, AttrDesc> ContentDirectoryAttrs =
{
    {"searchCaps",     {ATTR_TYPE_STRING, false, NULL}},
    {"sortCaps",       {ATTR_TYPE_STRING, false, NULL}},
    {"systemUpdateId", {ATTR_TYPE_INT,    true,  NULL}},
    {"resetToken",     {ATTR_TYPE_STRING, false, NULL}},
    {"featureList",    {ATTR_TYPE_STRING, false, NULL}},
};

// Rendering Control
const std::map <std::string, AttrDesc> RenderingControlAttrs =
{
    {"lastChange",     {ATTR_TYPE_STRING, true,  NULL}},
    {"presetNameList", {ATTR_TYPE_STRING, false, NULL}},
    {"mute",           {ATTR_TYPE_BOOL,   false, NULL}},
    {"volume",         {ATTR_TYPE_INT,    false, NULL}},
};

// Scheduled Recording
const std::map <std::string, AttrDesc> ScheduledRecordingAttrs =
{
    {"stateUpdateId",  {ATTR_TYPE_INT,    false, NULL}},
};

const std::map <std::string, const std::map <std::string, AttrDesc> & > ResourceAttrMap =
{
    {UPNP_OIC_TYPE_POWER_SWITCH,           BinarySwitchAttrs},
    {UPNP_OIC_TYPE_BRIGHTNESS ,            BrightnessAttrs},
    {UPNP_OIC_TYPE_WAN_IF_CONFIG,          WanIfConfigAttrs},
    {UPNP_OIC_TYPE_LAYER3_FORWARDING,      L3FAttrs},
    {UPNP_OIC_TYPE_DEVICE_PROTECTION,      DeviceProtectionAttrs},
    {UPNP_OIC_TYPE_WAN_CABLE_LINK_CONFIG,  WanCableConfigAttrs},
    {UPNP_OIC_TYPE_WAN_DSL_LINK_CONFIG,    WanDslConfigAttrs},
    {UPNP_OIC_TYPE_WAN_ETHERNET_CONFIG,    WanEthConfigAttrs},
    {UPNP_OIC_TYPE_WAN_POTS_LINK_CONFIG,   WanPotsConfigAttrs},
    {UPNP_OIC_TYPE_WAN_IP_CONNECTION,      WanIpConnectionAttrs},
    {UPNP_OIC_TYPE_LAN_HOST_CONFIG,        LanHostConfigAttrs},
    {UPNP_OIC_TYPE_WAN_PPP_CONNECTION,     WanPppConnectionAttrs},
    {UPNP_OIC_TYPE_AV_TRANSPORT,           AvTransportAttrs},
    {UPNP_OIC_TYPE_CONNECTION_MANAGER,     ConnectionManagerAttrs},
    {UPNP_OIC_TYPE_CONTENT_DIRECTORY,      ContentDirectoryAttrs},
    {UPNP_OIC_TYPE_RENDERING_CONTROL,      RenderingControlAttrs},
    {UPNP_OIC_TYPE_SCHEDULED_RECORDING,    ScheduledRecordingAttrs},
};

#endif
