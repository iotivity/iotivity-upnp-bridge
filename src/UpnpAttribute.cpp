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

#include "UpnpAttribute.h"

using namespace std;

static const string MODULE = "UpnpAttribute";

UpnpAttributeInfo * UpnpAttribute::getAttributeInfo(std::vector <UpnpAttributeInfo> *serviceAttributes,
                                                    std::string attrName)
{
    vector<UpnpAttributeInfo>::iterator attrInfo;
    for(attrInfo = serviceAttributes->begin() ; attrInfo != serviceAttributes->end() ; ++attrInfo)
    {
        if (attrInfo->name == attrName)
        {
            return &(*attrInfo);
        }
    }
    return nullptr;
}

bool UpnpAttribute::isValidRequest(map <string, pair <UpnpAttributeInfo*, int>> *attrMap,
                                   string attrName,
                                   UpnpActionType actionType)

{
    // Check if the attribute is present
    map  <string, pair< UpnpAttributeInfo*, int>>::iterator attr = attrMap->find(attrName);
    if (attr == attrMap->end())
    {
        return false;
    }

    // Check that expected action for the attribute has been found
    if ((attr->second.second) & (int) actionType)
    {
        return true;
    }

    return false;
}


void UpnpAttribute::getCb(GUPnPServiceProxy *proxy,
                          GUPnPServiceProxyAction *actionProxy,
                          gpointer userData)
{
    GError *error = NULL;
    UpnpVar value;
    UpnpRequest *request = static_cast<UpnpRequest*> (userData);
    UpnpAttributeInfo *attrInfo;

    std::map< GUPnPServiceProxyAction *, std::pair <UpnpAttributeInfo*, std::vector <UpnpVar> > >::iterator it = request->proxyMap.find(actionProxy);

    assert(it != request->proxyMap.end());

    attrInfo = it->second.first;
    DEBUG_PRINT(attrInfo->actions[0].varName);
    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  attrInfo->actions[0].varName,
                                                  attrInfo->actions[0].varType,
                                                  &value.var_int64,
                                                  NULL);

    if (error) {
        ERROR_PRINT("\"" <<attrInfo->actions[0].name << "\" action failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    if (status)
    {
        switch (attrInfo->actions[0].varType)
        {
        case G_TYPE_STRING:
            {
                DEBUG_PRINT("resource: " << request->resource->m_uri << ", " << attrInfo->name << ":(string) "<< string(static_cast<const char *>(value.var_pchar)));
                request->resource->setAttribute(attrInfo->name, string(value.var_pchar), false);
                break;
            }
        case G_TYPE_BOOLEAN:
            {
                bool vbool = value.var_boolean;
                DEBUG_PRINT("resource: " << request->resource->m_uri << ", " << attrInfo->name<<":(bool) "<< vbool);
                request->resource->setAttribute(attrInfo->name, vbool, false);
                break;
            }
        case G_TYPE_INT:
        case G_TYPE_UINT:
            {
                DEBUG_PRINT("resource: " << request->resource->m_uri << ", " << attrInfo->name<<":(int) "<< value.var_int);
                request->resource->setAttribute(attrInfo->name, value.var_int, false);
                break;
            }
        case G_TYPE_INT64:
        case G_TYPE_UINT64:
            {
                DEBUG_PRINT("resource: " << request->resource->m_uri << ", " << attrInfo->name<<":(int64) "<< value.var_uint64);
                request->resource->setAttribute(attrInfo->name, static_cast<double>(value.var_int64), false);
                break;
            }
        default:
            {
                //TODO: handle additional types?
                ERROR_PRINT("Type handling not implemented!");
                assert(0);
            }
        }
    } else
    {
        ERROR_PRINT("Failed to retrieve " << attrInfo->actions[0].varName);
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAttribute::get(GUPnPServiceProxy *serviceProxy,
                        UpnpRequest *request,
                        UpnpAttributeInfo *attrInfo)
{
    DEBUG_PRINT("");
    UpnpVar value;
    GUPnPServiceProxyAction *actionProxy = gupnp_service_proxy_begin_action (serviceProxy,
                                                                             attrInfo->actions[0].name,
                                                                             getCb,
                                                                             (gpointer *) request,
                                                                             NULL);
    // Hold on to the attribute info
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy].first = attrInfo;
    value.var_int64 = 0;
    request->proxyMap[actionProxy].second.push_back(value);
    return true;
}

void UpnpAttribute::setCb(GUPnPServiceProxy *proxy,
                          GUPnPServiceProxyAction *proxyAction,
                          gpointer userData)
{
    GError *error = NULL;
    UpnpRequest *request = static_cast<UpnpRequest*> (userData);
    UpnpAttributeInfo *attrInfo;

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  proxyAction,
                                                  &error,
                                                  NULL);
    if (error) {
        ERROR_PRINT("Set action failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    if (status)
    {
        std::map< GUPnPServiceProxyAction *, std::pair <UpnpAttributeInfo*, std::vector <UpnpVar> > >::iterator it = request->proxyMap.find(proxyAction);

        if (it != request->proxyMap.end())
        {
            attrInfo = it->second.first;
            UpnpVar value = it->second.second[0];

            switch (attrInfo->actions[1].varType)
            {
            case G_TYPE_STRING:
                {
                    DEBUG_PRINT("resource: " << request->resource->m_uri << ", " << attrInfo->name << ":(string) "<< string(value.var_pchar));
                    request->resource->setAttribute(attrInfo->name, string(value.var_pchar), false);
                    break;
                }
            case G_TYPE_BOOLEAN:
                {
                    bool vbool = value.var_boolean;
                    DEBUG_PRINT("resource: " << request->resource->m_uri << ", " << attrInfo->name<<":(bool) "<< vbool);
                    request->resource->setAttribute(attrInfo->name, vbool, false);
                    break;
                }
            case G_TYPE_INT:
            case G_TYPE_UINT:
                {
                    DEBUG_PRINT("resource: " << request->resource->m_uri << ", " << attrInfo->name<<":(int) "<< value.var_int);
                    request->resource->setAttribute(attrInfo->name, value.var_int, false);
                    break;
                }
            case G_TYPE_INT64:
            case G_TYPE_UINT64:
                {
                    DEBUG_PRINT("resource: " << request->resource->m_uri << ", " << attrInfo->name<<":(int64) "<< value.var_uint64);
                    request->resource->setAttribute(attrInfo->name, static_cast<double>(value.var_int64), false);
                    break;
                }
            default:
                {
                    //TODO: handle additional types?
                    ERROR_PRINT("Type handling not implemented!");
                    assert(0);
                }
            }
        }
    }

    UpnpRequest::requestDone(request, status);
}

bool UpnpAttribute::set(GUPnPServiceProxy *serviceProxy,
                        UpnpRequest *request,
                        UpnpAttributeInfo *attrInfo,
                        RCSResourceAttributes::Value* attrValue)
{
    GUPnPServiceProxyAction *actionProxy;
    UpnpVar value;

    if (attrValue != NULL)
    {
        // Type of the value to be stored can be derived either from
        // input variable type or from state variable type
        GType type = (attrInfo->actions[1].varType != G_TYPE_NONE) ?
            attrInfo->actions[1].varType : attrInfo->type;

        switch (type)
        {
        case G_TYPE_STRING:
            {
                const char* sValue = (attrValue->get< string >()).c_str();
                DEBUG_PRINT("resource: " << request->resource->m_uri << ", (string) " << sValue);
                value.var_pchar = sValue;
                break;
            }
        case G_TYPE_BOOLEAN:
            {
                value.var_boolean = attrValue->get< bool >();
                DEBUG_PRINT("resource: " << request->resource->m_uri << ", (bool) " << value.var_boolean);
                break;
            }
        case G_TYPE_INT:
        case G_TYPE_UINT:
            {
                value.var_int = attrValue->get< int >();
                DEBUG_PRINT("resource: " << request->resource->m_uri << ", (int) " << value.var_int);
                break;
            }
        case G_TYPE_INT64:
        case G_TYPE_UINT64:
            {
                value.var_int64 = attrValue->get< double >();
                DEBUG_PRINT("resource: " << request->resource->m_uri << ", (int64) " << value.var_int64);
                break;
            }
        default:
            {
                //TODO: handle additional types?
                ERROR_PRINT("Type handling not implemented!");
                assert(0);
            }
        }
    }
    else
    {
        value.var_int64 = 0;
    }

    if (string(attrInfo->actions[1].varName) == "")
    {
        DEBUG_PRINT("action (no args): " << attrInfo->actions[1].name);
        actionProxy = gupnp_service_proxy_begin_action (serviceProxy,
                                                        attrInfo->actions[1].name,
                                                        setCb,
                                                        (gpointer *) request,
                                                        NULL);
    }
    else
    {
        DEBUG_PRINT("action: " << attrInfo->actions[1].name << "( " << attrInfo->actions[1].varName << " )");
        actionProxy = gupnp_service_proxy_begin_action (serviceProxy,
                                                 attrInfo->actions[1].name,
                                                 setCb,
                                                 (gpointer *) request,
                                                 attrInfo->actions[1].varName,
                                                 attrInfo->actions[1].varType,
                                                 value.var_int64,
                                                 NULL);
    }

    // Hold on to the attribute info and value that is being modified
    if (NULL == actionProxy)
    {
        return false;
    }

    request->proxyMap[actionProxy].first = attrInfo;
    request->proxyMap[actionProxy].second.push_back(value);
    return true;
}
