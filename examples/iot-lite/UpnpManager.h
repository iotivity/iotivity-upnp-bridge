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

#ifndef UPNP_MANAGER_H_
#define UPNP_MANAGER_H_

#include <gupnp.h>
#include <string>

#include "UpnpResource.h"
#include "UpnpDevice.h"
#include "UpnpService.h"

class UpnpManager
{

    public:
        ~UpnpManager();

        UpnpResource::Ptr processDevice(GUPnPDeviceProxy *proxy,
                                        GUPnPDeviceInfo *info,
                                        bool isRoot,
                                        UpnpRequestState *requestState);

        UpnpResource::Ptr processService(GUPnPServiceProxy *proxy,
                                         GUPnPServiceInfo *info,
                                         GUPnPServiceIntrospection *introspection,
                                         UpnpRequestState *requestState);

        void removeService(GUPnPServiceInfo *info);
        void removeService(string serviceKey);
        void removeDevice(string udn);
        void stop();

        UpnpResource::Ptr findResource(GUPnPServiceInfo *info);
        UpnpResource::Ptr findResource(GUPnPDeviceInfo *info);

        std::shared_ptr<UpnpDevice>  findDevice(std::string udn);
        std::shared_ptr<UpnpService> findService(std::string serviceKey);

        // TODO make this private access it through accessors.
        // Device map, keyed off device UDN
        std::map<std::string, std::shared_ptr<UpnpDevice> > m_devices;
        // Service map, keyed off service ID
        std::map<std::string, std::shared_ptr<UpnpService> > m_services;

    private:
        std::shared_ptr<UpnpDevice> addDevice(GUPnPDeviceInfo *info,
                                              const string parent,
                                              UpnpRequestState *requestState);

        const std::string generateServiceKey(GUPnPServiceInfo *info);
        std::shared_ptr<UpnpService> findService(GUPnPServiceInfo *info);
        std::shared_ptr<UpnpService> generateService(GUPnPServiceInfo *serviceInfo,
                UpnpRequestState *requestState);
};

#endif
