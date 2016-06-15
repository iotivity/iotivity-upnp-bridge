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

#include "UpnpDeviceProtectionService.h"

using namespace OIC::Service;

static const string MODULE = "DeviceProtection";

// Device Protection service

// Organization:
// Attribute Name,
// State Variable Name (can be empty string), State variable type, "evented" flag
// Actions:
//    0: "GET" action name, action type, optional out parameters: var_name,var_type
//    1: "SET" action name, action type, optional in parameters: var_name,var_type
// Vector of embedded attributes (if present)
vector <UpnpAttributeInfo> UpnpDeviceProtection::Attributes =
{
    // Special case: no matching UPNP action, but the attribute value
    // can be set based on observation
    {
        "setupReady",
        "SetupReady", G_TYPE_BOOLEAN, true,
        {{"", UPNP_ACTION_GET, "", G_TYPE_NONE}}, {}
    },
    {
        "protocolList",
        "SupportedProtocols", G_TYPE_STRING, false,
        {   {"GetSupportedProtocols", UPNP_ACTION_GET, "ProtocolList", G_TYPE_STRING}},
        {}
    },
        {
        "roleList",
        "", G_TYPE_NONE, false,
        {   {"GetAssignedRoles", UPNP_ACTION_GET, "RoleList", G_TYPE_STRING}},
        {}
    },
    {
        "setupMessage",
        "", G_TYPE_NONE, false,
        {   {"SendSetupMessage", UPNP_ACTION_GET, NULL, G_TYPE_NONE}},
        {   {"protocolType", "", G_TYPE_STRING, false},
            {"message",      "", G_TYPE_STRING, false}
        }
    },
    // TODO: Add support for optional UPnP Device Protection Service actions
};

// Custom action map:
// "attribute name" -> GET request handlers
map <const string, UpnpDeviceProtection::GetAttributeHandler>
UpnpDeviceProtection::GetAttributeActionMap =
{
    {"setupMessage", &UpnpDeviceProtection::sendSetupMessage}
};

typedef  struct _SendSetupMessageRequest
{
    UpnpRequest *request;
    string protocol;
} SendSetupMessageRequest;

void UpnpDeviceProtection::sendSetupMessageCb(GUPnPServiceProxy *proxy,
                                             GUPnPServiceProxyAction *actionProxy,
                                             gpointer userData)
{
    SendSetupMessageRequest *sendRequest = static_cast<SendSetupMessageRequest *> (userData);
    char *outMessage;
    GError *error = NULL;

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "OutMessage",
                                                  G_TYPE_STRING,
                                                  &outMessage,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("SendSetupMessage failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes setupMessage;

        DEBUG_PRINT("SetupMessage protocol=" << sendRequest->protocol);
        DEBUG_PRINT("SetupMessage message=" << outMessage);

        setupMessage["protocol"] = sendRequest->protocol;
        setupMessage["message"] = string(outMessage);

        g_free(outMessage);

        sendRequest->request->resource->setAttribute("setupMessage", setupMessage, false);
    }

    UpnpRequest::requestDone(sendRequest->request, status);
    delete sendRequest;

}

bool UpnpDeviceProtection::sendSetupMessage(UpnpRequest *request,
        const map< string, string > &queryParams)
{
    SendSetupMessageRequest *sendRequest;
    string inMessage;

    if (queryParams.empty())
    {
        return false;
    }

    auto it = queryParams.find("message");
    if (it != queryParams.end())
    {
        inMessage = it->second;
    }
    else
    {
        return false;
    }

    it = queryParams.find("protocol");
    if (it != queryParams.end())
    {
        sendRequest = new SendSetupMessageRequest;
        if (sendRequest == NULL)
        {
            return false;
        }
        sendRequest->protocol = it->second;
    }
    else
    {
        return false;
    }

    sendRequest->request = request;

        GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "SendSetupMessage",
                                          sendSetupMessageCb,
                                          (gpointer *) sendRequest,
                                          "ProtocolType",
                                          G_TYPE_STRING,
                                          (sendRequest->protocol).c_str(),
                                          "InMessage",
                                          G_TYPE_STRING,
                                          inMessage.c_str(),
                                          NULL);
    if (NULL == actionProxy)
    {
        delete sendRequest;
        return false;
    }

    request->proxyMap[actionProxy] = m_attributeMap["setupMessage"].first;
    return true;
}

bool UpnpDeviceProtection::getAttributesRequest(UpnpRequest *request,
        const map< string, string > &queryParams)
{
    bool status = false;

    for (auto it = m_attributeMap.begin(); it != m_attributeMap.end(); ++it)
    {
        bool result = false;
        UpnpAttributeInfo *attrInfo = it->second.first;

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

bool UpnpDeviceProtection::setAttributesRequest(const RCSResourceAttributes &value,
                                                       UpnpRequest *request,
                                                       const map< string, string > &queryParams)
{
    // TODO: add support for optional UPnP actions that map to POST request
    return true;
}
