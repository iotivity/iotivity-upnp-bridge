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

#include <octypes.h>
#include <ocstack.h>
#include <oic_malloc.h>
#include <mpmErrorCode.h>
#include <ConcurrentIotivityUtils.h>

#include "UpnpAttribute.h"
#include "UpnpInternal.h"
#include "UpnpRequest.h"
#include "UpnpResource.h"

using namespace std;

class UpnpService: public UpnpResource
{
public:
    UpnpService(GUPnPServiceInfo *serviceInfo,
                string type,
                UpnpRequestState *requestState,
                vector <UpnpAttributeInfo> *attributeInfo);

    virtual ~UpnpService();

    void setProxy(GUPnPServiceProxy *proxy);
    GUPnPServiceProxy *getProxy();

    string getId();
    void stop();

    virtual OCEntityHandlerResult processGetRequest(string uri, OCRepPayload *payload, string resourceType);
    virtual OCEntityHandlerResult processPutRequest(OCEntityHandlerRequest *ehRequest,
            string uri, string resourceType, OCRepPayload *payload);


protected:
       // Map of associated attributes (OIC)
       // "OCF Attribute name" -> (attribute info, supported operations)
       map <string, pair <UpnpAttributeInfo *, int>> m_attributeMap;

       vector <UpnpAttributeInfo> *m_serviceAttributeInfo;

       GUPnPServiceProxy *m_proxy;

       UpnpRequestState *m_requestState;

private:

    string m_serviceId;

    typedef struct _StateVarAttr
    {
        string attrName;
        GType type;
        string parentName;
    } StateVarAttr;

    // Mapping of UPnP state variables that are observed/notified to
    // corresponding OCF attributes
    map <string, StateVarAttr> m_stateVarMap;

    static string getStringField(function< char *(GUPnPServiceInfo *serviceInfo)> f,
                                 GUPnPServiceInfo *serviceInfo);
};

#endif
