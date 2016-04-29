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
//    {"value", {&UpnpConnectionManager::getTarget, &UpnpConnectionManager::setTarget}}
};

// TODO Implement various OCF attributes/UPnP Actions

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
