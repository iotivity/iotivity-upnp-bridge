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

#ifndef UPNP_WAN_PPP_CONNECTION_SERVICE_H_
#define UPNP_WAN_PPP_CONNECTION_SERVICE_H_

#include <string>
#include <map>

#include <gupnp.h>

#include "UpnpResource.h"
#include "UpnpInternal.h"
#include "UpnpService.h"

using namespace std;

class UpnpWanPppConnection: public UpnpService
{
        friend class UpnpService;

    public:
        typedef bool (UpnpWanPppConnection::*GetAttributeHandler)(UpnpRequest *);

        typedef bool (UpnpWanPppConnection::*SetAttributeHandler)(UpnpRequest *,
                RCSResourceAttributes::Value *);

        UpnpWanPppConnection(GUPnPServiceInfo *serviceInfo,
                            UpnpRequestState *requestState):
            UpnpService(serviceInfo, UPNP_OIC_TYPE_WAN_PPP_CONNECTION, requestState, &Attributes)
        {
            m_sizePortMap = 0;
        }

    private:
        static map <const string, GetAttributeHandler> GetAttributeActionMap;
        static map <const string, SetAttributeHandler> SetAttributeActionMap;
        static vector <UpnpAttributeInfo> Attributes;
        static vector <const char *> statusUpdateActions;

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

        static void getMaxBitRateCb(GUPnPServiceProxy *proxy,
                                    GUPnPServiceProxyAction *action,
                                    gpointer userData);

        bool getMaxBitRate(UpnpRequest *request);

        static void getConnectionTypeInfoCb(GUPnPServiceProxy *proxy,
                                            GUPnPServiceProxyAction *action,
                                            gpointer userData);

        bool getConnectionTypeInfo(UpnpRequest *request);

        static void getStatusInfoCb(GUPnPServiceProxy *proxy,
                                    GUPnPServiceProxyAction *action,
                                    gpointer userData);

        bool getStatusInfo(UpnpRequest *request);

        static void setConnectionTypeInfoCb(GUPnPServiceProxy *proxy,
                                            GUPnPServiceProxyAction *action,
                                            gpointer userData);

        bool setConnectionTypeInfo(UpnpRequest *request,
                                   RCSResourceAttributes::Value *value);

        static void configureConnectionCb(GUPnPServiceProxy *proxy,
                                          GUPnPServiceProxyAction *action,
                                          gpointer userData);

        bool configureConnection(UpnpRequest *request,
                                 RCSResourceAttributes::Value *value);

        static void changeConnectionStatusCb(GUPnPServiceProxy *proxy,
                                             GUPnPServiceProxyAction *action,
                                             gpointer userData);

        bool changeConnectionStatus(UpnpRequest *request,
                                    RCSResourceAttributes::Value *value);

};

#endif
