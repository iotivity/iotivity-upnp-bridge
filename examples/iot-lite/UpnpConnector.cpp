//******************************************************************
//
// Copyright 2018 Intel Corporation All Rights Reserved.
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

#include <string>
#include <iostream>
#include <future>
#include <glib.h>
#include <glib-object.h>
#include <gssdp.h>
#include <gupnp.h>
#include <soup.h>
#include <boost/regex.hpp>

#define OC_SERVER
#define OC_DYNAMIC_ALLOCATION
#include <oc_core_res.h>

#include <UpnpConstants.h>

#include "UpnpConnector.h"

#include "UpnpDevice.h"
#include "UpnpException.h"
#include "UpnpInternal.h"
#include "UpnpManager.h"
#include "UpnpRequest.h"
#include "UpnpResource.h"
#include "UpnpService.h"

using namespace std;
using namespace boost;

static const string MODULE = "UpnpConnector";

static GMainLoop *s_mainLoop;
static GMainContext *s_mainContext;
static GUPnPContextManager *s_contextManager;
static UpnpManager *s_manager;
static UpnpRequestState s_requestState;
static map <gulong, GUPnPControlPoint *> s_signalMap;

static map <string, int> s_deviceIndexLookup; // key is udn
static int s_deviceIndex = 0;

static bool isRootDiscovery[] = {false, true};

UpnpConnector::UpnpConnector()
{
    DEBUG_PRINT("");

    s_manager = new UpnpManager();
    s_signalMap.clear();
}

UpnpConnector::~UpnpConnector()
{
    DEBUG_PRINT("");
    s_signalMap.clear();
    delete s_manager;
    s_manager = NULL;
}

void UpnpConnector::disconnect()
{
    DEBUG_PRINT("");

    if (!s_mainLoop)
    {
        DEBUG_PRINT("Already disconnected");
        return;
    }

    std::promise< bool > promise;
    UpnpRequest request;
    request.done = 0;
    request.expected = 0;
    {
        std::lock_guard< std::mutex > lock(s_requestState.queueLock);
        request.start = [&] ()
        {
            s_manager->stop();
            gupnpStop();
            return true;
        };
        request.finish = [&] (bool status) { promise.set_value(true); };
        s_requestState.requestQueue.push(&request);

        if (s_requestState.sourceId == 0)
        {
            s_requestState.sourceId = g_source_attach(s_requestState.source, s_mainContext);
            DEBUG_PRINT("sourceId: " << s_requestState.sourceId << "context: " << s_mainContext);
        }

    }
    promise.get_future().get();
}

void UpnpConnector::gupnpStop()
{
    DEBUG_PRINT("");
    g_source_unref(s_requestState.source);
    g_source_destroy(s_requestState.source);
    s_requestState.sourceId = 0;

    for (auto it : s_signalMap)
    {
        g_signal_handler_disconnect (it.second, it.first);
    }

    g_object_unref(s_contextManager);
    g_main_loop_quit(s_mainLoop);
    g_main_loop_unref(s_mainLoop);
    s_mainLoop = NULL;
}

void UpnpConnector::connect()
{
    std::thread discoveryThread(&UpnpConnector::gupnpStart, this);
    discoveryThread.detach();
}

void UpnpConnector::startDiscovery(GUPnPControlPoint *controlPoint)
{
    gulong instanceId;

    // the 'device-proxy-unavailable' signal is sent when any devices are lost
    instanceId = g_signal_connect(controlPoint, "device-proxy-unavailable",
                                  G_CALLBACK (&UpnpConnector::onDeviceProxyUnavailable), NULL);
    s_signalMap[instanceId] = controlPoint;

    // the 'device-proxy-available' signal is sent when any devices are found
    instanceId = g_signal_connect(controlPoint, "device-proxy-available",
                                  G_CALLBACK (&UpnpConnector::onDeviceProxyAvailable), (gpointer *) (&isRootDiscovery[0]));
    s_signalMap[instanceId] = controlPoint;

    // the 'service-proxy-unavailable' signal is sent when any services are lost
    instanceId = g_signal_connect(controlPoint, "service-proxy-unavailable",
                                  G_CALLBACK (&UpnpConnector::onServiceProxyUnavailable), NULL);
    s_signalMap[instanceId] = controlPoint;

    // the 'service-proxy-available' signal is sent when any services are found
    instanceId = g_signal_connect(controlPoint, "service-proxy-available",
                                  G_CALLBACK (&UpnpConnector::onServiceProxyAvailable), NULL);
    s_signalMap[instanceId] = controlPoint;

    // tell the control point to start searching
    gssdp_resource_browser_set_active(GSSDP_RESOURCE_BROWSER(controlPoint), true);
}

void UpnpConnector::gupnpStart()
{
    DEBUG_PRINT("");
    if (s_mainLoop)
    {
        DEBUG_PRINT("Don't start UPnP discovery twice!");
        return;
    }

    // create a new gupnp context manager
    s_contextManager = gupnp_context_manager_create(0);

    g_signal_connect(s_contextManager, "context-available",
                     G_CALLBACK(&UpnpConnector::onContextAvailable), NULL);

    DEBUG_PRINT("UPnP main loop starting... (" << std::this_thread::get_id() << ")");
    s_mainLoop = g_main_loop_new(NULL, false);
    s_mainContext = g_main_context_default();
    DEBUG_PRINT("main context" << s_mainContext);

    s_requestState.context = s_mainContext;
    initResourceCallbackHandler();
    g_main_loop_run(s_mainLoop);
}

int UpnpConnector::checkRequestQueue(gpointer data)
{
    // Check request queue
    std::lock_guard< std::mutex > lock(s_requestState.queueLock);
    DEBUG_PRINT("(" << s_requestState.requestQueue.size() << ")");

    while (!s_requestState.requestQueue.empty())
    {
        UpnpRequest *request = s_requestState.requestQueue.front();
        bool status = request->start();

        // If request completed, finalize here
        if (request->done == request->expected)
        {
            DEBUG_PRINT("finish " << request);
            request->finish(status);
        }
        s_requestState.requestQueue.pop();
    }

    DEBUG_PRINT("sourceId: " << s_requestState.sourceId << ", context: " << g_source_get_context (
                    s_requestState.source));
    if (s_requestState.sourceId != 0)
    {
        g_source_unref(s_requestState.source);
        g_source_destroy(s_requestState.source);
        s_requestState.sourceId = 0;

        // Prepare for the next scheduling call
        initResourceCallbackHandler();
    }
    return G_SOURCE_REMOVE;
}

void UpnpConnector::initResourceCallbackHandler()
{
    s_requestState.source = g_idle_source_new();
    g_source_set_priority(s_requestState.source, G_PRIORITY_HIGH_IDLE);
    g_source_set_callback(s_requestState.source,
                          checkRequestQueue,
                          NULL,
                          NULL);
}

// Callback: a gupnp context is available
void UpnpConnector::onContextAvailable(GUPnPContextManager *manager, GUPnPContext *context)
{
    GUPnPControlPoint * controlPointRoot;
    GUPnPControlPoint * controlPointAll;
    gulong instanceId;

    DEBUG_PRINT("context: " << context << ", manager: " << manager);

    // create a control point for root devices
    controlPointRoot = gupnp_control_point_new(context, "upnp:rootdevice");

    // the 'device-proxy-available' signal is sent when any devices are found
    instanceId = g_signal_connect(controlPointRoot, "device-proxy-available",
                                  G_CALLBACK (&UpnpConnector::onDeviceProxyAvailable), (gpointer *) (&isRootDiscovery[1]));
    s_signalMap[instanceId] = controlPointRoot;

    // let the context manager take care of this control point's life cycle
    gupnp_context_manager_manage_control_point(manager, controlPointRoot);
    g_object_unref(controlPointRoot);

    // create a control point (for all devices and services)
    controlPointAll = gupnp_control_point_new(context, "ssdp:all");
    startDiscovery(controlPointAll);

    // let the context manager take care of this control point's life cycle
    gupnp_context_manager_manage_control_point(manager, controlPointAll);
    g_object_unref(controlPointAll);
}

// Callback: a device has been discovered
void UpnpConnector::onDeviceProxyAvailable(GUPnPControlPoint *controlPoint,
                                           GUPnPDeviceProxy *proxy,
                                           gpointer userData)
{
    GUPnPDeviceInfo *deviceInfo = GUPNP_DEVICE_INFO(proxy);
    UpnpResource::Ptr pUpnpResource;
    const string udn = gupnp_device_info_get_udn(deviceInfo);
    bool isRoot = *(static_cast <bool *> (userData));

    DEBUG_PRINT("Device type: " << gupnp_device_info_get_device_type(deviceInfo));
#ifndef NDEBUG
    char *devModel = gupnp_device_info_get_model_name(deviceInfo);
    if (devModel != NULL)
    {
        DEBUG_PRINT("\tDevice model: " << devModel);
        g_free(devModel);
    }

    char *devName = gupnp_device_info_get_friendly_name(deviceInfo);
    if (devName != NULL)
    {
        DEBUG_PRINT("\tFriendly name: " << devName);
        g_free(devName);
    }
#endif
    DEBUG_PRINT("\tUdn: " << udn);

    if (isRoot)
    {
        // Root device
        pUpnpResource = s_manager->processDevice(proxy, deviceInfo, true, &s_requestState);
    }
    else
    {
        pUpnpResource = s_manager->processDevice(proxy, deviceInfo, false, &s_requestState);
    }

    if (pUpnpResource != nullptr && !pUpnpResource->isRegistered())
    {
        if (pUpnpResource->getResourceType() == UPNP_OIC_TYPE_DEVICE_LIGHT ||
            pUpnpResource->getResourceType() == UPNP_OIC_TYPE_DEVICE_AV_PLAYER)
        {
            int ret = oc_add_device("/oic/d", pUpnpResource->getResourceType().c_str(),
                    pUpnpResource->getName().c_str(), "ocf.1.0.0", "ocf.res.1.0.0,ocf.sh.1.0.0",
                    UpnpDevice::addDeviceCallback, deviceInfo);

            if (ret == 0)
            {
                s_deviceIndexLookup[pUpnpResource->getUdn()] = ++s_deviceIndex;
                pUpnpResource->setRegistered(true);
                DEBUG_PRINT("Added " << pUpnpResource->getResourceType() << " device " << pUpnpResource->getName());

//                for (std::map< string, int >::iterator iter = s_deviceIndexLookup.begin(); iter != s_deviceIndexLookup.end(); ++iter)
//                  std::cout << iter->first << " => " << iter->second << '\n';
            }
            else
            {
                pUpnpResource->setRegistered(false);
                unregisterDeviceResource(udn);
                DEBUG_PRINT("Failed to add " << pUpnpResource->getResourceType() << " device " << pUpnpResource->getName());
                return;
            }
        }
        else
        {
            DEBUG_PRINT("For now, ignoring upnp device " << pUpnpResource->getName() <<
                    " (" << pUpnpResource->getResourceType() << ")");
            return;
        }

        // Traverse the service list and register all the services where isReady() returns true.
        // This is done in order to catch all the services that have been seen
        // prior to discovering the hosting device.
        GList *childService = gupnp_device_info_list_services (deviceInfo);

        while (childService)
        {
            GUPnPServiceInfo *serviceInfo = GUPNP_SERVICE_INFO (childService->data);
            std::shared_ptr<UpnpResource> pUpnpResourceService = s_manager->findResource(serviceInfo);
            if (pUpnpResourceService == nullptr)
            {
                DEBUG_PRINT("Registering device: Service link is empty!");
                // This could happen if support for the service is not implemented
            }
            else
            {
                DEBUG_PRINT(pUpnpResourceService->getUri() << "ready " << pUpnpResourceService->isReady() <<
                            " and registered " << pUpnpResourceService->isRegistered());
                if (pUpnpResourceService->isReady() && !pUpnpResourceService->isRegistered())
                {
                    int deviceIndex = s_deviceIndexLookup[pUpnpResource->getUdn()];
                    DEBUG_PRINT("Register resource for previously discovered child service: " <<
                                pUpnpResourceService->getUri() << ", deviceIndex: " << deviceIndex);
                    oc_resource_t *res = createResource(deviceIndex,
                            pUpnpResourceService->getName(), pUpnpResourceService->getUri(),
                            pUpnpResourceService->getResourceType(),
                            pUpnpResourceService->getInterface(),
                            serviceInfo);
                    if (res)
                    {
                        pUpnpResourceService->setRegistered(true);

                        // Subscribe to notifications
                        // Important!!! UpnpService object associated with this info/proxy
                        // must stay valid until we unsubscribe from notificatons. This
                        // means we have to keep a reference to the object inside the
                        // UpnpManager as long as we are subscribed to notifications.
                        gupnp_service_proxy_set_subscribed(GUPNP_SERVICE_PROXY(serviceInfo), true);
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to add resource " << pUpnpResource->getUri());
                    }
                }
            }
            g_object_unref (childService->data);
            childService = g_list_delete_link (childService, childService);
        }
    }
}

void UpnpConnector::onServiceProxyAvailable(GUPnPControlPoint *controlPoint,
        GUPnPServiceProxy *proxy)
{
    GUPnPServiceInfo *info = GUPNP_SERVICE_INFO(proxy);

    DEBUG_PRINT("Service type: " << gupnp_service_info_get_service_type(info));
    DEBUG_PRINT("\tUdn: " << gupnp_service_info_get_udn(info));

    // Get service introspection.
    // TODO: consider using gupnp_service_info_get_introspection_full with GCancellable.
    gupnp_service_info_get_introspection_async (info,
            onIntrospectionAvailable,
            NULL);
}

// Introspection callback
void UpnpConnector::onIntrospectionAvailable(GUPnPServiceInfo *info,
        GUPnPServiceIntrospection *introspection,
        const GError              *error,
        gpointer                  context)
{
    DEBUG_PRINT(gupnp_service_info_get_service_type(info) << ", udn: " << gupnp_service_info_get_udn(
                    info));

    if (error)
    {
        ERROR_PRINT(error->message);
        return;
    }

    UpnpResource::Ptr pUpnpResourceService = s_manager->processService(GUPNP_SERVICE_PROXY (info), info,
            introspection,
            &s_requestState);

    if (introspection != NULL)
    {
        g_object_unref(introspection);
    }

    if (pUpnpResourceService == nullptr || pUpnpResourceService->isRegistered())
    {
        return;
    }

    // Check if the service resource is ready to be registered, i.e.,
    // the resource for the hosting device has been registered.
    // If yes, proceed registering the service resource.
    // Otherwise, the registration will happen when the hosting device proxy becomes available.
    UpnpResource::Ptr pUpnpResourceDevice = s_manager->findDevice(pUpnpResourceService->getUdn());
    if ((pUpnpResourceDevice != nullptr) && pUpnpResourceDevice->isRegistered())
    {
        int deviceIndex = s_deviceIndexLookup[pUpnpResourceService->getUdn()];
        DEBUG_PRINT("Register resource from introspection: " <<
                    pUpnpResourceService->getUri() << ", deviceIndex: " << deviceIndex);
        oc_resource_t *res = createResource(deviceIndex,
                pUpnpResourceService->getName(), pUpnpResourceService->getUri(),
                pUpnpResourceService->getResourceType(),
                pUpnpResourceService->getInterface(),
                info);
        if (res)
        {
            pUpnpResourceService->setRegistered(true);

            // Subscribe to notifications
            // Important!!! UpnpService object associated with this info/proxy
            // must stay valid until we unsubscribe from notifications. This
            // means we have to keep a reference to the object inside the
            // UpnpManager as long as we are subscribed to notifications.
            gupnp_service_proxy_set_subscribed(GUPNP_SERVICE_PROXY(info), true);
        }
        else
        {
            DEBUG_PRINT("Failed to add resource " << pUpnpResourceService->getUri());
        }
    }
}

// This is a recursive call. Should be safe as the depth of a UPNP device
// tree rarely is going to exceed 3 levels. If we ever discover that
// this is not the case, unravel the call into iterative function.
void UpnpConnector::unregisterDeviceResource(string udn)
{
    std::shared_ptr<UpnpDevice> pDevice = s_manager->findDevice(udn);

    if (pDevice == nullptr)
    {
        return;
    }

    // Unregister resources and remove references for embedded services
    for (auto serviceID : pDevice->getServiceList())
    {
        string serviceKey = udn + serviceID;
        std::shared_ptr<UpnpService> pService = s_manager->findService(serviceKey);

        if (pService != nullptr)
        {
            // Unsubscribe from notifications
            if (pService->getProxy() != nullptr)
            {
                gupnp_service_proxy_set_subscribed(pService->getProxy(), false);
            }

            if (pService->isRegistered())
            {
                // Deregister service resource
                size_t deviceIndex = s_deviceIndexLookup[pService->getUdn()];
                string uri = pService->getUri();
                DEBUG_PRINT("Removing resource for: " << uri << ", deviceIndex: " << deviceIndex);
                oc_resource_t *res = oc_ri_get_app_resource_by_uri((const char *)uri.c_str(), strlen((const char *)uri.c_str()), deviceIndex);
                bool result = oc_delete_resource(res);
                DEBUG_PRINT("Delete resource result: " << result);
            }
        }
        s_manager->removeService(serviceKey);
    }

    // Unregister resources and remove references for embedded devices
    for (auto udnChild : pDevice->getDeviceList())
    {
        unregisterDeviceResource(udnChild);
    }
    if (pDevice->isRegistered())
    {
        // There is no api to remove devices, so the core resources will be hidden
        size_t deviceIndex = s_deviceIndexLookup[pDevice->getUdn()];
        DEBUG_PRINT("Hiding core resources for device " << deviceIndex << ", " << pDevice->getName());
        oc_resource_t *res = oc_core_get_resource_by_index(OCF_RES, deviceIndex);
        if (res)
        {
            oc_resource_set_discoverable(res, false);
            oc_resource_set_observable(res, false);
        }
        res = oc_core_get_resource_by_index(OCF_CON, deviceIndex);
        if (res)
        {
            oc_resource_set_discoverable(res, false);
            oc_resource_set_observable(res, false);
        }
        res = oc_core_get_resource_by_index(OCF_INTROSPECTION_WK, deviceIndex);
        if (res)
        {
            oc_resource_set_discoverable(res, false);
            oc_resource_set_observable(res, false);
        }
        res = oc_core_get_resource_by_index(OCF_INTROSPECTION_DATA, deviceIndex);
        if (res)
        {
            oc_resource_set_discoverable(res, false);
            oc_resource_set_observable(res, false);
        }
        res = oc_core_get_resource_by_index(OCF_D, deviceIndex);
        if (res)
        {
            oc_resource_set_discoverable(res, false);
            oc_resource_set_observable(res, false);
        }
    }
    s_manager->removeDevice(udn);
}

void UpnpConnector::onDeviceProxyUnavailable(GUPnPControlPoint *controlPoint,
        GUPnPDeviceProxy *proxy)
{
    GUPnPDeviceInfo *info = GUPNP_DEVICE_INFO(proxy);
    const string udn = gupnp_device_info_get_udn(info);

    DEBUG_PRINT(": " << gupnp_device_info_get_device_type(info));
    DEBUG_PRINT("\tUdn: " << udn);

    unregisterDeviceResource(udn);
}

void UpnpConnector::onServiceProxyUnavailable(GUPnPControlPoint *controlPoint,
        GUPnPServiceProxy *proxy)
{
    GUPnPServiceInfo *info = GUPNP_SERVICE_INFO(proxy);

    DEBUG_PRINT("Service type: " << gupnp_service_info_get_service_type(info));
    DEBUG_PRINT("\tUdn: " << gupnp_service_info_get_udn(info));
    UpnpResource::Ptr pUpnpResourceService = s_manager->findResource(info);

    if (pUpnpResourceService != nullptr)
    {
        // Unsubscribe from notifications
        gupnp_service_proxy_set_subscribed(proxy, false);

        if (pUpnpResourceService->isRegistered())
        {
            // Deregister service resource
            size_t deviceIndex = s_deviceIndexLookup[pUpnpResourceService->getUdn()];
            string uri = pUpnpResourceService->getUri();
            DEBUG_PRINT("Removing resource for: " << uri << ", deviceIndex: " << deviceIndex);
            oc_resource_t *res = oc_ri_get_app_resource_by_uri((const char *)uri.c_str(), strlen((const char *)uri.c_str()), deviceIndex);
            bool result = oc_delete_resource(res);
            DEBUG_PRINT("Delete resource result: " << result);
        }
        s_manager->removeService(info);
    }
}

oc_resource_t *UpnpConnector::createResource(int device, const string name, const string uri,
        const string resourceType, const string resourceInterface, GUPnPServiceInfo *serviceInfo)
{
    size_t deviceIndex = device;
    oc_resource_t *res = oc_ri_get_app_resource_by_uri((const char *)uri.c_str(), strlen((const char *)uri.c_str()), deviceIndex);

    if (! res)
    {
        res = oc_new_resource((const char *)name.c_str(), (const char *)uri.c_str(), 1, deviceIndex);
        if (res)
        {
            const char *ri = resourceInterface.c_str();
            size_t ri_len = strlen(ri);
            oc_interface_mask_t interfaces = oc_ri_get_interface_mask((char *)ri, ri_len);

            oc_resource_bind_resource_type(res, (const char *)resourceType.c_str());
            oc_resource_bind_resource_interface(res, interfaces);
            oc_resource_set_default_interface(res, interfaces);
            oc_resource_set_discoverable(res, true);
            oc_resource_set_observable(res, true);
            oc_resource_set_request_handler(res, OC_GET, UpnpService::processGetRequest, serviceInfo);
            oc_resource_set_request_handler(res, OC_POST, UpnpService::processPostRequest, serviceInfo);
            oc_resource_set_request_handler(res, OC_PUT, UpnpService::processPutRequest, serviceInfo);
            oc_add_resource(res);

            UpnpService::setDeviceIndexForUri(uri, device);
            DEBUG_PRINT("Created resource " << uri << " for deviceIndex " << device);
        }
        else
        {
            DEBUG_PRINT("Failed to create resource " << uri);
        }
    }
    else
    {
        DEBUG_PRINT("Not creating resource " << uri << " (already exists)");
    }

    return res;
}
