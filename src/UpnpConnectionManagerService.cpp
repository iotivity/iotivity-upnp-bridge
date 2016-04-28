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

// Attribute map: "attribute name" -> GET/SET request handlers
map <const string, pair <UpnpConnectionManager::GetAttributeHandler, UpnpConnectionManager::SetAttributeHandler>> UpnpConnectionManager::AttributeMap =
{
    {"value", {&UpnpConnectionManager::getTarget, &UpnpConnectionManager::setTarget}}
};

void UpnpConnectionManager::getTargetCb(GUPnPServiceProxy *proxy,
                                  GUPnPServiceProxyAction *action,
                                  gpointer userData)
{
    GError *error = NULL;
    bool value;
    UpnpRequest *request = static_cast<UpnpRequest*> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  action,
                                                  &error,
                                                  "RetTargetValue",
                                                  G_TYPE_BOOLEAN,
                                                  &value,
                                                  NULL);
    if (error) {
        ERROR_PRINT("GetTarget failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    if (status)
    {
        DEBUG_PRINT("value "<< value);
        request->resource->setAttribute("value", value, false);
    }

    UpnpRequest::requestDone(request, status);
}

GUPnPServiceProxyAction* UpnpConnectionManager::getTarget(UpnpRequest *request)
{

    return gupnp_service_proxy_begin_action (m_proxy,
                                             "GetTarget",
                                             getTargetCb,
                                             (gpointer *) request,
                                             NULL);
}

void UpnpConnectionManager::setTargetCb(GUPnPServiceProxy *proxy,
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
        ERROR_PRINT("SetTarget failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    if (status)
    {
        std::map< GUPnPServiceProxyAction *, RCSResourceAttributes::Value >::iterator it = request->proxieMap.find(action);

        if (it != request->proxieMap.end())
        {
            bool value = it->second.get< bool >();
            DEBUG_PRINT("value "<< value);

            request->resource->setAttribute("value", value, false);
            request->proxieMap.erase(it);
        }
    }

    UpnpRequest::requestDone(request, status);
}

GUPnPServiceProxyAction * UpnpConnectionManager::setTarget(RCSResourceAttributes::Value& attrValue, UpnpRequest *request)
{

    bool value = attrValue.get< bool >();

    return gupnp_service_proxy_begin_action (m_proxy,
                                             "SetTarget",
                                             setTargetCb,
                                             (gpointer *) request,
                                             "newTargetValue",
                                             G_TYPE_BOOLEAN,
                                             value,
                                             NULL);
}

bool UpnpConnectionManager::getAttributesRequest(UpnpRequest *request)
{
    bool status = false;

    for (auto it = this->AttributeMap.begin(); it != this->AttributeMap.end(); ++it)
    {
        DEBUG_PRINT(" \"" << it->first << "\"");
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

bool UpnpConnectionManager::setAttributesRequest(const RCSResourceAttributes &value, UpnpRequest *request)
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
