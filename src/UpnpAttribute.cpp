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

#include "UpnpAttribute.h"

using namespace std;

// Service specific attribute initialization

// Power switch service
static vector <UpnpAttribute> UpnpPowerSwitchAttributes = {
    UpnpAttribute("value",
                  {{"GetTarget", UPNP_ACTION_GET}, {"SetTarget", UPNP_ACTION_POST}, {"GetStatus",UPNP_ACTION_GET}},
                  "Status", G_TYPE_BOOLEAN)
};

// Brightness service
    // TODO the following actions are not yet mapped: (note all unmapped actions are optional)
    //      SetOnEffectLevel
    //      SetOnEffect
    //      GetOnEffectParameters
    //      StepUp
    //      StepDown
    //      StartRampUp
    //      StartRampDown
    //      StopRamp
    //      StartRampToLevel
    //      SetStepDelta
    //      GetStepDelta
    //      SetRampRate
    //      GetRampRate
    //      PauseRamp
    //      ResumeRamp
    //      GetRampPaused
    //      GetRampTime
    //      GetIsRamping
static vector <UpnpAttribute> UpnpDimmingAttributes = {
    UpnpAttribute("brightness",
                  {{"GetLoadLevelTarget", UPNP_ACTION_GET}, {"SetLoadLevelTarget", UPNP_ACTION_POST}, {"GetLoadLevelStatus",UPNP_ACTION_GET}},
                  "LoadLevelStatus", G_TYPE_UINT)
};


static vector <UpnpAttribute> UpnpWanCommonInterfaceConfigAttributes = {
    UpnpAttribute("enabledForInternet",   {{"GetEnabledForInternet", UPNP_ACTION_GET}, {"SetEnabledForInternet", UPNP_ACTION_POST}},
                  "EnabledForInternet", G_TYPE_BOOLEAN),
    UpnpAttribute("accessType",           {{"GetCommonLinkProperties", UPNP_ACTION_GET}}),
    UpnpAttribute("upMaxBitRate",         {{"GetCommonLinkProperties", UPNP_ACTION_GET}}),
    UpnpAttribute("downMaxBitRate",       {{"GetCommonLinkProperties", UPNP_ACTION_GET}}),
    UpnpAttribute("physLinkStatus",       {{"GetCommonLinkProperties", UPNP_ACTION_GET}},
                  "PhysicalLinkStatus", G_TYPE_STRING),
    UpnpAttribute("wanAccesssProvider",   {{"GetWANAccessProvider", UPNP_ACTION_GET}}),
    UpnpAttribute("maxActiveConnections", {{"GetMaximumActiveConnections", UPNP_ACTION_GET}}),
    UpnpAttribute("totalBytesSent",       {{"GetTotalBytesSent", UPNP_ACTION_GET}}),
    UpnpAttribute("totalBytesReceived",   {{"GetTotalBytesRecieved", UPNP_ACTION_GET}}),
    UpnpAttribute("totalPacketsSent",     {{"GetTotalPacketsSent", UPNP_ACTION_GET}}),
    UpnpAttribute("totalPacketsReceived", {{"GetTotalPacketsRecieved", UPNP_ACTION_GET}}),
    //Special case: no matching UPNP function, but the attribute value
    //can be set based on observation
    UpnpAttribute("numActiveConnections",  {{"", UPNP_ACTION_GET}},
                  "NumberOfActiveConnections", G_TYPE_UINT),
    //"activeConnection" is a composite attribute with tags:
    //    "deviceContainer" (matches UpNP "ActiveConnectionDeviceContainer")" &
    //    "serviceID" (matches UPnP "ActiveConnectionServiceID")
    UpnpAttribute("activeConnection",     {{"GetActiveConnection", UPNP_ACTION_GET}})
};

// Resource type -> service attribute info
map <string, vector <UpnpAttribute>> UpnpAttributeMap =
{
    { UPNP_OIC_TYPE_BRIGHTNESS, UpnpDimmingAttributes },
    { UPNP_OIC_TYPE_POWER_SWITCH, UpnpPowerSwitchAttributes }
};

vector <UpnpAttribute> * UpnpAttribute::getServiceAttributeInfo(string resourceType)
{
    map<const string, vector<UpnpAttribute>>::iterator it = UpnpAttributeMap.find(resourceType);

    if (it == UpnpAttributeMap.end())
    {
        return nullptr;
    }

    return &(it->second);
}

UpnpAttribute * UpnpAttribute::getAttributeInfo(std::vector <UpnpAttribute> *serviceAttributes,
                                                std::string attrName)
{
    vector<UpnpAttribute>::iterator attr;
    for(attr = serviceAttributes->begin() ; attr != serviceAttributes->end() ; ++attr)
    {
        if (attr->name == attrName)
        {
            return &(*attr);
        }
    }
    return nullptr;
}

bool UpnpAttribute::isValidRequest(std::map <std::string, UpnpAttribute *> *attrMap,
                                   std::string attrName,
                                   UpnpActionType actionType)

{
    map  <std::string, UpnpAttribute*>::iterator attrInfo = attrMap->find(attrName);
    if (attrInfo == attrMap->end())
    {
        return false;
    }

    // Check that expected action for the attribute has been found
    if ((attrInfo->second->flags) & (int) actionType)
    {
        return true;
    }

    return false;
}
