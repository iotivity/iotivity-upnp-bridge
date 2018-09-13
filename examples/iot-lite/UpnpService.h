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

#ifndef UPNP_SERVICE_H_
#define UPNP_SERVICE_H_

#include <gupnp.h>

#include "UpnpInternal.h"
#include "UpnpRequest.h"
#include "UpnpResource.h"

using namespace std;

static const string SERVICE_TYPE = "serviceType";

class UpnpService: public UpnpResource
{
    public:
        UpnpService(GUPnPServiceInfo *serviceInfo,
                    string type,
                    UpnpRequestState *requestState);

        virtual ~UpnpService();

        void setProxy(GUPnPServiceProxy *proxy);
        GUPnPServiceProxy *getProxy();

        string getId();
        void stop();

        static void processGetRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data);
        static void processPostRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data);
        static void processPutRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data);

        static void setDeviceIndexForUri(const string uri, int deviceIndex);
        static int getDeviceIndexForUri(const string uri);

    protected:
        GUPnPServiceProxy *m_proxy;
        UpnpRequestState *m_requestState;

    private:

        string m_serviceId;

        static string getStringField(function< char *(GUPnPServiceInfo *serviceInfo)> f,
                                     GUPnPServiceInfo *serviceInfo);
};

#endif
