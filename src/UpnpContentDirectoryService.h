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

#ifndef UPNP_CONTENT_DIRECTORY_H_
#define UPNP_CONTENT_DIRECTORY_H_

#include <string>
#include <map>

#include <gupnp.h>

#include "UpnpResource.h"
#include "UpnpInternal.h"
#include "UpnpService.h"

using namespace std;

class UpnpContentDirectory: public UpnpService
{
        friend class UpnpService;

    public:
        typedef bool (UpnpContentDirectory::*GetAttributeHandler)(UpnpRequest *, const map< string, string > &);

        UpnpContentDirectory(GUPnPServiceInfo *serviceInfo, UpnpRequestState *requestState) :
            UpnpService(serviceInfo, UPNP_OIC_TYPE_CONTENT_DIRECTORY, requestState, &Attributes)
        {
        }

    private:
        static map< const string, UpnpContentDirectory::GetAttributeHandler > GetAttributeActionMap;

        static vector< UpnpAttributeInfo > Attributes;

        bool getAttributesRequest(UpnpRequest *request,
                                  const map< string, string > &queryParams);
        bool setAttributesRequest(const RCSResourceAttributes &attrs,
                                  UpnpRequest *request,
                                  const map< string, string > &queryParams);

        static void getBrowseResultCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action,
                                      gpointer userData);

        bool getBrowseResult(UpnpRequest *request, const map< string, string > &queryParams);

        static void getSearchResultCb(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action,
                                      gpointer userData);

        bool getSearchResult(UpnpRequest *request, const map< string, string > &queryParams);
};

#endif
