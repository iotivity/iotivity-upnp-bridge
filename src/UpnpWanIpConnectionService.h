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

#ifndef UPNP_WAN_IP_CONNECTION_SERVICE_H_
#define UPNP_WAN_IP_CONNECTION_SERVICE_H_

#include <string>
#include <map>

#include <gupnp.h>

#include "UpnpResource.h"
#include "UpnpInternal.h"
#include "UpnpService.h"

using namespace std;

class UpnpWanIpConnection: public UpnpService
{
        friend class UpnpService;

    public:
        typedef bool (UpnpWanIpConnection::*GetAttributeHandler)(UpnpRequest *);

        typedef bool (UpnpWanIpConnection::*SetAttributeHandler)(UpnpRequest *,
                RCSResourceAttributes::Value *);

        UpnpWanIpConnection(GUPnPServiceInfo *serviceInfo,
                            UpnpRequestState *requestState):
            UpnpService(serviceInfo, UPNP_OIC_TYPE_WAN_IP_CONNECTION, requestState, &Attributes)
        {
            m_sizePortMap = 0;
        }

    private:
        static map <const string, GetAttributeHandler> GetAttributeActionMap;
        static map <const string, SetAttributeHandler> SetAttributeActionMap;
        static vector <UpnpAttributeInfo> Attributes;
        int m_sizePortMap;

        bool getAttributesRequest(UpnpRequest *request,
                                  const map< string, string > &queryParams);
        bool setAttributesRequest(const RCSResourceAttributes &attrs,
                                  UpnpRequest *request,
                                  const map< string, string > &queryParams);
        bool processNotification(string attrName,
                                 string parent,
                                 GValue *value);

        static void getNatStatusCb(GUPnPServiceProxy *proxy,
                                   GUPnPServiceProxyAction *action,
                                   gpointer userData);

        bool getNatStatus(UpnpRequest *request);

        static void getConnectionTypeInfoCb(GUPnPServiceProxy *proxy,
                                            GUPnPServiceProxyAction *action,
                                            gpointer userData);

        bool getConnectionTypeInfo(UpnpRequest *request);

        static void setConnectionTypeInfoCb(GUPnPServiceProxy *proxy,
                                            GUPnPServiceProxyAction *action,
                                            gpointer userData);

        bool setConnectionTypeInfo(UpnpRequest *request,
                                   RCSResourceAttributes::Value *value);
};

#endif
