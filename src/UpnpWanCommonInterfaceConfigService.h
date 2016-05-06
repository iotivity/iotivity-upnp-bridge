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

#ifndef UPNP_COMMON_INTERFACE_CONFIG_SERVICE_H_
#define UPNP_COMMON_INTERFACE_CONFIG_SERVICE_H_

#include <string>
#include <map>

#include <gupnp.h>

#include "UpnpResource.h"
#include "UpnpInternal.h"
#include "UpnpService.h"

using namespace std;

class UpnpWanCommonInterfaceConfig: public UpnpService
{
    friend class UpnpService;

public:
    typedef bool (UpnpWanCommonInterfaceConfig::*GetAttributeHandler)(UpnpRequest *);

    typedef bool (UpnpWanCommonInterfaceConfig::*SetAttributeHandler)(GUPnPServiceProxy *,
                                                                      UpnpRequest *,
                                                                      UpnpAttributeInfo *,
                                                                      RCSResourceAttributes);

    UpnpWanCommonInterfaceConfig(GUPnPServiceInfo *serviceInfo,
                    UpnpRequestState *requestState):
        UpnpService(serviceInfo, UPNP_OIC_TYPE_WAN_IF_CONFIG, requestState, &Attributes)
    {
        m_numConnections = 0;
    }

private:
    int m_numConnections;
    map <UpnpRequest *, CompositeAttribute> m_ConnectionInfoRequestMap;

    static map <const string, GetAttributeHandler> GetAttributeActionMap;

    static vector <UpnpAttributeInfo> Attributes;

    bool getAttributesRequest(UpnpRequest *request);
    bool setAttributesRequest(const RCSResourceAttributes& attrs, UpnpRequest *request);
    bool processNotification(string attrName, string parent, GValue *value);

    static void getLinkPropertiesCb(GUPnPServiceProxy *proxy,
                                    GUPnPServiceProxyAction *action,
                                    gpointer userData);

    bool getLinkProperties(UpnpRequest *request);

    static void getConnectionInfoCb(GUPnPServiceProxy *proxy,
                                    GUPnPServiceProxyAction *action,
                                    gpointer userData);

    bool getConnectionInfo(UpnpRequest *request);

};

#endif
