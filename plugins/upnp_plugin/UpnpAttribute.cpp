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

void UpnpAttribute::getCb(GUPnPServiceProxy *proxy,
                          GUPnPServiceProxyAction *actionProxy,
                          gpointer userData)
{
    GError *error = NULL;
    UpnpVar value;
    UpnpRequest *request = static_cast<UpnpRequest *> (userData);
    UpnpAttributeInfo *attrInfo;

    std::map< GUPnPServiceProxyAction *, UpnpAttributeInfo *>::iterator it = request->proxyMap.find(actionProxy);

    assert(it != request->proxyMap.end());

    attrInfo = it->second;
    DEBUG_PRINT(attrInfo->actions[0].varName);
    bool status = gupnp_service_proxy_end_action (proxy,
                                                  actionProxy,
                                                  &error,
                                                  attrInfo->actions[0].varName,
                                                  attrInfo->actions[0].varType,
                                                  &value.var_int64,
                                                  NULL);

    if (error)
    {
        ERROR_PRINT("\"" << attrInfo->actions[0].name << "\" action failed: " << error->code << ", " <<
                    error->message);
        g_error_free (error);
        status = false;
    }

    if (status)
    {
        switch (attrInfo->actions[0].varType)
        {
            case G_TYPE_STRING:
                {
                    DEBUG_PRINT("resource: " << request->resource->m_uri << ", " << attrInfo->name << ":(string) " <<
                                string(static_cast<const char *>(value.var_pchar)));
                    if (NULL != value.var_pchar)
                    {
                        request->resource->setAttribute(attrInfo->name, string(value.var_pchar), false);
                        g_free(value.var_pchar);
                    }
                    break;
                }
            case G_TYPE_BOOLEAN:
                {
                    bool vbool = value.var_boolean;
                    DEBUG_PRINT("resource: " << request->resource->m_uri << ", " << attrInfo->name << ":(bool) " <<
                                vbool);
                    request->resource->setAttribute(attrInfo->name, vbool, false);
                    break;
                }
            case G_TYPE_INT:
            case G_TYPE_UINT:
                {
                    DEBUG_PRINT("resource: " << request->resource->m_uri << ", " << attrInfo->name << ":(int) " <<
                                value.var_int);
                    request->resource->setAttribute(attrInfo->name, value.var_int, false);
                    break;
                }
            case G_TYPE_INT64:
            case G_TYPE_UINT64:
                {
                    DEBUG_PRINT("resource: " << request->resource->m_uri << ", " << attrInfo->name << ":(int64) " <<
                                value.var_uint64);
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
    else
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
    GUPnPServiceProxyAction *actionProxy = gupnp_service_proxy_begin_action (serviceProxy,
                                                                             attrInfo->actions[0].name,
                                                                             getCb,
                                                                             (gpointer *) request,
                                                                             NULL);
    if (NULL == actionProxy)
    {
        return false;
    }

    // Hold on to the attribute info
    request->proxyMap[actionProxy] = attrInfo;

    return true;
}

void UpnpAttribute::setCb(GUPnPServiceProxy *proxy,
                          GUPnPServiceProxyAction *proxyAction,
                          gpointer userData)
{
    GError *error = NULL;
    UpnpRequest *request = static_cast<UpnpRequest *> (userData);

    bool status = gupnp_service_proxy_end_action (proxy,
                                                  proxyAction,
                                                  &error,
                                                  NULL);
    if (error)
    {
        ERROR_PRINT("Set action failed: " << error->code << ", " << error->message);
        g_error_free (error);
        status = false;
    }

    UpnpRequest::requestDone(request, status);
}
