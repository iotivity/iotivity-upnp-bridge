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

#include "UpnpWanCableLinkConfigService.h"

using namespace OIC::Service;

static const string MODULE = "WanCableLinkConfig";

// WAN Cable Link Config service

// Organization:
// Attribute Name,
// State Variable Name (can be empty string), State variable type, "evented" flag
// Actions:
//    0: "GET" action name, action type, optional out parameters: var_name,var_type
//    1: "SET" action name, action type, optional in parameters: var_name,var_type
// Vector of embedded attributes (if present)
vector <UpnpAttributeInfo> UpnpWanCableLinkConfig::Attributes =
{
    {
        "downFrequency",
        "DownstreamFrequency", G_TYPE_UINT, false,
        {   {"GetDownstreamFrequency", UPNP_ACTION_GET,  "NewDownstreamFrequency", G_TYPE_UINT}},
        {}
    },
    {
        "downModulation",
        "DownstreamModulation", G_TYPE_STRING, false,
        {   {"GetDownstreamModulation", UPNP_ACTION_GET,  "NewDownstreamModulation", G_TYPE_STRING}},
        {}
    },
    {
        "upFrequency",
        "UpstreamFrequency", G_TYPE_UINT, false,
        {   {"GetUpstreamFrequency", UPNP_ACTION_GET,  "NewUpstreamFrequency", G_TYPE_UINT}},
        {}
    },
    {
        "upModulation",
        "UpstreamModulation", G_TYPE_STRING, false,
        {   {"GetUpstreamModulation", UPNP_ACTION_GET,  "NewUpstreamModulation", G_TYPE_STRING}},
        {}
    },
    {
        "upChannelId",
        "UpstreamChannelID", G_TYPE_UINT, false,
        {   {"GetUpstreamChannelID", UPNP_ACTION_GET,  "NewUpstreamChannelID", G_TYPE_UINT}},
        {}
    },
    {
        "upPowerLevel",
        "UpstreamPowerLevel", G_TYPE_UINT, false,
        {   {"GetUpstreamPowerLevel", UPNP_ACTION_GET,  "NewUpstreamPowerLevel", G_TYPE_UINT}},
        {}
    },
    {
        "bpiEncrypt",
        "BPIEncryptionEnabled", G_TYPE_BOOLEAN, false,
        {   {"GetBPIEncryptionEnabled", UPNP_ACTION_GET,  "NewBPIEncryptionEnabled", G_TYPE_BOOLEAN}},
        {}
    },
    {
        "configFile",
        "ConfigFile", G_TYPE_STRING, false,
        {   {"GetConfigFile", UPNP_ACTION_GET,  "NewConfigFile", G_TYPE_STRING}},
        {}
    },
    {
        "tftp",
        "TFTPServer", G_TYPE_STRING, false,
        {   {"GetTFTPServer", UPNP_ACTION_GET,  "NewTFTPServer", G_TYPE_STRING}},
        {}
    },
    {
        "linkInfo",
        "", G_TYPE_NONE, false,
        {   {"GetCableLinkConfigInfo", UPNP_ACTION_GET,  NULL, G_TYPE_NONE}},
        {   {"linkType",     "LinkType",             G_TYPE_STRING, false},
            {"linkState",    "CableLinkConfigState", G_TYPE_STRING, true}
        }
    }
};

// Custom action map:
// "attribute name" -> GET request handlers
map <const string, UpnpWanCableLinkConfig::GetAttributeHandler>
    UpnpWanCableLinkConfig::GetAttributeActionMap =
{
    {"linkInfo", &UpnpWanCableLinkConfig::getLinkInfo},
};

void UpnpWanCableLinkConfig::getLinkInfoCb(GUPnPServiceProxy *proxy,
                                           GUPnPServiceProxyAction *actionProxy,
                                           gpointer userData)
{
    GError *error = NULL;
    char *linkType;
    char *linkState;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "NewCableLinkConfigState",
                                                  G_TYPE_STRING,
                                                  &linkState,
                                                  "NewLinkType",
                                                  G_TYPE_STRING,
                                                  &linkType,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetCableLinkConfigInfo failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes linkInfo;

        DEBUG_PRINT("linkState=" << linkState << ", linkType=" << linkType);
        linkInfo["linkState"] = string(linkState);
        linkInfo["linkType"] = string(linkType);
        request->resource->setAttribute("linkInfo", linkInfo, false);

        g_free(linkType);
        g_free(linkState);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpWanCableLinkConfig::getLinkInfo(UpnpRequest *request)
{
    DEBUG_PRINT("");
    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "GetCableLinkConfigInfo",
                                          getLinkInfoCb,
                                          (gpointer *) request,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    return true;
}

bool UpnpWanCableLinkConfig::getAttributesRequest(UpnpRequest *request,
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
            result = (this->*fp)(request);
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

bool UpnpWanCableLinkConfig::setAttributesRequest(const RCSResourceAttributes &value,
                                                  UpnpRequest *request,
                                                  const map< string, string > &queryParams)
{
    request->done = request->expected; // safeguard to allow
                                       // the request to finish cleanly
    // There is nothing to set, return error
    return false;
}
