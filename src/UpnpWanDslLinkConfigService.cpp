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

#include "UpnpWanDslLinkConfigService.h"

using namespace OIC::Service;

static const string MODULE = "WanDslLinkConfig";

// WAN Dsl Link Config service

// Organization:
// Attribute Name,
// State Variable Name (can be empty string), State variable type, "evented" flag
// Actions:
//    0: "GET" action name, action type, optional out parameters: var_name,var_type
//    1: "SET" action name, action type, optional in parameters: var_name,var_type
// Vector of embedded attributes (if present)
vector <UpnpAttributeInfo> UpnpWanDslLinkConfig::Attributes =
{
    {
        "autoConfig",
        "AutoConfig", G_TYPE_BOOLEAN, true,
        {   {"GetAutoConfig", UPNP_ACTION_GET,  "NewAutoConfig", G_TYPE_BOOLEAN}},
        {}
    },
    {
        "modulationType",
        "ModulationType", G_TYPE_STRING, false,
        {   {"GetModulationType", UPNP_ACTION_GET,  "NewModulationType", G_TYPE_STRING}},
        {}
    },
    {
        "destAddr",
        "DestinationAddress", G_TYPE_STRING, false,
        {   {"GetNewDestinationAddress", UPNP_ACTION_GET,  "NewDestinationAddress", G_TYPE_STRING},
            {"SetNewDestinationAddress", UPNP_ACTION_POST, "NewDestinationAddress", G_TYPE_STRING}},
        {}
    },
    {
        "atmEncapsulation",
        "ATMEncapsulation", G_TYPE_STRING, false,
        {   {"GetATMEncapsulation", UPNP_ACTION_GET,  "NewATMEncapsulation", G_TYPE_STRING},
            {"SetATMEncapsulation", UPNP_ACTION_POST, "NewATMEncapsulation", G_TYPE_STRING}},
        {}
    },
        {
        "fcs",
        "FCSPreserved", G_TYPE_BOOLEAN, false,
        {   {"GetFCSPreserved", UPNP_ACTION_GET,  "NewFCSPreserved", G_TYPE_BOOLEAN},
            {"SetFCSPreserved", UPNP_ACTION_POST, "NewFCSPreserved", G_TYPE_BOOLEAN}},
        {}
    },
    {
        "linkInfo",
        "", G_TYPE_NONE, false,
        {   {"GetDSLLinkInfo", UPNP_ACTION_GET,  NULL, G_TYPE_NONE},
            {"SetDSLLinkType", UPNP_ACTION_POST, NULL, G_TYPE_NONE}},
        {   {"linkType",     "LinkType",    G_TYPE_STRING, false},
            {"linkStatus",   "LinkStatus",  G_TYPE_STRING, true}
        }
    }
};

// Custom action map:
// "attribute name" -> GET request handlers
map <const string, UpnpWanDslLinkConfig::GetAttributeHandler>
    UpnpWanDslLinkConfig::GetAttributeActionMap =
{
    {"linkInfo", &UpnpWanDslLinkConfig::getLinkInfo},
};

// Custom action map:
// "attribute name" -> SET request handlers
map <const string, UpnpWanDslLinkConfig::SetAttributeHandler>
    UpnpWanDslLinkConfig::SetAttributeActionMap =
{
    {"linkInfo", &UpnpWanDslLinkConfig::setLinkInfo}
};

void UpnpWanDslLinkConfig::getLinkInfoCb(GUPnPServiceProxy *proxy,
                                           GUPnPServiceProxyAction *actionProxy,
                                           gpointer userData)
{
    GError *error = NULL;
    char *linkType;
    char *linkStatus;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "NewDSLLinkType",
                                                  G_TYPE_STRING,
                                                  &linkType,
                                                  "NewDSLLinkStatus",
                                                  G_TYPE_STRING,
                                                  &linkStatus,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetDSLLinkInfo failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes linkInfo;

        DEBUG_PRINT("linkStatus=" << linkStatus << ", linkType=" << linkType);
        linkInfo["linkStatus"] = string(linkStatus);
        linkInfo["linkType"] = string(linkType);
        request->resource->setAttribute("linkInfo", linkInfo, false);

        g_free(linkType);
        g_free(linkStatus);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpWanDslLinkConfig::getLinkInfo(UpnpRequest *request)
{
    DEBUG_PRINT("");
    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "GetDSLLinkInfo",
                                          getLinkInfoCb,
                                          (gpointer *) request,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    return true;
}

void UpnpWanDslLinkConfig::setLinkInfoCb(GUPnPServiceProxy *proxy,
                                         GUPnPServiceProxyAction *actionProxy,
                                         gpointer userData)
{
    GError *error = NULL;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("SetLinkType failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpWanDslLinkConfig::setLinkInfo(UpnpRequest *request,
                                       RCSResourceAttributes::Value *attrValue)
{
    DEBUG_PRINT("");
    GUPnPServiceProxyAction *actionProxy;
    const char *sNewType;
    const auto &attrs = attrValue->get< RCSResourceAttributes >();
    bool found = false;

    for (const auto &kvPair : attrs)
    {
        if (kvPair.key() == "linkType")
        {
            string sValue = (kvPair.value()). get < string> ();
            sNewType = sValue.c_str();
            DEBUG_PRINT("linkType: " << sNewType);
            found = true;
        }
    }

    if (!found)
    {
        ERROR_PRINT("Invalid attribute");
        return false;
    }

    actionProxy = gupnp_service_proxy_begin_action (m_proxy,
                                                    "SetLinkType",
                                                    setLinkInfoCb,
                                                    (gpointer *) request,
                                                    "NewLinkType",
                                                    G_TYPE_STRING,
                                                    sNewType,
                                                    NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy]  = m_attributeMap["connectionTypeInfo"].first;
    return true;
}

bool UpnpWanDslLinkConfig::getAttributesRequest(UpnpRequest *request,
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

bool UpnpWanDslLinkConfig::setAttributesRequest(const RCSResourceAttributes &value,
                                                UpnpRequest *request,
                                                const map< string, string > &queryParams)
{
    bool status = false;

    for (auto it = value.begin(); it != value.end(); ++it)
    {
        bool result = false;
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

        // Check if custom SET needs to be called
        auto attr = this->SetAttributeActionMap.find(attrName);
        if (attr != this->SetAttributeActionMap.end())
        {
            SetAttributeHandler fp = attr->second;
            result = (this->*fp)(request, &attrValue);
        }
        else
        {
            result = UpnpAttribute::set(m_proxy, request, attrInfo, &attrValue);
        }

        status |= result;
        if (!result)
        {
            request->done++;
        }
    }

    return status;
}
