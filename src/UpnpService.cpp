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
                         UpnpRequestState *requestState)
{
    DEBUG_PRINT("(" << std::this_thread::get_id()<< ")");
    m_proxy = nullptr;
    m_resourceType = type;

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
        throw NotImplementedException("UpnpService::ctor: Service ID for " + m_resourceType + " not present!");
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
        ERROR_PRINT("URI too long " << m_uri<< "( " << m_uri.length());
        throw BadUriException("UpnpService::ctor: uri length too long");
        return;
    }

    m_interface = UpnpInterfaceMap[m_resourceType];

    // Load attributes description
    m_attributes = UpnpAttribute::getServiceAttributeInfo(m_resourceType);
    if (m_attributes == nullptr)
    {
        ERROR_PRINT("Attributes for " << m_resourceType << " not found!");
        throw NotImplementedException("UpnpService::ctor: attributes for " + m_resourceType + " not found!");
        return;
    }

    // Generate convenient maps
    vector<UpnpAttribute>::iterator attr;
    for(attr = m_attributes->begin() ; attr != m_attributes->end() ; ++attr)
    {
        if (attr->stateVar != "")
        {
            m_stateVarMap[attr->stateVar] = {attr->name, attr->type};
        }

        if (!attr->actions.empty())
        {
            for (auto action : attr->actions)
            {
                m_actionMap[action.first] = {attr->name, action.second};
            }
        }
    }

    initAttributes();

}

UpnpService::~UpnpService()
{
   m_proxy = nullptr;
   m_stateVarMap.clear();
}

RCSResourceAttributes UpnpService::handleGetAttributesRequest()
{
    DEBUG_PRINT("(" << std::this_thread::get_id()<< ")");

    std::promise< bool > promise;
    UpnpRequest request;
    request.done = 0;
    request.expected = m_attributeMap.size();
    request.resource = this;
    {
        std::lock_guard< std::mutex > lock(m_requestState->queueLock);
        request.start = [&] (){
            bool status = getAttributesRequest(&request);
            return status;
        };
        request.finish = [&] (bool status) { promise.set_value(status); };
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
    if (status != true) {
        DEBUG_PRINT("Failed to get attributes for " << m_uri);
    }

    return BundleResource::getAttributes();

}

void UpnpService::handleSetAttributesRequest(const RCSResourceAttributes &value)
{
    DEBUG_PRINT("(" << std::this_thread::get_id()<< ")");

    std::promise< bool > promise;
    UpnpRequest request;
    request.done = 0;
    request.expected = value.size();
    request.resource = this;
    {
        std::lock_guard< std::mutex > lock(m_requestState->queueLock);
        request.start = [&] (){
            bool status = setAttributesRequest(value, &request);
            return status;
        };
        request.finish = [&] (bool status) { promise.set_value(status); };
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

GUPnPServiceProxy * UpnpService::getProxy()
{
    return m_proxy;
}

string UpnpService::getId()
{
    return m_serviceId;
}

void UpnpService::processIntrospection(GUPnPServiceProxy *proxy, GUPnPServiceIntrospection *introspection)
{
    const GList *actionNameList = gupnp_service_introspection_list_action_names(introspection);

    // Populate the name list of supported attributes
    if (actionNameList != NULL)
    {
        const GList *l;
        DEBUG_PRINT("# of actions: " << g_list_length((GList *)actionNameList));

        for (l = actionNameList; l != NULL; l = l->next)
        {
            const string actionName = string ((char *) l->data);
            std::map<const string,pair<string, UpnpActionType>>::iterator it = m_actionMap.find(actionName);

            if (it != m_actionMap.end())
            {
                string attrName =  (it->second).first;

                // Update/add the supported attribute in the attribute map associated
                // with this particular instance of the service
                UpnpAttribute *attrInfo = UpnpAttribute::getAttributeInfo(m_attributes, attrName);

                if (attrInfo == nullptr)
                {
                    //TODO: throw exception
                    ERROR_PRINT(attrName << " not found!");

                    assert(0);                }
                attrInfo->flags |= (it->second).second;
                m_attributeMap[attrName] = attrInfo;

                DEBUG_PRINT("Action: "<< actionName << " maps to \"" << attrName << "\" ( flags: " << m_attributeMap[attrName] << " )");
            }
        }
        DEBUG_PRINT("Matched " << m_attributeMap.size() << " attributes");
    }

    if (m_stateVarMap.empty())
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
            const string varName = string ((char *) l->data);
            DEBUG_PRINT("State variable: "<< varName);

            std::map<const string, pair<string, GType>>::iterator it = m_stateVarMap.find(varName);

            if (it != m_stateVarMap.end())
            {
                if (!gupnp_service_proxy_add_notify (proxy,
                                                     varName.c_str(),
                                                     (it->second).second,
                                                     onStateChanged,
                                                     this))
                {
                    ERROR_PRINT("Failed to add notify for " << varName);
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
    UpnpService * pService = (UpnpService *) userData;
    const string varName = string(variable);

    std::map<const string, pair<string, GType>>::iterator it = pService->m_stateVarMap.find(varName);

    DEBUG_PRINT("("<< std::this_thread::get_id() << "): notification state variable: "<< varName);

    if (it == pService->m_stateVarMap.end())
    {
        DEBUG_PRINT("state variable: "<< varName << " not found for" << pService->getId());
        return;
    }

    if ((it->second).second == G_TYPE_BOOLEAN)
    {
        pService->BundleResource::setAttribute((it->second).first, g_value_get_boolean(value));
    }
    else if ((it->second).second == G_TYPE_UINT)
    {
        pService->BundleResource::setAttribute((it->second).first, (int) (g_value_get_uint(value)));
    }
    else
    {
        //TODO this should probably throw and error.
        ERROR_PRINT("Not implemented!");
    }
}

// TODO: This should probably live in some UpnpUtil class
string UpnpService::getStringField(function< char*(GUPnPServiceInfo *serviceInfo)> f, GUPnPServiceInfo *serviceInfo)
{
    char *c_field = f(serviceInfo);
    if (c_field != NULL) {
        string s_field = string(c_field);
        g_free(c_field);
        return s_field;
    }
    return "";
}

void UpnpService::initAttributes()
{
    m_attributeMap.clear();

    BundleResource::setAttribute("name", m_name, false); // need to keep name with attributes (OCRepresentation bug)
    BundleResource::setAttribute("uri", m_uri, false);   // need to keep uri with attributes (OCRepresentation bug)
    BundleResource::setAttribute("if", m_interface, false);

}
