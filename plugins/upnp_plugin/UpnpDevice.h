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

#ifndef UPNP_DEVICE_H_
#define UPNP_DEVICE_H_

#include <gupnp.h>

#include "UpnpRequest.h"
#include "UpnpResource.h"
#include "UpnpService.h"

using namespace std;

class UpnpDevice: public UpnpResource
{
    public:
        UpnpDevice(GUPnPDeviceInfo *deviceInfo,
                   UpnpRequestState *requestState);

        virtual ~UpnpDevice();

        void initAttributes();

        void insertDevice(string udn);
        void insertService(string id);

        void setProxy(GUPnPDeviceProxy *proxy);
        GUPnPDeviceProxy *getProxy();

        const string getParent();
        void setParent(const string parent);

        std::vector<string> &getDeviceList();
        std::vector<string> &getServiceList();

    private:

        GUPnPDeviceProxy *m_proxy;
        string m_parent;
        string m_deviceType;

        // UDNs of embedded devices
        std::vector<string> m_deviceList;

        // IDs of embedded services
        std::vector<string> m_serviceList;

        UpnpRequestState *m_requestState;

        void initBasicAttributes(GUPnPDeviceInfo *deviceInfo);
        string getStringField(function< char *(GUPnPDeviceInfo *deviceInfo)> f,
                              GUPnPDeviceInfo *deviceInfo);
};

#endif
