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
static vector <UpnpAttributeInfo> UpnpPowerSwitchAttributes = {
    {"value", "Status", G_TYPE_BOOLEAN,
     {{"GetTarget", UPNP_ACTION_GET}, {"SetTarget", UPNP_ACTION_POST}, {"GetStatus",UPNP_ACTION_GET}},
    }
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
static vector <UpnpAttributeInfo> UpnpDimmingAttributes = {
    {"brightness", "LoadLevelStatus", G_TYPE_UINT,
     {{"GetLoadLevelTarget", UPNP_ACTION_GET}, {"SetLoadLevelTarget", UPNP_ACTION_POST}, {"GetLoadLevelStatus",UPNP_ACTION_GET}}
    }
};

#if 0
static vector <UpnpAttributeInfo> UpnpWanCommonInterfaceConfigAttributes = {
    UpnpAttribute("enabledForInternet",   {{"GetEnabledForInternet", UPNP_ACTION_GET}, {"SetEnabledForInternet", UPNP_ACTION_POST}},
                  "EnabledForInternet", G_TYPE_BOOLEAN),
    UpnpAttributeInfo("accessType",           {{"GetCommonLinkProperties", UPNP_ACTION_GET}}),
    UpnpAttributeInfo("upMaxBitRate",         {{"GetCommonLinkProperties", UPNP_ACTION_GET}}),
    UpnpAttributeInfo("downMaxBitRate",       {{"GetCommonLinkProperties", UPNP_ACTION_GET}}),
    UpnpAttributeInfo("physLinkStatus",       {{"GetCommonLinkProperties", UPNP_ACTION_GET}},
                      "PhysicalLinkStatus", G_TYPE_STRING),
    UpnpAttributeInfo("wanAccesssProvider",   {{"GetWANAccessProvider", UPNP_ACTION_GET}}),
    UpnpAttributeInfo("maxActiveConnections", {{"GetMaximumActiveConnections", UPNP_ACTION_GET}}),
    UpnpAttributeInfo("totalBytesSent",       {{"GetTotalBytesSent", UPNP_ACTION_GET}}),
    UpnpAttributeInfo("totalBytesReceived",   {{"GetTotalBytesRecieved", UPNP_ACTION_GET}}),
    UpnpAttributeInfo("totalPacketsSent",     {{"GetTotalPacketsSent", UPNP_ACTION_GET}}),
    UpnpAttributeInfo("totalPacketsReceived", {{"GetTotalPacketsRecieved", UPNP_ACTION_GET}}),
    //Special case: no matching UPNP function, but the attribute value
    //can be set based on observation
    UpnpAttributeInfo("numActiveConnections",  {{"", UPNP_ACTION_GET}},
                  "NumberOfActiveConnections", G_TYPE_UINT),
    //"activeConnection" is a composite attribute with tags:
    //    "deviceContainer" (matches UpNP "ActiveConnectionDeviceContainer")" &
    //    "serviceID" (matches UPnP "ActiveConnectionServiceID")
    UpnpAttributeInfo("activeConnection",     {{"GetActiveConnection", UPNP_ACTION_GET}})
};
#endif

// Resource type -> service attribute info
map <string, vector <UpnpAttributeInfo>> UpnpAttributeInfoMap =
{
    { UPNP_OIC_TYPE_BRIGHTNESS, UpnpDimmingAttributes },
    { UPNP_OIC_TYPE_POWER_SWITCH, UpnpPowerSwitchAttributes }
};

vector <UpnpAttributeInfo> * UpnpAttribute::getServiceAttributeInfo(string resourceType)
{
    map<const string, vector<UpnpAttributeInfo>>::iterator it = UpnpAttributeInfoMap.find(resourceType);

    if (it == UpnpAttributeInfoMap.end())
    {
        return nullptr;
    }

    return &(it->second);
}

UpnpAttributeInfo * UpnpAttribute::getAttributeInfo(std::vector <UpnpAttributeInfo> *serviceAttributes,
                                                    std::string attrName)
{
    vector<UpnpAttributeInfo>::iterator attrInfo;
    for(attrInfo = serviceAttributes->begin() ; attrInfo != serviceAttributes->end() ; ++attrInfo)
    {
        if (attrInfo->name == attrName)
        {
            return &(*attrInfo);
        }
    }
    return nullptr;
}

bool UpnpAttribute::isValidRequest(map <string, pair <UpnpAttributeInfo*, int>> *attrMap,
                                   string attrName,
                                   UpnpActionType actionType)

{
    // Check if the attribute is present
    map  <string, pair< UpnpAttributeInfo*, int>>::iterator attr = attrMap->find(attrName);
    if (attr == attrMap->end())
    {
        return false;
    }

    // Check that expected action for the attribute has been found
    if ((attr->second.second) & (int) actionType)
    {
        return true;
    }

    return false;
}
