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

#ifndef UPNP_AV_TRANSPORT_SERVICE_H_
#define UPNP_AV_TRANSPORT_SERVICE_H_

#include <string>
#include <map>

#include <gupnp.h>

#include "UpnpResource.h"
#include "UpnpInternal.h"
#include "UpnpService.h"

using namespace std;

class UpnpAVTransport: public UpnpService
{
        friend class UpnpService;

    public:
    typedef bool (UpnpAVTransport::*GetAttributeHandler)(UpnpRequest *, const map< string, string > &);
    typedef bool (UpnpAVTransport::*SetAttributeHandler)(UpnpRequest *, RCSResourceAttributes::Value *, const map< string, string > &);

        UpnpAVTransport(GUPnPServiceInfo *serviceInfo, UpnpRequestState *requestState) :
            UpnpService(serviceInfo, UPNP_OIC_TYPE_AV_TRANSPORT, requestState, &Attributes)
        {
        }

    private:
        static map< const string, UpnpAVTransport::GetAttributeHandler > GetAttributeActionMap;
        static map< const string, UpnpAVTransport::SetAttributeHandler > SetAttributeActionMap;

        static vector< UpnpAttributeInfo > Attributes;

        bool getAttributesRequest(UpnpRequest *request, const map< string, string > &queryParams);
        bool setAttributesRequest(const RCSResourceAttributes &attrs, UpnpRequest *request, const map< string, string > &queryParams);

        static void getCurrentTransportActionsCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool getCurrentTransportActions(UpnpRequest *request, const map< string, string > &queryParams);

        static void getDeviceCapabilitiesCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool getDeviceCapabilities(UpnpRequest *request, const map< string, string > &queryParams);

        static void getMediaInfoCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool getMediaInfo(UpnpRequest *request, const map< string, string > &queryParams);

        static void getPositionInfoCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool getPositionInfo(UpnpRequest *request, const map< string, string > &queryParams);

        static void getTransportInfoCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool getTransportInfo(UpnpRequest *request, const map< string, string > &queryParams);

        static void getTransportSettingsCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool getTransportSettings(UpnpRequest *request, const map< string, string > &queryParams);

        static void setAvTransportUriCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool setAvTransportUri(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams);

        static void setNextAvTransportUriCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool setNextAvTransportUri(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams);

        static void setPlayModeCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool setPlayMode(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams);

        static void nextCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool next(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams);

        static void pauseCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool pause(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams);

        static void playCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool play(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams);

        static void previousCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool previous(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams);

        static void seekCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool seek(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams);

        static void stopCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool stop(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams);
};

#endif
