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

#ifndef UPNP_CONNECTOR_H_
#define UPNP_CONNECTOR_H_

#include <functional>
#include <string>

#include <gupnp-control-point.h>
#include <gupnp-device-proxy.h>
#include <gupnp-service-proxy.h>

#include <Configuration.h>
#include <OCPlatform.h>
#include <ProtocolBridgeConnector.h>

#include "UpnpManager.h"
#include "UpnpResource.h"

using namespace OIC::Service;

class UpnpConnector
{
    public:
        typedef std::function< int(UpnpResource::Ptr) > DiscoveryCallback;
        typedef std::function< void(UpnpResource::Ptr) > LostCallback;
        UpnpConnector(DiscoveryCallback discoveryCallback, LostCallback lostCallback);
        virtual ~UpnpConnector();

        void connect();
        void disconnect();

    private:
        DiscoveryCallback m_discoveryCallback;
        LostCallback m_lostCallback;

        void gupnpStart();
        void gupnpStop();

        static void startDiscovery(GUPnPControlPoint *controlPoint);

        // static is necessary for callbacks defined with the c gupnp functions (c code)
        static void onContextAvailable(GUPnPContextManager *manager, GUPnPContext *context);
        static void onDeviceProxyAvailable(GUPnPControlPoint *cp, GUPnPDeviceProxy *proxy, gpointer userData);
        static void onDeviceProxyUnavailable(GUPnPControlPoint *cp, GUPnPDeviceProxy *proxy);
        static void onServiceProxyAvailable(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy);
        static void onServiceProxyUnavailable(GUPnPControlPoint *cp, GUPnPServiceProxy *proxy);
        static int checkRequestQueue(gpointer data);

        static void onIntrospectionAvailable(GUPnPServiceInfo  *serviceInfo,
                                             GUPnPServiceIntrospection *introspection,
                                             const GError              *error,
                                             gpointer                   userContext);
        static void unregisterDeviceResource(string udn);
        static void initResourceCallbackHandler();
};

#endif
