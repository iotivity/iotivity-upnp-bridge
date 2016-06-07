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

#ifndef UPNP_RENDERING_CONTROL_H_
#define UPNP_RENDERING_CONTROL_H_

#include <string>
#include <map>

#include <gupnp.h>

#include "UpnpResource.h"
#include "UpnpInternal.h"
#include "UpnpService.h"

using namespace std;

class UpnpRenderingControl: public UpnpService
{
        friend class UpnpService;

    public:
        typedef bool (UpnpRenderingControl::*GetAttributeHandler)(UpnpRequest *, const map< string, string > &);
        typedef bool (UpnpRenderingControl::*SetAttributeHandler)(UpnpRequest *, RCSResourceAttributes::Value *, const map< string, string > &);

        UpnpRenderingControl(GUPnPServiceInfo *serviceInfo, UpnpRequestState *requestState) :
            UpnpService(serviceInfo, UPNP_OIC_TYPE_RENDERING_CONTROL, requestState, &Attributes)
        {
        }

    private:
        static map< const string, UpnpRenderingControl::GetAttributeHandler > GetAttributeActionMap;
        static map< const string, UpnpRenderingControl::SetAttributeHandler > SetAttributeActionMap;

        static vector< UpnpAttributeInfo > Attributes;

        bool getAttributesRequest(UpnpRequest *request, const map< string, string > &queryParams);
        bool setAttributesRequest(const RCSResourceAttributes &attrs, UpnpRequest *request, const map< string, string > &queryParams);

        static void getPresetNameListCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool getPresetNameList(UpnpRequest *request, const map< string, string > &queryParams);

        static void setPresetNameCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool setPresetName(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams);

        static void getMuteCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool getMute(UpnpRequest *request, const map< string, string > &queryParams);

        static void setMuteCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool setMute(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams);

        static void getVolumeCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool getVolume(UpnpRequest *request, const map< string, string > &queryParams);

        static void setVolumeCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer userData);
        bool setVolume(UpnpRequest *request, RCSResourceAttributes::Value *value, const map< string, string > &queryParams);
};

#endif
