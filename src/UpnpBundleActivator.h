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

#ifndef UPNPBUNDLEACTIVATOR_H_
#define UPNPBUNDLEACTIVATOR_H_

#include <BundleActivator.h>
#include <BundleResource.h>
#include <ResourceContainerBundleAPI.h>

#include "UpnpConnector.h"
#include "UpnpManager.h"
#include "BundleResource.h"

using namespace OIC::Service;

class UpnpBundleActivator: public BundleActivator
{
    public:
        UpnpBundleActivator();
        ~UpnpBundleActivator();
        void activateBundle(ResourceContainerBundleAPI *resourceContainer, std::string bundleId);
        void deactivateBundle();
        void createResource(resourceInfo resourceInfo);
        void destroyResource(BundleResource::Ptr pBundleResource);

    private:
        std::string m_bundleId;
        ResourceContainerBundleAPI *m_pResourceContainer;
        std::vector< BundleResource::Ptr > m_vecResources;

        UpnpConnector *m_connector;

        void connectorDiscoveryCb(UpnpResource::Ptr pBundleResource);
        void connectorLostCb(UpnpResource::Ptr pBundleResource);

};

#endif /* UPNPBUNDLEACTIVATOR_H_ */
