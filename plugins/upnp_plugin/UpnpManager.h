#ifndef UPNP_MANAGER_H_
#define UPNP_MANAGER_H_

#include <gupnp.h>
#include <functional>
#include <string>

#include "UpnpResource.h"
#include "UpnpDevice.h"
#include "UpnpService.h"

class UpnpManager
{

    public:
        ~UpnpManager();

        UpnpResource::Ptr processDevice(GUPnPDeviceProxy *proxy,
                                        GUPnPDeviceInfo *info,
                                        bool isRoot,
                                        UpnpRequestState *requestState);

        UpnpResource::Ptr processService(GUPnPServiceProxy *proxy,
                                         GUPnPServiceInfo *info,
                                         GUPnPServiceIntrospection *introspection,
                                         UpnpRequestState *requestState);

        void removeService(GUPnPServiceInfo *info);
        void removeService(string serviceKey);
        void removeDevice(string udn);
        void stop();


        void onScan();

        UpnpResource::Ptr findResource(GUPnPServiceInfo *info);
        UpnpResource::Ptr findResource(GUPnPDeviceInfo *info);

        std::shared_ptr<UpnpDevice>  findDevice(std::string udn);
        std::shared_ptr<UpnpService> findService(std::string serviceKey);

        // TODO make this private access it through accessors.
        // Device map, keyed off device UDN
        std::map<std::string, std::shared_ptr<UpnpDevice> > m_devices;
        // Service map, keyed off service ID
        std::map<std::string, std::shared_ptr<UpnpService> > m_services;

    private:

        std::shared_ptr<UpnpDevice> addDevice(GUPnPDeviceInfo *info,
                                              const string parent,
                                              UpnpRequestState *requestState);

        const std::string generateServiceKey(GUPnPServiceInfo *info);
        std::shared_ptr<UpnpService> findService(GUPnPServiceInfo *info);
        std::shared_ptr<UpnpService>  generateService(GUPnPServiceInfo *serviceInfo,
                UpnpRequestState *requestState);
};

#endif
