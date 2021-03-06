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

#ifndef UPNP_LAYER3_FORWARDING_SERVICE_H_
#define UPNP_LAYER3_FORWARDING_SERVICE_H_

#include <string>
#include <map>

#include <gupnp.h>

#include "UpnpResource.h"
#include "UpnpInternal.h"
#include "UpnpService.h"

using namespace std;

class UpnpLayer3Forwarding: public UpnpService
{
        friend class UpnpService;

    public:
        typedef GUPnPServiceProxyAction *(UpnpLayer3Forwarding::*GetAttributeHandler)(GUPnPServiceProxy *,
                UpnpRequest *,
                UpnpAttributeInfo *);

        typedef GUPnPServiceProxyAction *(UpnpLayer3Forwarding::*SetAttributeHandler)(GUPnPServiceProxy *,
                UpnpRequest *,
                UpnpAttributeInfo *,
                RCSResourceAttributes);

        UpnpLayer3Forwarding(GUPnPServiceInfo *serviceInfo,
                             UpnpRequestState *requestState):
            UpnpService(serviceInfo, UPNP_OIC_TYPE_LAYER3_FORWARDING, requestState, &Attributes)
        {
        }

    private:
        static vector <UpnpAttributeInfo> Attributes;

        bool getAttributesRequest(UpnpRequest *request,
                                  const map< string, string > &queryParams);
        bool setAttributesRequest(const RCSResourceAttributes &attrs,
                                  UpnpRequest *request,
                                  const map< string, string > &queryParams);

};

#endif
