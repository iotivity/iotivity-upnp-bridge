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

#include <soup.h>
#include <future>
#include <thread>

#include "UpnpConstants.h"
#include "UpnpException.h"
#include "UpnpInternal.h"
#include "UpnpService.h"

using namespace std;
using namespace OIC::Service;

static const string MODULE = "UpnpService";

UpnpService::UpnpService(GUPnPServiceInfo *serviceInfo,
                         string type,
                         UpnpRequestState *requestState,
                         vector <UpnpAttributeInfo> *attributeInfo)
{
    DEBUG_PRINT("(" << std::this_thread::get_id() << ")");
    m_proxy = nullptr;
    m_resourceType = type;

    if (attributeInfo == nullptr)
    {
        ERROR_PRINT("Service attribute table for " << m_resourceType << " not present!");
        throw NotImplementedException("UpnpService::ctor: Service attribute table for " + m_resourceType +
                                      " not present!");
        return;
    }

    m_serviceAttributeInfo = attributeInfo;
    m_requestState = requestState;

    //UDN of the hosting device
    m_udn = gupnp_service_info_get_udn(serviceInfo);

    // Service ID (differentiates between different services of identical type hosted by one device).
    m_serviceId = getStringField(gupnp_service_info_get_id, serviceInfo);

    // Use service ID as basis for resource name, stripping service ID prefix.
    // Check that service ID was set. If not, raise exception.
    if (m_serviceId == "")
    {
        //TODO change to invalid exception
        ERROR_PRINT("Service ID for " << m_resourceType << " not present!");
        throw NotImplementedException("UpnpService::ctor: Service ID for " + m_resourceType +
                                      " not present!");
        return;
    }

    if (m_serviceId.compare(0, UPNP_PREFIX_SERVICE_ID.size(), UPNP_PREFIX_SERVICE_ID) != 0)
    {
        ERROR_PRINT("Invalid service ID format " << m_serviceId);
        return;
    }

    m_name = m_serviceId.substr(UPNP_PREFIX_SERVICE_ID.size());

    //TODO: discuss service URI format. Currently: resource_type/service_id/udn
    m_uri = UpnpUriPrefixMap[m_resourceType] + m_name + "/" + m_udn;

    if (m_uri.length() > MAX_URI_LENGTH)
    {
        ERROR_PRINT("URI too long " << m_uri << "( " << m_uri.length());
        throw BadUriException("UpnpService::ctor: uri length too long");
        return;
    }

    m_interface = UpnpInterfaceMap[m_resourceType];

}

UpnpService::~UpnpService()
{
    DEBUG_PRINT("(" << std::this_thread::get_id() << "), uri:" << m_uri);

    if (!m_stateVarMap.empty())
    {
        std::map<string, StateVarAttr>::iterator it;
        for (it = m_stateVarMap.begin(); it != m_stateVarMap.end(); ++it)
        {
            DEBUG_PRINT("remove notify for \"" << it->first << "\"");

            gupnp_service_proxy_remove_notify (m_proxy,
                                               (it->first).c_str(),
                                               onStateChanged,
                                               this);
        }
        m_stateVarMap.clear();
    }

    m_proxy = nullptr;
}

RCSResourceAttributes UpnpService::handleGetAttributesRequest()
{
    DEBUG_PRINT("(" << std::this_thread::get_id() << "), uri:" << m_uri);

    std::promise< bool > promise;
    UpnpRequest request;
    request.done = 0;
    request.expected = m_attributeMap.size();
    request.resource = this;
    {
        std::lock_guard< std::mutex > lock(m_requestState->queueLock);
        request.start = [&] ()
        {
            bool status = getAttributesRequest(&request);
            return status;
        };
        request.finish = [&] (bool status) { DEBUG_PRINT("finish get request"); promise.set_value(status); };
        m_requestState->requestQueue.push(&request);

        // Add the gupnp callback if it has not been scheduled yet
        if (m_requestState->sourceId == 0)
        {
            DEBUG_PRINT("source: " << m_requestState->source << "context: " << m_requestState->context);
            m_requestState->sourceId = g_source_attach(m_requestState->source, m_requestState->context);
        }
    }

    bool status = promise.get_future().get();

    // Notice the API deficiency: there is no way to return error code
    // to the iotivity layer
    if (status != true)
    {
        DEBUG_PRINT("Failed to get attributes for " << m_uri);
    }

    return BundleResource::getAttributes();

}

void UpnpService::handleSetAttributesRequest(const RCSResourceAttributes &value)
{
    DEBUG_PRINT("(" << std::this_thread::get_id() << "), uri:" << m_uri);

    std::promise< bool > promise;
    UpnpRequest request;
    request.done = 0;
    request.expected = value.size();
    request.resource = this;
    {
        std::lock_guard< std::mutex > lock(m_requestState->queueLock);
        request.start = [&] ()
        {
            bool status = setAttributesRequest(value, &request);
            return status;
        };
        request.finish = [&] (bool status) { DEBUG_PRINT("finish set request"); promise.set_value(status); };
        m_requestState->requestQueue.push(&request);

        // Add the gupnp callback if it has not been scheduled yet
        if (m_requestState->sourceId == 0)
        {
            m_requestState->sourceId = g_source_attach(m_requestState->source, m_requestState->context);
        }
    }
    bool status = promise.get_future().get();

    // Notice the API deficiency: there is no way to return error code
    // to the iotivity layer
    if (!status)
    {
        DEBUG_PRINT("failed to set attributes for " <<  m_uri);
    }
}

void UpnpService::setProxy(GUPnPServiceProxy *proxy)
{
    m_proxy = proxy;
}

GUPnPServiceProxy *UpnpService::getProxy()
{
    return m_proxy;
}

string UpnpService::getId()
{
    return m_serviceId;
}

void UpnpService::processIntrospection(GUPnPServiceProxy *proxy,
                                       GUPnPServiceIntrospection *introspection)
{

    // Load attributes description
    vector <UpnpAttributeInfo> *attributeList = m_serviceAttributeInfo;
    vector <UpnpAttributeInfo>::iterator attr;
    const GList *actionNameList = gupnp_service_introspection_list_action_names(introspection);

    // Populate the name list of supported attributes
    if (actionNameList != NULL)
    {
        const GList *l;
        DEBUG_PRINT("# of actions: " << g_list_length((GList *)actionNameList));
        // Generate convenient map of actions associated with the service (UPnP)
        // "UPnP acttion name" -> ("OCF attribute name", GET/POST/PUT)
        std::map <const string, pair<string, UpnpActionType>> actionMap;
        for (attr = attributeList->begin() ; attr != attributeList->end() ; ++attr)
        {
            if (!attr->actions.empty())
            {
                for (auto action : attr->actions)
                {
                    actionMap[action.name] = {attr->name, action.type};
                }
            }
        }

        for (l = actionNameList; l != NULL; l = l->next)
        {
            const string actionName = string ((char *) l->data);
            std::map<const string, pair<string, UpnpActionType>>::iterator it = actionMap.find(actionName);

            if (it != actionMap.end())
            {
                string attrName =  (it->second).first;

                // Update/add the supported attribute in the attribute map associated
                // with this particular instance of the service
                UpnpAttributeInfo *attrInfo = UpnpAttribute::getAttributeInfo(attributeList, attrName);

                if (attrInfo == nullptr)
                {
                    //TODO: throw exception
                    ERROR_PRINT(attrName << " not found!");

                    assert(0);
                }
                int flags = m_attributeMap[attrName].second | (it->second).second;
                m_attributeMap[attrName] = {attrInfo, flags};

                DEBUG_PRINT("Action: " << actionName << " maps to \"" << attrName << "\" ( flags: " <<
                            m_attributeMap[attrName].second << " )");
            }
            else
            {
                DEBUG_PRINT("Match not found for action: " << actionName);
            }
        }
        DEBUG_PRINT("Matched " << m_attributeMap.size() << " attributes");
    }

    // Initialize attributes
    initAttributes();

    // Generate convenient map of UPnP state variables that are observed/notified to
    // corresponding OCF attributes
    map <string, StateVarAttr> varMap;

    for (attr = attributeList->begin() ; attr != attributeList->end() ; ++attr)
    {
        if (!attr->evented)
        {
            // Not  observable attribute
            continue;
        }

        if (string(attr->varName) != "")
        {
            DEBUG_PRINT("Attr State variable: " << attr->varName);
            varMap[string(attr->varName)].attrName = attr->name;
            varMap[string(attr->varName)].type = attr->type;
            varMap[string(attr->varName)].parentName = "";
        }
        else if (!attr->attrs.empty()) // check if embedded attributes
            // are observable
        {
            for (auto &it : attr->attrs)
            {
                if (it.evented)
                {
                    string stateVarName = string(it.varName);

                    DEBUG_PRINT("Attr State variable: " << stateVarName);
                    varMap[stateVarName].attrName = it.name;
                    varMap[stateVarName].type = it.type;
                    varMap[stateVarName].parentName = attr->name;
                }
            }
        }
    }

    if (varMap.empty())
    {
        return;
    }

    // Set notifications on supported state variables
    const GList *stateVarList = gupnp_service_introspection_list_state_variable_names(introspection);

    if (stateVarList != NULL)
    {
        const GList *l;
        DEBUG_PRINT(" # of state variables: " << g_list_length((GList *)stateVarList));
        for (l = stateVarList; l != NULL; l = l->next)
        {
            const char *varName = (const char *) l->data;
            DEBUG_PRINT("State variable: " << varName);

            std::map<string, StateVarAttr>::iterator it = varMap.find(string(varName));

            if (it != varMap.end())
            {
                if (string(it->first) == string(varName))
                {
                    if (!gupnp_service_proxy_add_notify (proxy,
                                                         varName,
                                                         (it->second).type,
                                                         onStateChanged,
                                                         this))
                    {
                        ERROR_PRINT("Failed to add notify for " << varName);
                    }
                    else
                    {
                        DEBUG_PRINT("Added notify for: " << varName << ", " << (it->second).attrName <<
                                    ", " << g_type_name((it->second).type));
                        m_stateVarMap[it->first] = it->second;
                    }
                }
            }

        }
    }
}

void UpnpService::onStateChanged(GUPnPServiceProxy *proxy,
                                 const char *variable,
                                 GValue *value,
                                 gpointer userData)
{
    UpnpService *pService = (UpnpService *) userData;

    std::map<string, StateVarAttr>::iterator it = pService->m_stateVarMap.find(string(variable));

    DEBUG_PRINT("(" << std::this_thread::get_id() << "): notification state variable: " << variable);

    if (it == pService->m_stateVarMap.end())
    {
        DEBUG_PRINT("state var not found: " << variable);
        return;
    }


    string attrName = (it->second).attrName;
    string parentName = (it->second).parentName;

    // Check if the value needs customized conversion (specific
    // to a particular service obect)
    if (pService->processNotification(attrName, parentName, value))
    {
        return;
    }

    GType type = (it->second).type;
    if (parentName == "")
    {
        if (type == G_TYPE_BOOLEAN)
        {
            bool vbool = g_value_get_boolean(value);
            DEBUG_PRINT("set " << attrName << " to " << vbool);
            pService->BundleResource::setAttribute(attrName, vbool);
        }
        else if (type == G_TYPE_UINT)
        {
            DEBUG_PRINT("set " << attrName << " to " << g_value_get_uint(value));
            pService->BundleResource::setAttribute(attrName, (int) (g_value_get_uint(value)));
        }
        else if (type == G_TYPE_INT)
        {
            DEBUG_PRINT("set " << attrName << " to " << g_value_get_int(value));
            pService->BundleResource::setAttribute(attrName, (int) (g_value_get_int(value)));
        }
        else if (type == G_TYPE_STRING)
        {
            DEBUG_PRINT("set " << attrName << " to " << g_value_get_string(value));
            pService->BundleResource::setAttribute(attrName, string(g_value_get_string(value)));
        }
        else
        {
            //TODO this should probably throw and error.
            ERROR_PRINT("Type handling not implemented: " << g_type_name(type));
        }
    }
    else
    {
        RCSResourceAttributes::Value attrValue = pService->BundleResource::getAttribute(parentName);
        RCSResourceAttributes composite = attrValue.get < RCSResourceAttributes > ();

        if (type == G_TYPE_BOOLEAN)
        {
            bool vbool = g_value_get_boolean(value);
            DEBUG_PRINT("set composite: " << parentName <<  "->" << attrName << " to " << vbool);
            composite[attrName] = vbool;
        }
        else if (type == G_TYPE_UINT)
        {
            DEBUG_PRINT("set composite: " << parentName <<  "->" << attrName << " to " << g_value_get_uint(
                            value));
            composite[attrName] = (int) (g_value_get_uint(value));
        }
        else if (type == G_TYPE_INT)
        {
            DEBUG_PRINT("set composite: " << parentName <<  "->" << attrName << " to " << g_value_get_int(
                            value));
            composite[attrName] = (int) (g_value_get_int(value));
        }
        else if (type == G_TYPE_STRING)
        {
            DEBUG_PRINT("set composite: " << parentName <<  "->" << attrName << " to " << g_value_get_string(
                            value));
            composite[attrName] = string(g_value_get_string(value));
        }
        else
        {
            //TODO this should probably throw and error.
            ERROR_PRINT("Type handling not implemented: " << g_type_name(type));
            return;
        }

        pService->BundleResource::setAttribute(parentName, composite);
    }
}

// TODO: This should probably live in some UpnpUtil class
string UpnpService::getStringField(function< char *(GUPnPServiceInfo *serviceInfo)> f,
                                   GUPnPServiceInfo *serviceInfo)
{
    char *c_field = f(serviceInfo);
    if (c_field != NULL)
    {
        string s_field = string(c_field);
        g_free(c_field);
        return s_field;
    }
    return "";
}

void UpnpService::initCompositeAttribute(RCSResourceAttributes composite,
        vector<EmbeddedAttribute> attrs)
{
    for (auto &attr : attrs)
    {
        if (attr.type == G_TYPE_BOOLEAN)
        {
            composite[attr.name] = false;
        }
        else if ((attr.type == G_TYPE_UINT) || (attr.type == G_TYPE_INT))
        {
            composite[attr.name] = 0;
        }
        else if ((attr.type == G_TYPE_UINT64) || (attr.type == G_TYPE_INT64))
        {
            composite[attr.name] = (double) 0;
        }
        else if (attr.type == G_TYPE_STRING)
        {
            composite[attr.name] = "";
        }
        else
        {
            ERROR_PRINT("Type handling not implemented: " << g_type_name(attr.type));
        }
    }
}

void UpnpService::initAttributes()
{

    BundleResource::setAttribute("name", m_name,
                                 false); // need to keep name with attributes (OCRepresentation bug)
    BundleResource::setAttribute("uri", m_uri,
                                 false);   // need to keep uri with attributes (OCRepresentation bug)
    BundleResource::setAttribute("if", m_interface, false);

    for (auto attr : m_attributeMap)
    {
        if (attr.second.first->type == G_TYPE_BOOLEAN)
        {
            BundleResource::setAttribute(attr.second.first->name, false);
        }
        else if ((attr.second.first->type == G_TYPE_UINT) || (attr.second.first->type == G_TYPE_INT))
        {
            BundleResource::setAttribute(attr.second.first->name, 0);
        }
        else if ((attr.second.first->type == G_TYPE_UINT64) || (attr.second.first->type == G_TYPE_INT64))
        {
            BundleResource::setAttribute(attr.second.first->name, (double) 0);
        }
        else if (attr.second.first->type == G_TYPE_STRING)
        {
            BundleResource::setAttribute(attr.second.first->name, "");
        }
        else if (!attr.second.first->attrs.empty()) // composite attribute
        {
            RCSResourceAttributes composite;

            initCompositeAttribute(composite, attr.second.first->attrs);
            BundleResource::setAttribute(attr.second.first->name, composite);
        }
        else
        {
            ERROR_PRINT("Type handling not implemented: " << g_type_name((attr.second).first->type));
        }
    }

}

bool UpnpService::processNotification(string attrName, string parent, GValue *value)
{
    // Default: no custom variable->attribute conversion
    return false;
}
