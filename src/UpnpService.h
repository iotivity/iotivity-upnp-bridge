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

#ifndef UPNP_SERVICE_H_
#define UPNP_SERVICE_H_

#include <gupnp.h>

#include "UpnpAttribute.h"
#include "UpnpInternal.h"
#include "UpnpRequest.h"
#include "UpnpResource.h"

using namespace std;
using namespace OIC::Service;

class UpnpService: public UpnpResource
{
public:
    UpnpService(GUPnPServiceInfo *serviceInfo,
                string type,
                UpnpRequestState *requestState);

    virtual ~UpnpService();

    virtual void initAttributes();

    virtual void handleSetAttributesRequest(const RCSResourceAttributes &attrs);

    virtual RCSResourceAttributes handleGetAttributesRequest();

    void setProxy(GUPnPServiceProxy *proxy);
    GUPnPServiceProxy* getProxy();

    virtual void processIntrospection(GUPnPServiceProxy *proxy, GUPnPServiceIntrospection *introspection);

    virtual bool getAttributesRequest(UpnpRequest *request) = 0;

    virtual bool setAttributesRequest(const RCSResourceAttributes &value, UpnpRequest *request) = 0;

    string getId();

protected:
    // Map of associated attributes (OIC)
    // "OCF Attribute name" -> (attribute info, supported operations)
    map <string, pair <UpnpAttributeInfo*, int>> m_attributeMap;

    GUPnPServiceProxy *m_proxy;

    UpnpRequestState *m_requestState;

private:

    string m_serviceId;

    // Mapping of UPnP state variables that are observed/notified to
    // corresponding OCF attributes
    map <const char*, pair <string, GType>> m_stateVarMap;

    static string getStringField(function< char*(GUPnPServiceInfo *serviceInfo)> f, GUPnPServiceInfo *serviceInfo);

    static void onStateChanged(GUPnPServiceProxy *proxy,
                               const char *varName,
                               GValue *value,
                               gpointer userData);
};

#endif
