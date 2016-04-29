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

#include "UpnpException.h"
#include "UpnpHelper.h"
#include "UpnpInternal.h"
#include "UpnpManager.h"

#include "UpnpDimmingService.h"
#include "UpnpPowerSwitchService.h"
#include "UpnpContentDirectoryService.h"
#include "UpnpAVTransportService.h"

#define nestedAtrribute std::vector<std::vector<RCSResourceAttributes>>

using namespace std;

static const string MODULE = "UpnpManager";

UpnpManager::UpnpManager()
{
    m_devices.clear();
    m_services.clear();
 }

UpnpManager::~UpnpManager()
{
    m_devices.clear();
    m_services.clear();
}

UpnpResource::Ptr UpnpManager::processDevice(GUPnPDeviceProxy *proxy,
                                             GUPnPDeviceInfo *deviceInfo,
                                             bool isRoot,
                                             UpnpRequestState *requestState)
{
    const string udn = gupnp_device_info_get_udn(deviceInfo);
    std::shared_ptr<UpnpDevice> pDevice = findDevice(udn);

    if (pDevice == nullptr)
    {
        if (isRoot)
        {
            pDevice = addDevice(deviceInfo, UPNP_ROOT_DEVICE, requestState);
        } else {
            DEBUG_PRINT(udn << " is not root and does not have a parent");
            pDevice = addDevice(deviceInfo, "", requestState);
        }
    }

    if (pDevice != nullptr)
    {
        pDevice->setProxy(proxy);
        pDevice->setReady(true);
    }
    return pDevice;
}

// This is a recursive call. Should be safe as the depth of a UPNP device
// tree rarely is going to exceed 3 levels. If we ever discover that
// this is not the case, unravel the call into iterative function.
//
// We keep the device lookup in manager, inside a map as <udn, device object reference> pair
std::shared_ptr<UpnpDevice> UpnpManager::addDevice(GUPnPDeviceInfo *deviceInfo,
                                                   const string parent,
                                                   UpnpRequestState *requestState)
{
    const string udn = gupnp_device_info_get_udn(deviceInfo);
    std::shared_ptr<UpnpDevice> pDevice;

    DEBUG_PRINT(udn);

    // Create a new UpnpDevice Object
    try {
        pDevice = std::make_shared < UpnpDevice > (deviceInfo, requestState);
    } catch (exception& e) {
        ERROR_PRINT("What(): " << e.what());
        return nullptr;
    }

    pDevice->setParent(parent);

    // Populate UpnpDeviceObject::deviceList from gupnp_device_list_devices
    // Populate UpnpDeviceObject::serviceList from gupnp_device_list_devices
    // Set the attributes:
    //   "n" - device name
    //   "id"
    //   "resources" {
    //               {"href": dev_1_uuid, "rel": contains, "rt" : dev_1_type, "if": "oic.if.b oic.if...."},
    //                ...,
    //               {"href": 1_uuid, "rel": contains, "rt" : service_type, "if": "oic.if.b oic.if...."}
    //
    // Add to DeviceMap
    // Register resource

    // Check if there are embedded devices
    GList *childDev = gupnp_device_info_list_devices (deviceInfo);

    while (childDev)
    {
        try {
            GUPnPDeviceInfo *info = GUPNP_DEVICE_INFO (childDev->data);
            const string udnChild = gupnp_device_info_get_udn(info);
            std::shared_ptr<UpnpDevice> pChildDev = findDevice(udnChild);
            if (pChildDev != nullptr)
            {
                // The embedded device proxy has been discovered previously and
                // the corresponding device resource should be already registered with the bundle.
                DEBUG_PRINT("previously discovered child device " << udnChild << " to the tree");
            }
            else
            {
                // Never seen before embedded device
                DEBUG_PRINT("new child device " << udnChild << " to the tree");
                pChildDev = addDevice(info, udnChild, requestState);
            }

            if (pChildDev != nullptr)
            {
                // Add to list of embedded devices
                pDevice->insertDevice(udnChild);

                // Add link to "links" attribute map
                pDevice->addLink(pChildDev);
            }
        } catch (exception& e) {
            ERROR_PRINT("What(): " << e.what());
        }

        g_object_unref (childDev->data);
        childDev = g_list_delete_link (childDev, childDev);
    }

    // Add to map of devices
    m_devices[udn]  = pDevice;

    // Check if there are embedded services
    GList *childService = gupnp_device_info_list_services (deviceInfo);

    while (childService)
    {
        try {
            DEBUG_PRINT("Add service description");
            GUPnPServiceInfo *serviceInfo = GUPNP_SERVICE_INFO (childService->data);
            std::shared_ptr<UpnpService> pService = findService(serviceInfo);

            if (pService != nullptr)
            {
                // The service proxy has been discovered previously and
                // the corresponding service resource is now ready to be registered with the bundle.
                pService->setReady(true);
            }
            else
            {
                // Never seen before service
                pService = generateService(serviceInfo, requestState);
            }

            // Add to list of embedded services
            pDevice->insertService(pService->getId());

            // Add to the manager's map of services
            m_services[udn + pService->getId()] = pService;

            // Add link to "links" attribute map
            pDevice->addLink(pService);
        } catch(NotImplementedException& e) {
            ERROR_PRINT("What(): " << e.what());
        } catch(BadUriException& e) {
            ERROR_PRINT("What(): " << e.what());
        }

        g_object_unref (childService->data);
        childService = g_list_delete_link (childService, childService);
    }

    // Finalize "links" attribute
    pDevice->setLinkAttribute();

    return pDevice;

}

// We keep the service lookup inside a map as <udn + service ID, service object reference> pair
UpnpResource::Ptr UpnpManager::processService(GUPnPServiceProxy *proxy,
                                              GUPnPServiceInfo *serviceInfo,
                                              GUPnPServiceIntrospection *introspection,
                                              UpnpRequestState *requestState)
{
    const string udn = gupnp_service_info_get_udn(serviceInfo);
    DEBUG_PRINT("type: " << gupnp_service_info_get_service_type(serviceInfo) << ", Udn: " << udn);

    std::shared_ptr<UpnpService> pService = findService(serviceInfo);

    if (pService == nullptr)
    {
        // Never seen before service
        try {
            pService = generateService(serviceInfo, requestState);
        } catch(NotImplementedException& e) {
            ERROR_PRINT("What(): " << e.what());
            return nullptr;
        } catch(BadUriException& e) {
            ERROR_PRINT("What(): " << e.what());
            return nullptr;
        }
    }

    if (introspection != NULL)
    {
        pService->processIntrospection(proxy, introspection);
    }

    pService->setProxy(proxy);
    pService->setReady(true);

    // Add to the manager's map of services
    m_services[udn + pService->getId()]  = pService;

    return pService;
}

void UpnpManager::removeService(GUPnPServiceInfo *info)
{
    DEBUG_PRINT("type: " << gupnp_service_info_get_service_type(info));
    const string serviceKey = generateServiceKey(info);

    if (serviceKey != "") {
        std::map< string, shared_ptr<UpnpService> >::iterator it = m_services.find(serviceKey);

        if (it != m_services.end())
        {
            m_services.erase(it);
        }
    }
}

void UpnpManager::removeService(string serviceKey)
{
    DEBUG_PRINT("key = " << serviceKey);

    std::map< string, shared_ptr<UpnpService> >::iterator it = m_services.find(serviceKey);

    if (it != m_services.end())
    {
        m_services.erase(it);
    }
}

void UpnpManager::removeDevice(string udn)
{
    DEBUG_PRINT("udn = " << udn);

    std::map< string, shared_ptr<UpnpDevice> >::iterator it = m_devices.find(udn);

    if (it != m_devices.end())
    {
        m_devices.erase(it);
    }
}

UpnpResource::Ptr UpnpManager::findResource(GUPnPServiceInfo *info)
{
    return findService(info);
}

UpnpResource::Ptr UpnpManager::findResource(GUPnPDeviceInfo *info)
{
    return findDevice(gupnp_device_info_get_udn(info));
}

std::shared_ptr<UpnpService>  UpnpManager::findService(std::string serviceKey)
{
    std::map< string, shared_ptr<UpnpService> >::iterator it = m_services.find(serviceKey);

    if (it != m_services.end())
    {
        return it->second;
    }
    return nullptr;

}

shared_ptr<UpnpDevice> UpnpManager::findDevice(string udn)
{

    std::map< string, shared_ptr<UpnpDevice> >::iterator it = m_devices.find(udn);

    if (it != m_devices.end())
    {
        return it->second;
    }

    return nullptr;
}

const string UpnpManager::generateServiceKey(GUPnPServiceInfo* info)
{
    // Extract service ID
    char *c_field = gupnp_service_info_get_id(info);
    if (c_field != NULL) {

        const string udn = gupnp_service_info_get_udn(info);
        string id = string(c_field);
        const string serviceKey = udn + id;

        g_free(c_field);
        return serviceKey;
    }

    return "";
}

std::shared_ptr<UpnpService> UpnpManager::findService (GUPnPServiceInfo* info)
{
    const string serviceKey = generateServiceKey(info);
    if (serviceKey != "")
    {
        return findService(serviceKey);
    }

    return nullptr;
}

std::shared_ptr<UpnpService>  UpnpManager::generateService(GUPnPServiceInfo *serviceInfo,
                                                           UpnpRequestState *requestState)
{
    // Service type
    string serviceType = gupnp_service_info_get_service_type(serviceInfo);
    string resourceType = findResourceType(serviceType);

    if (resourceType == UPNP_OIC_TYPE_POWER_SWITCH) {
        return (std::make_shared < UpnpPowerSwitch > (serviceInfo, requestState));
    }
    else if(resourceType == UPNP_OIC_TYPE_BRIGHTNESS) {
        return (std::make_shared < UpnpDimming > (serviceInfo, requestState));
    }
    else if(resourceType == UPNP_OIC_TYPE_CONTENT_DIRECTORY) {
        return (std::make_shared < UpnpContentDirectory > (serviceInfo, requestState));
    }
    else if(resourceType == UPNP_OIC_TYPE_AV_TRANSPORT) {
        return (std::make_shared < UpnpAVTransport > (serviceInfo, requestState));
    }
    else {
        //throw an exception
        ERROR_PRINT("Service type " << serviceType << " not implemented!");
        throw NotImplementedException("UpnpService::ctor: Service " + serviceType + " not implemented!");
        return nullptr;
    }
}
