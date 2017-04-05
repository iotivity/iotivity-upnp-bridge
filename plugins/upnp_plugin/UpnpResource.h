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

#ifndef UPNP_RESOURCE_H_
#define UPNP_RESOURCE_H_

#include <Configuration.h>
#include <OCPlatform.h>
#include <gupnp.h>

#include "UpnpInternal.h"

using namespace std;

struct _link {
	string href;
	string rel;
	string rt;
};

class UpnpResource
{
    public:

        typedef std::shared_ptr< UpnpResource > Ptr;

        UpnpResource();
        virtual ~UpnpResource();

        virtual void addLink(UpnpResource::Ptr resource);
        virtual void setLinkAttribute();

        string getResourceType();
        string getUdn();

        bool isRegistered();
        void setRegistered(bool registered);

        void setReady(bool isReady);
        bool isReady();

    //protected:
        std::string m_name;
        std::string m_uri;
        std::string m_resourceType;
        std::string m_interface;
        std::string m_address;
//        CompositeAttribute m_links;
        vector<_link> m_links;
        string m_udn;
        bool m_ready;
        bool m_registered;
};

#endif
