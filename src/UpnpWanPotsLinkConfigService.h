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
// Unless required by applidsl law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef UPNP_WAN_POTS_LINK_CONFIG_SERVICE_H_
#define UPNP_WAN_POTS_LINK_CONFIG_SERVICE_H_

#include <string>
#include <map>

#include <gupnp.h>

#include "UpnpResource.h"
#include "UpnpInternal.h"
#include "UpnpService.h"

using namespace std;

class UpnpWanPotsLinkConfig: public UpnpService
{
        friend class UpnpService;

    public:
        typedef bool (UpnpWanPotsLinkConfig::*GetAttributeHandler)(UpnpRequest *);
        typedef bool (UpnpWanPotsLinkConfig::*SetAttributeHandler)(UpnpRequest *,
                RCSResourceAttributes::Value *);

        UpnpWanPotsLinkConfig(GUPnPServiceInfo *serviceInfo,
                               UpnpRequestState *requestState):
            UpnpService(serviceInfo, UPNP_OIC_TYPE_WAN_POTS_LINK_CONFIG, requestState, &Attributes)
        {
        }

    private:
        static map <const string, GetAttributeHandler> GetAttributeActionMap;
        static map <const string, SetAttributeHandler> SetAttributeActionMap;
        static vector <UpnpAttributeInfo> Attributes;

        bool getAttributesRequest(UpnpRequest *request,
                                  const map< string, string > &queryParams);
        bool setAttributesRequest(const RCSResourceAttributes &attrs,
                                  UpnpRequest *request,
                                  const map< string, string > &queryParams);

        static void getIspInfoCb(GUPnPServiceProxy *proxy,
                                 GUPnPServiceProxyAction *action,
                                 gpointer userData);
        bool getIspInfo(UpnpRequest *request);

        static void setIspInfoCb(GUPnPServiceProxy *proxy,
                                 GUPnPServiceProxyAction *action,
                                 gpointer userData);
        bool setIspInfo(UpnpRequest *request,
                        RCSResourceAttributes::Value *value);

    static void getCallRetryInfoCb(GUPnPServiceProxy *proxy,
                                       GUPnPServiceProxyAction *action,
                                       gpointer userData);
        bool getCallRetryInfo(UpnpRequest *request);

        static void setCallRetryInfoCb(GUPnPServiceProxy *proxy,
                                       GUPnPServiceProxyAction *action,
                                       gpointer userData);
        bool setCallRetryInfo(UpnpRequest *request,
                              RCSResourceAttributes::Value *value);


};

#endif
