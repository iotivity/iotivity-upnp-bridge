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

#ifndef WANCOMMONINTERFACECONFIG_H_
#define WANCOMMONINTERFACECONFIG_H_

#include <vector>
#include <string>
#include <functional>
#include <mutex>
#include <condition_variable>

#include <OCResource.h>
#include <OCRepresentation.h>

class WANCommonInterfaceConfig
{
    public:
        WANCommonInterfaceConfig();
        WANCommonInterfaceConfig(OC::OCResource::Ptr resource);
        virtual ~WANCommonInterfaceConfig();
        WANCommonInterfaceConfig(const WANCommonInterfaceConfig &other);
        WANCommonInterfaceConfig &operator=(const WANCommonInterfaceConfig &other);

        struct LinkProperties
        {
            std::string accessType;
            int upMaxBitrate;
            int downMaxBitrate;
            std::string linkStatus;
        };

        struct ConnectionInfo
        {
            std::string deviceContainer;
            std::string serviceId;
        };

        void setEnabledForInternet(bool inetEnabled);
        bool getEnabledForInternet();
        LinkProperties getCommonLinkProperties();
        std::string getWANAccessProvider();
        int getMaximumActiveConnections();
        int getTotalBytesSent();
        int getTotalBytesReceived();
        int getTotalPacketsSent();
        int getTotalPacketsReceived();
        //special case: no matching UPNP action, but can be set based on observation
        int getNumberOfActiveConnections();
        ConnectionInfo getActiveConnection();

        //Overloaded operator used for putting into a 'set'
        bool operator<(const WANCommonInterfaceConfig &other) const;
        const std::shared_ptr<OC::OCResource> getResource() const {return m_resource;}
    private:
        void onSetEnabledForInternet(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetEnabledForInternet(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetCommonLinkProperties(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetWANAccessProvider(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetMaximumActiveConnections(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetTotalBytesSent(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetTotalBytesReceived(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetTotalPacketsSent(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetTotalPacketsReceived(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetNumberOfActiveConnections(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetActiveConnection(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);

        OC::PostCallback m_setEnabledForInternetCB;
        OC::GetCallback m_getEnabledForInternetCB;
        OC::GetCallback m_getCommonLinkPropertiesCB;
        OC::GetCallback m_getWANAccessProviderCB;
        OC::GetCallback m_getMaximumActiveConnectionsCB;
        OC::GetCallback m_getTotalBytesSentCB;
        OC::GetCallback m_getTotalBytesReceivedCB;
        OC::GetCallback m_getTotalPacketsSentCB;
        OC::GetCallback m_getTotalPacketsReceivedCB;
        OC::GetCallback m_getNumberOfActiveConnectionsCB;
        OC::GetCallback m_getActiveConnectionCB;

        // Used to hold return values for async calls
        bool m_inetEnabled;
        LinkProperties m_linkProperties;
        std::string m_accessProvider;
        int m_maxActiveConnections;
        int m_totalBytesSent;
        int m_totalBytesReceived;
        int m_totalPacketsSent;
        int m_totalPacketsReceived;
        int m_numberOfConnections;
        ConnectionInfo m_activeConnection;

        OC::OCResource::Ptr m_resource;

        //used to turn the async get call to a sync method call
        std::mutex m_mutex;
        std::condition_variable m_cv;
};

#endif /* WANCOMMONINTERFACECONFIG_H_ */
