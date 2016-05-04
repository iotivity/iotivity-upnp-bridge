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

#include "UpnpWanCommonInterfaceConfigService.h"

using namespace OIC::Service;

static const string MODULE = "WanCommonInterfaceConfig";

// Wan Common Interface Config service

// Organization:
// Attribute Name,
// State Variable Name (can be empty string), State variable type, "evented" flag
// Actions:
//    0: "GET" action name, action type, optional out parameters: var_name,var_type
//    1: "SET" action name, action type, optional in parameters: var_name,var_type
// Vector of embedded attributes (if present)
vector <UpnpAttributeInfo> UpnpWanCommonInterfaceConfig::Attributes = {
    {"inetEnabled",
     "EnabledForInternet", G_TYPE_BOOLEAN, true,
     {{"GetEnabledForInternet", UPNP_ACTION_GET, "NewEnabledForInternet", G_TYPE_BOOLEAN},
      {"SetEnabledForInternet", UPNP_ACTION_POST, "NewEnabledForInternet", G_TYPE_BOOLEAN}},
     {}
    },
    //"linkProperties" is a composite attribute with tags:
    // "accessType", "upMaxBitRate", "downMaxBitRate", "linkStatus"
    {"linkProperties",
     "", G_TYPE_NONE, true,
     {{"GetCommonLinkProperties", UPNP_ACTION_GET, "", G_TYPE_NONE}},
     {
         {"accessType",     "WANAccessType", G_TYPE_STRING, false},
         {"upMaxBitrate",   "Layer1UpstreamMaxBitRate", G_TYPE_UINT, false},
         {"downMaxBitrate", "Layer1DownstreamMaxBitRate", G_TYPE_UINT, false},
         {"linkStatus",     "PhysicalLinkStatus", G_TYPE_STRING, true}
     }
    },
    {"wanAccesssProvider",
     "WANAccessProvider", G_TYPE_STRING, false,
     {{"GetWANAccessProvider", UPNP_ACTION_GET, "NewWANAccessProvider", G_TYPE_STRING}},
     {}
    },
    {"maxConnections",
     "MaximumActiveConnections", G_TYPE_STRING, false,
     {{"GetMaximumActiveConnections", UPNP_ACTION_GET, "NewMaximumActiveConnections", G_TYPE_UINT}},
     {}
    },
    {"bytesSent",
     "TotalBytesSent", G_TYPE_UINT, false,
     {{"GetTotalBytesSent", UPNP_ACTION_GET, "NewTotalBytesSent", G_TYPE_UINT}},
     {}
    },
    {"bytesReceived",
     "TotalBytesReceived", G_TYPE_UINT, false,
     {{"GetTotalBytesReceived", UPNP_ACTION_GET, "NewTotalBytesReceived", G_TYPE_UINT}},
     {}
    },
    {"packetsSent",
     "TotalPacketsSent", G_TYPE_UINT, false,
     {{"GetTotalPacketsSent", UPNP_ACTION_GET, "NewTotalPacketsSent", G_TYPE_UINT}},
     {}
    },
    {"packetsReceived",
     "TotalPacketsReceived", G_TYPE_UINT, false,
     {{"GetTotalPacketsReceived", UPNP_ACTION_GET, "NewTotalPacketsReceived", G_TYPE_UINT}},
     {}
    },
    //Special case: no matching UPNP action, but the attribute value
    //can be set based on observation
    {"numConnections",
     "NumberOfActiveConnections", G_TYPE_UINT, true,
     {{"", UPNP_ACTION_GET, "", G_TYPE_NONE}}, {}
    },
    //"connectionInfo" is a composite attribute with tags:
    //    "deviceContainer" (matches UpNP "ActiveConnectionDeviceContainer")" &
    //    "serviceID" (matches UPnP "ActiveConnectionServiceID")
    //{"connectionInfo",
    // "", G_TYPE_NONE, false
    //{{"GetActiveConnections", UPNP_ACTION_GET, NULL, G_TYPE_NONE}}, ""
    //}
};

// Custom action map:
// "attribute name" -> GET request handlers
map <const string, UpnpWanCommonInterfaceConfig::GetAttributeHandler> UpnpWanCommonInterfaceConfig::GetAttributeActionMap =
{
    {"linkProperties", &UpnpWanCommonInterfaceConfig::getLinkProperties},
    //{"connectionInfo", &UpnpWanCommonInterfaceConfig::getConnectionInfo}
};

void UpnpWanCommonInterfaceConfig::getLinkPropertiesCb(GUPnPServiceProxy *proxy,
                                                       GUPnPServiceProxyAction *action,
                                                       gpointer userData)
{
    GError *error = NULL;
    const char* accessType;
    int upBitrate;
    int downBitrate;
    const char* linkStatus;

    UpnpRequest *request = static_cast<UpnpRequest*> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  action,
                                                  &error,
                                                  "NewWANAccessType",
                                                  G_TYPE_STRING,
                                                  &accessType,
                                                  "NewLayer1UpstreamMaxBitRate",
                                                  G_TYPE_UINT,
                                                  &upBitrate,
                                                 "NewLayer1DownstreamMaxBitRate",
                                                  G_TYPE_UINT,
                                                  &downBitrate,
                                                  "NewPhysicalLinkStatus",
                                                  G_TYPE_STRING,
                                                  &linkStatus,
                                                  NULL);
    if (error) {
        ERROR_PRINT("GetCommonLinkProperties failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes properties;

        DEBUG_PRINT("accessType="<< accessType << ", upMaxBitrate=" << upBitrate <<
                    ", downMaxBitrate=" << downBitrate << ", linkStatus=" << linkStatus);
        properties["accessType"]     = accessType;
        properties["upMaxBitrate"]   = upBitrate;
        properties["downMaxBitrate"] = downBitrate;
        properties["linkStatus"]     = linkStatus;

        request->resource->setAttribute("linkProperties", properties, false);
    }

    UpnpRequest::requestDone(request, status);
}

GUPnPServiceProxyAction* UpnpWanCommonInterfaceConfig::getLinkProperties(UpnpRequest *request)
{
    DEBUG_PRINT("");
    return gupnp_service_proxy_begin_action (m_proxy,
                                             "GetCommonLinkProperties",
                                             getLinkPropertiesCb,
                                             (gpointer *) request,
                                             NULL);
}

bool UpnpWanCommonInterfaceConfig::getAttributesRequest(UpnpRequest *request)
{
    bool status = false;

    for (auto it = m_attributeMap.begin(); it != m_attributeMap.end(); ++it)
    {
        GUPnPServiceProxyAction *action;

        DEBUG_PRINT(" \"" << it->first << "\"");
        // Check the request
        if (!UpnpAttribute::isValidRequest(&m_attributeMap, it->first, UPNP_ACTION_GET))
        {
            request->done++;
            continue;
        }

        // Check if custom GET needs to be called
        auto attr = this->GetAttributeActionMap.find(it->first);
        if (attr != this->GetAttributeActionMap.end())
        {
            GetAttributeHandler fp = attr->second;
            action = (this->*fp)(request);
            status |= (action != NULL);
        }
        else {
            UpnpAttributeInfo *attrInfo = it->second.first;

            if (string(attrInfo->actions[0].name) == "")
            {
                // This attribute value is updated based on  notification.
                // There is no associated GET action.
                action = NULL;
            }
            else
            {
                UpnpAttributeInfo *attrInfo = it->second.first;
                action = UpnpAttribute::get(m_proxy, request, attrInfo);
                status |= (action != NULL);
            }
        }


        if (action == NULL)
        {
            request->done++;
        }
    }
    return status;
}

bool UpnpWanCommonInterfaceConfig::setAttributesRequest(const RCSResourceAttributes &value, UpnpRequest *request)
{
    bool status = false;

    for (auto it = value.begin(); it != value.end(); ++it)
    {
        GUPnPServiceProxyAction *action;
        const std::string attrName = it->key();

        DEBUG_PRINT(" \"" << attrName << "\"");

        // Check the request
        if (!UpnpAttribute::isValidRequest(&m_attributeMap, attrName, UPNP_ACTION_POST))
        {
            request->done++;
            continue;
        }
        RCSResourceAttributes::Value attrValue = it->value();

        UpnpAttributeInfo *attrInfo = m_attributeMap[attrName].first;
        action = UpnpAttribute::set(m_proxy, request, attrInfo, &attrValue);
        status |= (action != NULL);
        if (action == NULL)
        {
            request->done++;
        }
    }

    return status;
}
