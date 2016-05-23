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

#include "UpnpContentDirectoryService.h"

using namespace OIC::Service;

static const string MODULE = "UpnpContentDirectoryService";

// Content Directory Service

// Organization:
// Attribute Name,
// State Variable Name (can be empty string), State variable type, "evented" flag
// Actions:
//    0: "GET" action name, action type, optional out parameters: var_name,var_type
//    1: "SET" action name, action type, optional in parameters: var_name,var_type
// Vector of embedded attributes (if present)
vector <UpnpAttributeInfo> UpnpContentDirectory::Attributes =
{
    {
        "browseResult",
        "", G_TYPE_NONE, false,
        {{"Browse", UPNP_ACTION_GET, "", G_TYPE_NONE}},
        {
            {"result", "Result", G_TYPE_STRING, false},
            {"numberReturned", "NumberReturned", G_TYPE_UINT, false},
            {"totalMatches", "TotalMatches", G_TYPE_UINT, false},
            {"updateId", "UpdateID", G_TYPE_UINT, false}
        }
    },
    {
        "searchResult",
        "", G_TYPE_NONE, false,
        {{"Search", UPNP_ACTION_GET, "", G_TYPE_NONE}},
        {
            {"result", "Result", G_TYPE_STRING, false},
            {"numberReturned", "NumberReturned", G_TYPE_UINT, false},
            {"totalMatches", "TotalMatches", G_TYPE_UINT, false},
            {"updateId", "UpdateID", G_TYPE_UINT, false}
        }
    },
    {
        "searchCaps",
        "SearchCapabilities", G_TYPE_STRING, false,
        {{"GetSearchCapabilities", UPNP_ACTION_GET, "SearchCaps", G_TYPE_STRING}},
        {}
    },
    {
        "sortCaps",
        "SortCapabilities", G_TYPE_STRING, false,
        {{"GetSortCapabilities", UPNP_ACTION_GET, "SortCaps", G_TYPE_STRING}},
        {}
    },
    {
        "systemUpdateId",
        "SystemUpdateID", G_TYPE_UINT, true,
        {{"GetSystemUpdateID", UPNP_ACTION_GET, "Id", G_TYPE_UINT}},
        {}
    },
    {
        "resetToken",
        "ServiceResetToken", G_TYPE_STRING, false,
        {{"GetServiceResetToken", UPNP_ACTION_GET, "ResetToken", G_TYPE_STRING}},
        {}
    },
    {
        "featureList",
        "FeatureList", G_TYPE_STRING, false,
        {{"GetFeatureList", UPNP_ACTION_GET, "FeatureList", G_TYPE_STRING}},
        {}
    }
};

// Custom action map:
// "attribute name" -> GET request handlers
map <const string, UpnpContentDirectory::GetAttributeHandler>
UpnpContentDirectory::GetAttributeActionMap =
{
    {"browseResult", &UpnpContentDirectory::getBrowseResult},
    {"searchResult", &UpnpContentDirectory::getSearchResult}
};

// TODO Implement optional actions as necessary

// TODO Browse and Search are not complete, to be functional there will need
// to be a fix for IOT-971 item 6 (query params in resource container)
// or some workaround.  Also, the didl-lite xml that is returned must
// be sent to iotivity in some other form.

void UpnpContentDirectory::getBrowseResultCb(GUPnPServiceProxy *proxy,
        GUPnPServiceProxyAction *actionProxy,
        gpointer userData)
{
    GError *error = NULL;
    const char *result;
    int numberReturned;
    int totalMatches;
    int updateId;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "Result",
                                                  G_TYPE_STRING,
                                                  &result,
                                                  "NumberReturned",
                                                  G_TYPE_UINT,
                                                  &numberReturned,
                                                  "TotalMatches",
                                                  G_TYPE_UINT,
                                                  &totalMatches,
                                                  "UpdateID",
                                                  G_TYPE_UINT,
                                                  &updateId,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetBrowseResult failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes browseResult;

        DEBUG_PRINT("browse result=" << result);
        browseResult["result"] = string(result);
        browseResult["numberReturned"] = numberReturned;
        browseResult["totalMatches"] = totalMatches;
        browseResult["updateId"] = updateId;

        request->resource->setAttribute("browseResult", browseResult, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpContentDirectory::getBrowseResult(UpnpRequest *request)
{
    DEBUG_PRINT("");
    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "Browse",
                                          getBrowseResultCb,
                                          (gpointer *) request,
                                          "ObjectID",
                                          G_TYPE_STRING,
                                          "0",
                                          "BrowseFlag",
                                          G_TYPE_STRING,
                                          "BrowseDirectChildren",
                                          "Filter",
                                          G_TYPE_STRING,
                                          "*",
                                          "StartingIndex",
                                          G_TYPE_UINT,
                                          (unsigned int)0,
                                          "RequestedCount",
                                          G_TYPE_UINT,
                                          (unsigned int)0,
                                          "SortCriteria",
                                          G_TYPE_STRING,
                                          "",
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy].first = m_attributeMap["browseResult"].first;
    return true;
}

void UpnpContentDirectory::getSearchResultCb(GUPnPServiceProxy *proxy,
        GUPnPServiceProxyAction *actionProxy,
        gpointer userData)
{
    GError *error = NULL;
    const char *result;
    int numberReturned;
    int totalMatches;
    int updateId;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "Result",
                                                  G_TYPE_STRING,
                                                  &result,
                                                  "NumberReturned",
                                                  G_TYPE_UINT,
                                                  &numberReturned,
                                                  "TotalMatches",
                                                  G_TYPE_UINT,
                                                  &totalMatches,
                                                  "UpdateID",
                                                  G_TYPE_UINT,
                                                  &updateId,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetSearchResult failed: " << error->code << ", " << error->message);
        g_error_free(error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes searchResult;

        DEBUG_PRINT("search result=" << result);
        searchResult["result"] = string(result);
        searchResult["numberReturned"] = numberReturned;
        searchResult["totalMatches"] = totalMatches;
        searchResult["updateId"] = updateId;

        request->resource->setAttribute("searchResult", searchResult, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpContentDirectory::getSearchResult(UpnpRequest *request)
{
    DEBUG_PRINT("");
    GUPnPServiceProxyAction *actionProxy =
        gupnp_service_proxy_begin_action (m_proxy,
                                          "Search",
                                          getSearchResultCb,
                                          (gpointer *) request,
                                          "ContainerID",
                                          G_TYPE_STRING,
                                          "0",
                                          "SearchCriteria",
                                          G_TYPE_STRING,
                                          "*",
                                          "Filter",
                                          G_TYPE_STRING,
                                          "*",
                                          "StartingIndex",
                                          G_TYPE_UINT,
                                          (unsigned int)0,
                                          "RequestedCount",
                                          G_TYPE_UINT,
                                          (unsigned int)10, // limit to 10
                                          "SortCriteria",
                                          G_TYPE_STRING,
                                          "",
                                          NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy].first = m_attributeMap["searchResult"].first;
    return true;
}

bool UpnpContentDirectory::getAttributesRequest(UpnpRequest *request,
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

bool UpnpContentDirectory::setAttributesRequest(const RCSResourceAttributes &value,
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
