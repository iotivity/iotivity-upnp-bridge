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

#ifndef LANHOSTCONFIGMANAGEMENT_H_
#define LANHOSTCONFIGMANAGEMENT_H_


#include <OCResource.h>
#include <OCRepresentation.h>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <string>

class LANHostConfigManagement
{
    public:
        LANHostConfigManagement();
        LANHostConfigManagement(std::shared_ptr<OC::OCResource> resource);
        virtual ~LANHostConfigManagement();
        LANHostConfigManagement( const LANHostConfigManagement &other );
        LANHostConfigManagement &operator=(const LANHostConfigManagement &other);
        //Overloaded operator used for putting into a 'set'
        bool operator<(const LANHostConfigManagement &other) const;

        struct AddressRange
        {
                std::string minAddr;
                std::string maxAddr;
        };
        void setSubnetMask(std::string subnetMask);
        std::string getSubnetMask();

        void setIPRouter(std::string ipRouter);
        std::string getIPRoutersList();

        void setDomainName(std::string domanName);
        std::string getDomainName();

        void setAddressRange(const AddressRange& addressRange);
        AddressRange getAddressRange();

        void setReservedAddress(std::string reservedAddress);
        std::string getReservedAddresses();

        void setDNSServer(std::string dnsServer);
        std::string getDNSServers();

        void setDHCPServerConfigurable(bool isConfigurable);
        bool getDHCPServerConfigurable();

        void setDHCPRelay(bool dhcpRelay);
        bool getDHCPRelay();

        const std::shared_ptr<OC::OCResource> getResource() const {return m_resource;}
    private:
        void onSetSubnetMask(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetSubnetMask(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onSetIPRouter(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetIPRoutersList(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onSetDomainName(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetDomainName(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onSetAddressRange(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetAddressRange(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onSetReservedAddress(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetReservedAddresses(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onSetDNSServer(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetDNSServers(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onSetDHCPServerConfigurable(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetDHCPServerConfigurable(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onSetDHCPRelay(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetDHCPRelay(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);

        OC::PostCallback m_setSubnetMaskCB;
        OC::GetCallback m_getSubnetMaskCB;
        OC::PostCallback m_setIPRouterCB;
        OC::GetCallback m_getIPRoutersListCB;
        OC::PostCallback m_setDomainNameCB;
        OC::GetCallback m_getDomainNameCB;
        OC::PostCallback m_setAddressRangeCB;
        OC::GetCallback m_getAddressRangeCB;
        OC::PostCallback m_setReservedAddressCB;
        OC::GetCallback m_getReservedAddressCB;
        OC::PostCallback m_setDNSServerCB;
        OC::GetCallback m_getDNSServersCB;
        OC::PostCallback m_setDHCPServerConfigureableCB;
        OC::GetCallback m_getDHCPServerConfigureableCB;
        OC::PostCallback m_setDHCPRelayCB;
        OC::GetCallback m_getDHCPRelayCB;

        std::shared_ptr<OC::OCResource> m_resource;
        std::string m_subnetMask;
        std::string m_ipRoutersList;
        std::string m_domainName;
        AddressRange m_addressRange;
        std::string m_reservedAddress;
        std::string m_dnsServers;
        bool m_dhcpServerConfigurable;
        bool m_dhcpRelay;
        //used to turn the async get call to a sync method call
        std::mutex m_mutex;
        std::condition_variable m_cv;
};

#endif /* LANHOSTCONFIGMANAGEMENT_H_ */
