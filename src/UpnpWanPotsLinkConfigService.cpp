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

#include "UpnpWanPotsLinkConfigService.h"

using namespace OIC::Service;

static const string MODULE = "WanPotsLinkConfig";

// WAN Pots Link Config service

// Organization:
// Attribute Name,
// State Variable Name (can be empty string), State variable type, "evented" flag
// Actions:
//    0: "GET" action name, action type, optional out parameters: var_name,var_type
//    1: "SET" action name, action type, optional in parameters: var_name,var_type
// Vector of embedded attributes (if present)
vector <UpnpAttributeInfo> UpnpWanPotsLinkConfig::Attributes =
{
    {
        "fclass",
        "Fclass", G_TYPE_STRING, false,
        {   {"GetFclass", UPNP_ACTION_GET, "NewFclass", G_TYPE_STRING}},
        {}
    },
    {
        "dataModulation",
        "DataModulationSupported", G_TYPE_STRING, false,
        {   {"GetDataModulationSupported", UPNP_ACTION_GET, "NewDataModulationSupported", G_TYPE_STRING}},
        {}
    },
    {
        "dataProtocol",
        "DataProtocol", G_TYPE_STRING, false,
        {   {"GetDataProtocol", UPNP_ACTION_GET, "NewDataProtocol", G_TYPE_STRING}},
        {}
    },
    {
        "dataCompression",
        "DataCompression", G_TYPE_STRING, false,
        {   {"GetDataCompression ", UPNP_ACTION_GET, "NewDataCompression ", G_TYPE_STRING}},
        {}
    },
        {
        "vtrCommand",
        "PlusVTRCommandSupported", G_TYPE_BOOLEAN, false,
        {   {"GetPlusVTRCommandSupported", UPNP_ACTION_GET, "NewPlusVTRCommandSupported", G_TYPE_BOOLEAN}},
        {}
    },
    {
        "isp",
        "", G_TYPE_NONE, false,
        {   {"GetISPInfo", UPNP_ACTION_GET,  NULL, G_TYPE_NONE},
            {"SetISPInfo", UPNP_ACTION_POST, NULL, G_TYPE_NONE}},
        {   {"phoneNumber",     "ISPPhoneNumber",  G_TYPE_STRING, false},
            {"info",            "ISPInfo",         G_TYPE_STRING, false},
            {"linkType",        "LinkType",        G_TYPE_STRING, false}}
    },
    {
        "callRetry",
        "", G_TYPE_NONE, false,
        {   {"GetCallRetryInfo", UPNP_ACTION_GET,  NULL, G_TYPE_NONE},
            {"SetCallRetryInfo", UPNP_ACTION_POST, NULL, G_TYPE_NONE}},
        {   {"numRetries",     "NumberOfRetries",     G_TYPE_INT, false},
            {"interval",       "DelayBetweenRetries", G_TYPE_INT, false}}
    }
};

// Custom action map:
// "attribute name" -> GET request handlers
map <const string, UpnpWanPotsLinkConfig::GetAttributeHandler>
    UpnpWanPotsLinkConfig::GetAttributeActionMap =
{
    {"isp",       &UpnpWanPotsLinkConfig::getIspInfo},
    {"callRetry", &UpnpWanPotsLinkConfig::getCallRetryInfo}
};

// Custom action map:
// "attribute name" -> SET request handlers
map <const string, UpnpWanPotsLinkConfig::SetAttributeHandler>
    UpnpWanPotsLinkConfig::SetAttributeActionMap =
{
    {"isp",       &UpnpWanPotsLinkConfig::setIspInfo},
    {"callRetry", &UpnpWanPotsLinkConfig::setCallRetryInfo}
};

void UpnpWanPotsLinkConfig::getIspInfoCb(GUPnPServiceProxy *proxy,
                                           GUPnPServiceProxyAction *actionProxy,
                                           gpointer userData)
{
    GError *error = NULL;
    char *linkType;
    char *info;
    char *phoneNumber;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "NewISPPhoneNumber",
                                                  G_TYPE_STRING,
                                                  &phoneNumber,
                                                  "NewISPInfo",
                                                  G_TYPE_STRING,
                                                  &info,
                                                  "NewLinkType",
                                                  G_TYPE_STRING,
                                                  &linkType,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetISPInfo failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes isp;

        DEBUG_PRINT("phoneNumber=" << phoneNumber << "ispInfo=" << info << ", linkType=" << linkType);
        isp["phoneNumber"] = string(phoneNumber);
        isp["info"] = string(info);
        isp["linkType"] = string(linkType);
        request->resource->setAttribute("isp", isp, false);

        g_free(linkType);
        g_free(info);
        g_free(phoneNumber);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpWanPotsLinkConfig::getIspInfo(UpnpRequest *request)
{
    DEBUG_PRINT("");
    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "GetISPInfo",
                                          getIspInfoCb,
                                          (gpointer *) request,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    return true;
}

void UpnpWanPotsLinkConfig::getCallRetryInfoCb(GUPnPServiceProxy *proxy,
                                               GUPnPServiceProxyAction *actionProxy,
                                               gpointer userData)
{
    GError *error = NULL;
    int numRetries;
    int interval;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "NewNumberOfRetries",
                                                  G_TYPE_INT,
                                                  &numRetries,
                                                  "NewDelayBetweenRetries",
                                                  G_TYPE_INT,
                                                  &interval,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetCallRetryInfo failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes callRetry;

        DEBUG_PRINT("numRetries=" << numRetries << "interval=" << interval);
        callRetry["numRetries"] = numRetries;
        callRetry["interval"] = interval;
        request->resource->setAttribute("callRetry", callRetry, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpWanPotsLinkConfig::getCallRetryInfo(UpnpRequest *request)
{
    DEBUG_PRINT("");
    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "GetCallRetryInfo",
                                          getCallRetryInfoCb,
                                          (gpointer *) request,
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    return true;
}

void UpnpWanPotsLinkConfig::setIspInfoCb(GUPnPServiceProxy *proxy,
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
        ERROR_PRINT("SetISPInfo failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpWanPotsLinkConfig::setIspInfo(UpnpRequest *request,
                                       RCSResourceAttributes::Value *attrValue)
{
    DEBUG_PRINT("");
    GUPnPServiceProxyAction *actionProxy;
    const char *sType;
    const char *sInfo;
    const char *sPhoneNumber;
    const auto &attrs = attrValue->get< RCSResourceAttributes >();
    int count = 3; // number of embedded attributes

    for (const auto &kvPair : attrs)
    {
        if (kvPair.key() == "linkType")
        {
            string sValue = (kvPair.value()).get<string>();
            sType = sValue.c_str();
            DEBUG_PRINT("linkType: " << sType);
            count--;
        } else if (kvPair.key() == "info")
        {
            string sValue = (kvPair.value()).get<string>();
            sInfo = sValue.c_str();
            DEBUG_PRINT("info: " << sInfo);
            count--;
        } else if (kvPair.key() == "phoneNumber")
        {
            string sValue = (kvPair.value()).get<string>();
            sPhoneNumber = sValue.c_str();
            DEBUG_PRINT("phoneNumber: " << sInfo);
            count--;
        }
    }

    if (count != 0)
    {
        ERROR_PRINT("Invalid attribute");
        return false;
    }

    actionProxy = gupnp_service_proxy_begin_action (m_proxy,
                                                    "SetISPInfo",
                                                    setIspInfoCb,
                                                    (gpointer *) request,
                                                    "NewISPPhoneNumber",
                                                    G_TYPE_STRING,
                                                    sPhoneNumber,
                                                    "NewISPInfo",
                                                    G_TYPE_STRING,
                                                    sInfo,
                                                    "NewLinkType",
                                                    G_TYPE_STRING,
                                                    sType,
                                                    NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy]  = m_attributeMap["isp"].first;
    return true;
}

void UpnpWanPotsLinkConfig::setCallRetryInfoCb(GUPnPServiceProxy *proxy,
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
        ERROR_PRINT("SeCallRetryInfo failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpWanPotsLinkConfig::setCallRetryInfo(UpnpRequest *request,
                                             RCSResourceAttributes::Value *attrValue)
{
    DEBUG_PRINT("");
    GUPnPServiceProxyAction *actionProxy;
    int numRetries;
    int interval;
    const auto &attrs = attrValue->get< RCSResourceAttributes >();
    int count = 2; // number of embedded attributes

    for (const auto &kvPair : attrs)
    {
        if (kvPair.key() == "interval")
        {
            interval = (kvPair.value()).get<int>();
            DEBUG_PRINT("interval: " << interval);
            count--;
        } else if (kvPair.key() == "numRetries")
        {
            numRetries = (kvPair.value()).get<int>();
            DEBUG_PRINT("numRetries: " << numRetries);
            count--;
        }
    }

    if (count != 0)
    {
        ERROR_PRINT("Invalid attribute");
        return false;
    }

    actionProxy = gupnp_service_proxy_begin_action (m_proxy,
                                                    "SetCallRetryInfo",
                                                    setCallRetryInfoCb,
                                                    (gpointer *) request,
                                                    "NewNumberOfRetries",
                                                    G_TYPE_INT,
                                                    numRetries,
                                                    "NewDelayBetweenRetries",
                                                    G_TYPE_INT,
                                                    interval,
                                                    NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy]  = m_attributeMap["callRetry"].first;
    return true;
}

bool UpnpWanPotsLinkConfig::getAttributesRequest(UpnpRequest *request,
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

bool UpnpWanPotsLinkConfig::setAttributesRequest(const RCSResourceAttributes &value,
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
