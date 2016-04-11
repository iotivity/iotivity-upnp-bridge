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

#include "UpnpDimmingService.h"

static const string MODULE = "UpnpDimmingService";

// Attribute map: "attribute name" -> GET/SET request handlers
map <const string, pair <UpnpDimming::GetAttributeHandler, UpnpDimming::SetAttributeHandler>> UpnpDimming::AttributeMap =
{
    {"brightness", {&UpnpDimming::getLoadLevel, &UpnpDimming::setLoadLevel}}
};

GUPnPServiceProxyAction* UpnpDimming::getLoadLevel(UpnpRequest *request)
{
    DEBUG_PRINT("");

    return gupnp_service_proxy_begin_action (m_proxy,
                                             "GetLoadLevelStatus",
                                             getLoadLevelCb,
                                             (gpointer *) request,
                                             NULL);
}

void UpnpDimming::getLoadLevelCb(GUPnPServiceProxy *proxy,
                                 GUPnPServiceProxyAction *action,
                                 gpointer userData)
{
    GError *error = NULL;
    int value;
    UpnpRequest *request = static_cast<UpnpRequest*> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  action,
                                                  &error,
                                                  "retLoadlevelStatus",
                                                  G_TYPE_UINT,
                                                  &value,
                                                  NULL);

    if (error) {
        ERROR_PRINT("GetLoadLevel failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    if (status)
    {
        DEBUG_PRINT("brightness "<< value);
        request->resource->setAttribute("brightness", value, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpDimming::getAttributesRequest(UpnpRequest *request)
{
    bool status = false;
    for (auto it = this->AttributeMap.begin(); it != this->AttributeMap.end(); ++it)
    {
        // Check the request
        if (!UpnpAttribute::isValidRequest(&m_attributeMap, it->first, UPNP_ACTION_GET))
        {
            request->done++;
            continue;
        }

        GetAttributeHandler fp = it->second.first;
        GUPnPServiceProxyAction *action = (this->*fp)(request);
        status |= (action != NULL);
        if (action == NULL)
        {
            request->done++;
        }
    }
    return status;
}

bool UpnpDimming::setAttributesRequest(const RCSResourceAttributes &value, UpnpRequest *request)
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

        SetAttributeHandler fp = this->AttributeMap[attrName].second;
        GUPnPServiceProxyAction *action = (this->*fp)(attrValue, request);
        status |= (action != NULL);
        if (action == NULL)
        {
            request->done++;
        }
        else
        {
            // Save the proxy handle and value to be used in callback
            request->proxieMap[action] = attrValue;
        }
    }

    return status;
}

GUPnPServiceProxyAction * UpnpDimming::setLoadLevel(RCSResourceAttributes::Value& attrValue, UpnpRequest *request)
{

    int value = attrValue.get< int >();

    return gupnp_service_proxy_begin_action (m_proxy,
                                             "SetLoadLevelTarget",
                                             setLoadLevelCb,
                                             (gpointer *) request,
                                             "newLoadlevelTarget",
                                             G_TYPE_UINT,
                                             value,
                                             NULL);
}

void UpnpDimming::setLoadLevelCb(GUPnPServiceProxy *proxy,
                                 GUPnPServiceProxyAction *action,
                                 gpointer userData)
{
    GError *error = NULL;
    UpnpRequest *request = static_cast<UpnpRequest*> (userData);
    bool status = gupnp_service_proxy_end_action (proxy,
                                                  action,
                                                  &error,
                                                  NULL);
    if (error) {
        ERROR_PRINT("SetLoadLevel failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    if (status)
    {
        std::map< GUPnPServiceProxyAction *, RCSResourceAttributes::Value >::iterator it = request->proxieMap.find(action);

        if (it != request->proxieMap.end())
        {
            int value = it->second.get< int >();
            DEBUG_PRINT("brightness "<< value);

            request->resource->setAttribute("brightness", value, false);
            request->proxieMap.erase(it);
        }
    }

    UpnpRequest::requestDone(request, status);
}
