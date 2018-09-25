//******************************************************************
//
// Copyright 2018 Intel Corporation All Rights Reserved.
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

#include <memory>
#include <vector>

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

        string getName();
        string getUri();
        string getResourceType();
        string getInterface();
        string getUdn();

        bool isRegistered();
        void setRegistered(bool registered);

        bool isReady();
        void setReady(bool isReady);


    protected:
        vector<_link> m_links;
        string m_name;
        string m_uri;
        string m_resourceType;
        string m_interface;
        string m_udn;
        bool m_ready;
        bool m_registered;
};

#endif
