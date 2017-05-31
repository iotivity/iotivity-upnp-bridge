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

#ifndef UPNP_ATTRIBUTE_H_
#define UPNP_ATTRIBUTE_H_

#include "UpnpInternal.h"
#include "UpnpRequest.h"

class UpnpAttribute
{
    public:
        static void getCb(GUPnPServiceProxy *proxy,
                          GUPnPServiceProxyAction *action,
                          gpointer userData);

        static  bool get(GUPnPServiceProxy *serviceProxy,
                         UpnpRequest *request,
                         UpnpAttributeInfo *attrInfo);

        static void setCb(GUPnPServiceProxy *proxy,
                          GUPnPServiceProxyAction *action,
                          gpointer userData);
};

#endif
