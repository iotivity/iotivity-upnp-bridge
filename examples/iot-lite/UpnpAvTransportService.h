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

#ifndef UPNP_AV_TRANSPORT_SERVICE_H_
#define UPNP_AV_TRANSPORT_SERVICE_H_

#include <string>
#include <map>

#include <gupnp.h>

#include "UpnpResource.h"
#include "UpnpInternal.h"
#include "UpnpService.h"

using namespace std;

class UpnpAvTransport: public UpnpService
{

    public:
        UpnpAvTransport(GUPnPServiceInfo *serviceInfo,
                        UpnpRequestState *requestState):
            UpnpService(serviceInfo, UPNP_OIC_TYPE_MEDIA_CONTROL, requestState)
        {
        }

        static void processGetRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data);
        static void processPostRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data);
        static void processPutRequest(oc_request_t *request, oc_interface_mask_t interface, void *user_data);

    private:

};

#endif // UPNP_AV_TRANSPORT_SERVICE_H_
