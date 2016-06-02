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

#include "UpnpConnectionManagerService.h"

using namespace OIC::Service;

static const string MODULE = "UpnpConnectionManagerService";

// Connection Manager Service

// Organization:
// Attribute Name,
// State Variable Name (can be empty string), State variable type, "evented" flag
// Actions:
//    0: "GET" action name, action type, optional out parameters: var_name,var_type
//    1: "SET" action name, action type, optional in parameters: var_name,var_type
// Vector of embedded attributes (if present)
vector <UpnpAttributeInfo> UpnpConnectionManager::Attributes =
{
    {
        "protocolInfo",
        "", G_TYPE_NONE, false,
        {{"GetProtocolInfo", UPNP_ACTION_GET, "", G_TYPE_NONE}},
        {
            {"source", "Source", G_TYPE_STRING, false},
            {"sink", "Sink", G_TYPE_STRING, false}
        }
    },
    {
        "currentConnectionIds",
        "CurrentConnectionIDs", G_TYPE_STRING, true,
        {{"GetCurrentConnectionIDs", UPNP_ACTION_GET, "ConnectionIDs", G_TYPE_STRING}},
        {}
    },
    {
        "featureList",
        "FeatureList", G_TYPE_STRING, false,
        {{"GetFeatureList", UPNP_ACTION_GET, "FeatureList", G_TYPE_STRING}},
        {}
    },
    {
        "currentConnectionInfo",
        "", G_TYPE_NONE, false,
        {{"GetCurrentConnectionInfo", UPNP_ACTION_GET, "", G_TYPE_NONE}},
        {
            {"rcsId", "RcsID", G_TYPE_UINT, false},
            {"avTransportId", "AVTransportID", G_TYPE_UINT, false},
            {"protocolInfo", "ProtocolInfo", G_TYPE_STRING, false},
            {"peerConnectionManager", "PeerConnectionManager", G_TYPE_STRING, false},
            {"peerConnectionId", "PeerConnectionID", G_TYPE_UINT, false},
            {"direction", "Direction", G_TYPE_STRING, false},
            {"status", "Status", G_TYPE_STRING, false}
        }
    }
};

// Custom action map:
// "attribute name" -> GET request handlers
map <const string, UpnpConnectionManager::GetAttributeHandler>
UpnpConnectionManager::GetAttributeActionMap =
{
    {"protocolInfo", &UpnpConnectionManager::getProtocolInfo},
    {"currentConnectionInfo", &UpnpConnectionManager::getCurrentConnectionInfo}
};

// TODO Implement additional OCF attributes/UPnP Actions as necessary

void UpnpConnectionManager::getProtocolInfoCb(GUPnPServiceProxy *proxy,
        GUPnPServiceProxyAction *actionProxy,
        gpointer userData)
{
    GError *error = NULL;
    char *sourceProtocolInfo;
    char *sinkProtocolInfo;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                  actionProxy,
                  &error,
                  "Source",
                  G_TYPE_STRING,
                  &sourceProtocolInfo,
                  "Sink",
                  G_TYPE_STRING,
                  &sinkProtocolInfo,
                  NULL);
    if (error)
    {
        ERROR_PRINT("GetProtocolInfo failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes protocolInfo;

        DEBUG_PRINT("source=" << sourceProtocolInfo << ", sink=" << sinkProtocolInfo);
        protocolInfo["source"] = string(sourceProtocolInfo);
        protocolInfo["sink"] = string(sinkProtocolInfo);

        g_free(sourceProtocolInfo);
        g_free(sinkProtocolInfo);

        request->resource->setAttribute("protocolInfo", protocolInfo, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpConnectionManager::getProtocolInfo(UpnpRequest *request, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");
    GUPnPServiceProxyAction *actionProxy = gupnp_service_proxy_begin_action (m_proxy,
                                           "GetProtocolInfo",
                                           getProtocolInfoCb,
                                           (gpointer *) request,
                                           NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["protocolInfo"].first;
    return true;
}

void UpnpConnectionManager::getCurrentConnectionInfoCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *actionProxy, gpointer userData)
{
    GError *error = NULL;
    int rcsId;
    int avTransportId;
    char *protocolInfo;
    char *peerConnectionManager;
    int peerConnectionId;
    char *direction;
    char *connectionStatus;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "RcsID",
                                                  G_TYPE_UINT,
                                                  &rcsId,
                                                  "AVTransportID",
                                                  G_TYPE_UINT,
                                                  &avTransportId,
                                                  "ProtocolInfo",
                                                  G_TYPE_STRING,
                                                  &protocolInfo,
                                                  "PeerConnectionManager",
                                                  G_TYPE_STRING,
                                                  &peerConnectionManager,
                                                  "PeerConnectionID",
                                                  G_TYPE_UINT,
                                                  &peerConnectionId,
                                                  "Direction",
                                                  G_TYPE_STRING,
                                                  &direction,
                                                  "Status",
                                                  G_TYPE_STRING,
                                                  &connectionStatus,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetCurrentConnectionInfo failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes currentConnectionInfo;

        DEBUG_PRINT("Current Connection Info rcsId=" << rcsId);
        DEBUG_PRINT("Current Connection Info avTransportId=" << avTransportId);
        DEBUG_PRINT("Current Connection Info protocolInfo=" << protocolInfo);
        DEBUG_PRINT("Current Connection Info peerConnectionManager=" << peerConnectionManager);
        DEBUG_PRINT("Current Connection Info peerConnectionId=" << peerConnectionId);
        DEBUG_PRINT("Current Connection Info direction=" << direction);
        DEBUG_PRINT("Current Connection Info status=" << connectionStatus);

        currentConnectionInfo["rcsId"] = rcsId;
        currentConnectionInfo["avTransportId"] = avTransportId;
        currentConnectionInfo["protocolInfo"] = string(protocolInfo);
        currentConnectionInfo["peerConnectionManager"] = string(peerConnectionManager);
        currentConnectionInfo["peerConnectionId"] = peerConnectionId;
        currentConnectionInfo["direction"] = string(direction);
        currentConnectionInfo["status"] = string(connectionStatus);

        g_free(protocolInfo);
        g_free(peerConnectionManager);
        g_free(direction);
        g_free(connectionStatus);

        request->resource->setAttribute("currentConnectionInfo", currentConnectionInfo, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpConnectionManager::getCurrentConnectionInfo(UpnpRequest *request, const map< string, string > &queryParams)
{
    DEBUG_PRINT("");

    int connectionId = 0;

    if (! queryParams.empty()) {
        auto it = queryParams.find("connectionId");
        if (it != queryParams.end()) {
            try {
                connectionId = std::stoi(it->second);
                connectionId = std::max(0, connectionId);
                DEBUG_PRINT("getCurrentConnectionInfo queryParam " << it->first << "=" << it->second);
            } catch (const std::invalid_argument& ia) {
                ERROR_PRINT("Invalid getCurrentConnectionInfo queryParam " << it->first << "=" << it->second);
            }
        }
    }

    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "GetCurrentConnectionInfo",
                                          getCurrentConnectionInfoCb,
                                          (gpointer *) request,
                                          "ConnectionID",
                                          G_TYPE_UINT,
                                          connectionId,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["currentConnectionInfo"].first;
    return true;
}

bool UpnpConnectionManager::getAttributesRequest(UpnpRequest *request,
                                                 const map< string, string > &queryParams)
{
    bool status = false;

    for (auto it = m_attributeMap.begin(); it != m_attributeMap.end(); ++it)
    {
        DEBUG_PRINT(" \"" << it->first << "\"");
        // Check the request
        if (!UpnpAttribute::isValidRequest(&m_attributeMap, it->first, UPNP_ACTION_GET))
        {
            request->done++;
            continue;
        }

        UpnpAttributeInfo *attrInfo = it->second.first;
        bool result = false;

        // Check if custom GET needs to be called
        auto attr = this->GetAttributeActionMap.find(it->first);
        if (attr != this->GetAttributeActionMap.end())
        {
            GetAttributeHandler fp = attr->second;
            result = (this->*fp)(request, queryParams);
        }
        else if (string(attrInfo->actions[0].name) != "")
        {
            result = UpnpAttribute::get(m_proxy, request, attrInfo);
        }

        status |= result;
        if (!result)
        {
            request->done++;
        }
    }

    return status;
}

bool UpnpConnectionManager::setAttributesRequest(const RCSResourceAttributes &value,
                                                 UpnpRequest *request,
                                                 const map< string, string > &queryParams)
{
    bool status = false;

    for (auto it = value.begin(); it != value.end(); ++it)
    {
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
        bool result = UpnpAttribute::set(m_proxy, request, attrInfo, &attrValue);

        status |= result;
        if (!result)
        {
            request->done++;
        }
    }

    return status;
}
