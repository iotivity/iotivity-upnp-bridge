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

#ifndef UPNP_DIMMING_SERVICE_H_
#define UPNP_DIMMING_SERVICE_H_

#include <string>
#include <map>

#include <gupnp.h>

#include "UpnpResource.h"
#include "UpnpInternal.h"
#include "UpnpService.h"

class UpnpDimming: public UpnpService
{
    friend class UpnpService;

public:
    typedef GUPnPServiceProxyAction* (UpnpDimming::*GetAttributeHandler)(UpnpRequest *request);
    typedef GUPnPServiceProxyAction* (UpnpDimming::*SetAttributeHandler)(RCSResourceAttributes::Value&, UpnpRequest *request);

    static vector <UpnpAttribute> Attributes;

    UpnpDimming(GUPnPServiceInfo *serviceInfo,
                UpnpRequestState *requestState):
        UpnpService(serviceInfo, UPNP_OIC_TYPE_BRIGHTNESS, requestState)
    {
    }

    GUPnPServiceProxyAction *getLoadLevel(UpnpRequest *request);
    static void getLoadLevelCb(GUPnPServiceProxy *proxy,
                               GUPnPServiceProxyAction *action,
                               gpointer userData);

    GUPnPServiceProxyAction *setLoadLevel(RCSResourceAttributes::Value& value,
                                          UpnpRequest *request);
    static void setLoadLevelCb(GUPnPServiceProxy *proxy,
                               GUPnPServiceProxyAction *action,
                               gpointer userData);

private:
    static map <const string, pair <GetAttributeHandler, SetAttributeHandler>> AttributeMap;

    bool getAttributesRequest(UpnpRequest *request);
    bool setAttributesRequest(const RCSResourceAttributes &value, UpnpRequest *request);
};

#endif //UPNP_DIMMING_SERVICE_H_
