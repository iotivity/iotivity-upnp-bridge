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

#include "UpnpLanHostConfigManagementService.h"

using namespace OIC::Service;

static const string MODULE = "LanHostConfigManagement";

// LAN Ethernet Host Config Management service

// Organization:
// Attribute Name,
// State Variable Name (can be empty string), State variable type, "evented" flag
// Actions:
//    0: "GET" action name, action type, optional out parameters: var_name,var_type
//    1: "SET" action name, action type, optional in parameters: var_name,var_type
// Vector of embedded attributes (if present)
vector <UpnpAttributeInfo> UpnpLanHostConfigManagement::Attributes =
{
    {
        "configurable",
        "DHCPServerConfigurable", G_TYPE_BOOLEAN, false,
        {   {"GetDHCPServerConfigurable", UPNP_ACTION_GET, "NewDHCPServerConfigurable", G_TYPE_BOOLEAN},
            {"SetDHCPServerConfigurable", UPNP_ACTION_POST, "NewDHCPServerConfigurable", G_TYPE_BOOLEAN}
        },
        {}
    },
    {
        "dhcpRelay",
        "DHCPRelay", G_TYPE_BOOLEAN, false,
        {   {"GetDHCPRelay", UPNP_ACTION_GET, "NewDHCPRelay", G_TYPE_BOOLEAN},
            {"SetDHCPRelay", UPNP_ACTION_POST, "NewDHCPRelay", G_TYPE_BOOLEAN}
        },
        {}
    },
    {
        "subnetMask",
        "SubnetMask", G_TYPE_STRING, false,
        {   {"GetSubnetMask", UPNP_ACTION_GET, "NewSubnetMask", G_TYPE_STRING},
            {"SetSubnetMask", UPNP_ACTION_POST, "NewSubnetMask", G_TYPE_STRING}
        },
        {}
    },
    {
        "domainName",
        "DomainNAme", G_TYPE_STRING, false,
        {   {"GetDomainName", UPNP_ACTION_GET, "NewDomainName", G_TYPE_STRING},
            {"SetDomainName", UPNP_ACTION_POST, "NewDomainName", G_TYPE_STRING}
        },
        {}
    },
    {
        "addrRange",
        "", G_TYPE_NONE, false,
        {   {"GetAddressRange", UPNP_ACTION_GET, NULL, G_TYPE_NONE},
            {"SetAddressRange", UPNP_ACTION_GET, NULL, G_TYPE_NONE}
        },
        {   {"minAddr", "MinAddress", G_TYPE_STRING, false},
            {"maxAddr", "MaxAddress", G_TYPE_STRING, false}
        }
    },
    {
        "dnsServers",
        "DNSServers", G_TYPE_STRING, false,
        {   {"GetDNSServers", UPNP_ACTION_GET, "NewDNSServers", G_TYPE_STRING},
            {"SetDNSServer", UPNP_ACTION_POST, "NewDNSServers", G_TYPE_STRING}
        },
        {}
    },
    {
        "reservedAddr",
        "ReservedAddresses", G_TYPE_STRING, false,
        {   {"GetReservedAddressess", UPNP_ACTION_GET, "NewReservedAddresses", G_TYPE_STRING},
            {"SetReservedAddress",   UPNP_ACTION_POST, "NewReservedAddresses", G_TYPE_STRING}
        },
        {}
    },
    {
        "ipRouters",
        "IPRouters", G_TYPE_STRING, false,
        {   {"GetIPRoutersList", UPNP_ACTION_GET, "NewIPRouters", G_TYPE_STRING},
            {"SetIPRouter",     UPNP_ACTION_POST, "NewIPRouters", G_TYPE_STRING}
        },
        {}
    },
};

// Custom action map:
// "attribute name" -> GET request handlers
map <const string, UpnpLanHostConfigManagement::GetAttributeHandler>
UpnpLanHostConfigManagement::GetAttributeActionMap =
{
    {"addrRange", &UpnpLanHostConfigManagement::getAddressRange}
};

// Custom action map:
// "attribute name" -> SET request handlers
map <const string, UpnpLanHostConfigManagement::SetAttributeHandler>
UpnpLanHostConfigManagement::SetAttributeActionMap =
{
    {"addrRange", &UpnpLanHostConfigManagement::setAddressRange}
};

void UpnpLanHostConfigManagement::getAddressRangeCb(GUPnPServiceProxy *proxy,
        GUPnPServiceProxyAction *actionProxy,
        gpointer userData)
{
    GError *error = NULL;
    const char *minAddress;
    const char *maxAddress;

    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  "NewMinAddress",
                                                  G_TYPE_STRING,
                                                  &minAddress,
                                                  "NewMaxAddress",
                                                  G_TYPE_STRING,
                                                  &maxAddress,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("GetAddressRange failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    if (status)
    {
        RCSResourceAttributes addrRange;

        DEBUG_PRINT("minAddress=" << minAddress << ", maxAddress=" << maxAddress);
        addrRange["minAddress"]   = string(minAddress);
        addrRange["maxAddress"]   = string(maxAddress);
        request->resource->setAttribute("addrRange", addrRange, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpLanHostConfigManagement::getAddressRange(UpnpRequest *request)
{
    DEBUG_PRINT("");
    GUPnPServiceProxyAction *actionProxy = gupnp_service_proxy_begin_action (m_proxy,
                                           "GetAddressRange",
                                           getAddressRangeCb,
                                           (gpointer *) request,
                                           NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    return true;
}

void UpnpLanHostConfigManagement::setAddressRangeCb(GUPnPServiceProxy *proxy,
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
        ERROR_PRINT("SetAddressRange failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    if (status)
    {
        const char *minAddress;
        const char *maxAddress;
        RCSResourceAttributes addrRange;
        std::map< GUPnPServiceProxyAction *, std::pair <UpnpAttributeInfo *, std::vector <UpnpVar> > >::iterator
        it = request->proxyMap.find(actionProxy);

        assert(it != request->proxyMap.end());

        // Important! Values need to retrieved in the same order they
        // have been stored
        minAddress = it->second.second[0].var_pchar;
        maxAddress = it->second.second[1].var_pchar;

        DEBUG_PRINT("minAddress=" << minAddress << ", maxAddress=" << maxAddress);
        addrRange["minAddress"]   = string(minAddress);
        addrRange["maxAddress"]   = string(maxAddress);
        request->resource->setAttribute("addrRange", addrRange, false);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpLanHostConfigManagement::setAddressRange(UpnpRequest *request,
        RCSResourceAttributes::Value *attrValue)
{
    DEBUG_PRINT("");
    GUPnPServiceProxyAction *actionProxy;
    const char *sMinAddr;
    const char *sMaxAddr;
    UpnpVar value;
    int count = 0;

    const auto &attrVector = attrValue->get< vector< RCSResourceAttributes > >();
    for (const auto &attrs : attrVector)
    {
        for (const auto &kvPair : attrs)
        {
            string sValue = (kvPair.value()). get < string> ();
            if (kvPair.key() == "minAddr")
            {
                sMinAddr = sValue.c_str();
                count++;
            }
            else if (kvPair.key() == "maxAddr")
            {
                sMaxAddr = sValue.c_str();
                count++;
            }
            else
            {
                ERROR_PRINT("Invalid attribute name:" << kvPair.key());
                return false;
            }
        }
    }

    // Check, if all the sub-attributes have been initialized.
    // In case of "addrRange", there are 2 sub-attributes: "minAddr" and "maxAddr"
    if (count < 2)
    {
        ERROR_PRINT("Incomplete attribute");
        return false;
    }

    actionProxy = gupnp_service_proxy_begin_action (m_proxy,
                                                    "SetAddressRange",
                                                    setAddressRangeCb,
                                                    (gpointer *) request,
                                                    "MinAddress",
                                                    G_TYPE_STRING,
                                                    sMinAddr,
                                                    "MaxAddress",
                                                    G_TYPE_STRING,
                                                    sMaxAddr,
                                                    NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy].first  = m_attributeMap["addrRange"].first;

    // Important! Values need to be stored in the same order they
    // going to be retrieved
    value.var_pchar = sMinAddr;
    request->proxyMap[actionProxy].second.push_back(value);
    value.var_pchar = sMaxAddr;
    request->proxyMap[actionProxy].second.push_back(value);

    return true;
}

bool UpnpLanHostConfigManagement::getAttributesRequest(UpnpRequest *request)
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

bool UpnpLanHostConfigManagement::setAttributesRequest(const RCSResourceAttributes &value,
        UpnpRequest *request)
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
