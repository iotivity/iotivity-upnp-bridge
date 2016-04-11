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

#include <cstdint>

#include "UpnpBundleActivator.h"
#include "UpnpConstants.h"
#include "UpnpManager.h"

static const string MODULE = "UpnpBundleActivator";

UpnpBundleActivator *g_upnpBundle;

UpnpBundleActivator::UpnpBundleActivator()
{
    printf("Started UpnpBundleActivator\n");
    m_pResourceContainer = nullptr;
    m_connector = nullptr;
}

UpnpBundleActivator::~UpnpBundleActivator()
{
    DEBUG_PRINT("Stopping UpnpBundleActivator");
    m_pResourceContainer = nullptr;
    m_connector = nullptr;
}

/*
 This method gets called back from the connector whenever a resource is
 discovered on the bridged protocol
 */
void UpnpBundleActivator::connectorDiscoveryCb(UpnpResource::Ptr pUpnpResource)
{
    pUpnpResource->m_bundleId = m_bundleId;

    DEBUG_PRINT("UpnpResource URI " << pUpnpResource->m_uri);
    m_pResourceContainer->registerResource(pUpnpResource);
    m_vecResources.push_back(pUpnpResource);
}

/*
 This method gets called back from the connector whenever a resource is
 lost on the bridged protocol
 */
void UpnpBundleActivator::connectorLostCb(UpnpResource::Ptr pUpnpResource)
{
    destroyResource(pUpnpResource);
}

void UpnpBundleActivator::activateBundle(ResourceContainerBundleAPI *resourceContainer,
        std::string bundleId)
{
    m_pResourceContainer = resourceContainer;
    m_bundleId = bundleId;
    UpnpConnector::DiscoveryCallback discoveryCb = std::bind(
            &UpnpBundleActivator::connectorDiscoveryCb, this, std::placeholders::_1);
    UpnpConnector::LostCallback lostCb = std::bind(&UpnpBundleActivator::connectorLostCb, this,
            std::placeholders::_1);

    m_connector = new UpnpConnector(discoveryCb, lostCb);
    m_connector->connect();
}

void UpnpBundleActivator::deactivateBundle()
{
    DEBUG_PRINT("");
    std::vector< BundleResource::Ptr >::iterator itor;
    for (itor = m_vecResources.begin(); itor != m_vecResources.end(); ++itor)
    {
        DEBUG_PRINT((*itor)->m_uri);
        m_pResourceContainer->unregisterResource(*itor);
    }
    m_vecResources.clear();
    m_connector->disconnect();
    delete m_connector;
}

void UpnpBundleActivator::createResource(resourceInfo resourceInfo)
{

}

void UpnpBundleActivator::destroyResource(BundleResource::Ptr pBundleResource)
{
    DEBUG_PRINT(pBundleResource->m_uri);
    std::vector< BundleResource::Ptr >::iterator itor;
    itor = std::find(m_vecResources.begin(), m_vecResources.end(), pBundleResource);
    if (itor != m_vecResources.end())
    {
        m_pResourceContainer->unregisterResource(pBundleResource);
        m_vecResources.erase(itor);
    }
}

#define DLL_PUBLIC __attribute__((__visibility__("default")))

extern "C" DLL_PUBLIC void upnp_externalActivateBundle(
        ResourceContainerBundleAPI *resourceContainer, std::string bundleId)
{
    g_upnpBundle = new UpnpBundleActivator();
    g_upnpBundle->activateBundle(resourceContainer, bundleId);
}

extern "C" DLL_PUBLIC void upnp_externalDeactivateBundle()
{
    g_upnpBundle->deactivateBundle();
    delete g_upnpBundle;
}

extern "C" DLL_PUBLIC void upnp_externalCreateResource(resourceInfo resourceInfo)
{
    g_upnpBundle->createResource(resourceInfo);
}

extern "C" DLL_PUBLIC void upnp_externalDestroyResource(BundleResource::Ptr pBundleResource)
{
    g_upnpBundle->destroyResource(pBundleResource);
}
