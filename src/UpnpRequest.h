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

#ifndef UPNP_REQUEST_H_
#define UPNP_REQUEST_H_

#include <functional>
#include <mutex>
#include <queue>

#include <glib.h>

#include "UpnpInternal.h"
#include "UpnpResource.h"

class UpnpRequest
{
    public:
        std::function< bool() > start;
        std::function< void(bool) > finish;
        int expected;
        int done;

        UpnpResource *resource;
        // We have to keep attribute info and (optional) set value(s)
        std::map <GUPnPServiceProxyAction *, std::pair <UpnpAttributeInfo *, std::vector <UpnpVar> > >
        proxyMap;

        static void requestDone (UpnpRequest *request, bool status)
        {
            request->done++;
            if (request->done == request->expected)
            {
                request->proxyMap.clear();
                request->finish(status);
            }
        }
};

typedef struct _UpnpRequestState
{
    GSource *source;
    guint sourceId;
    GMainContext *context;

    std::queue< UpnpRequest * > requestQueue;
    std::mutex queueLock;
} UpnpRequestState;
#endif
