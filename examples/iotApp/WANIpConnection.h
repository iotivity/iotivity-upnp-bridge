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

#ifndef WANIPCONNECTION_H_
#define WANIPCONNECTION_H_

#include <vector>
#include <string>
#include <functional>
#include <mutex>
#include <condition_variable>

#include <OCResource.h>
#include <OCRepresentation.h>

class WANIpConnection
{
    public:
        WANIpConnection();
        WANIpConnection(OC::OCResource::Ptr resource);
        virtual ~WANIpConnection();

        WANIpConnection(const WANIpConnection &other);
        WANIpConnection &operator=(const WANIpConnection &other);

        //Overloaded operator used for putting into a 'set'
        bool operator<(const WANIpConnection &other) const;
        const std::shared_ptr<OC::OCResource> getResource() const {return m_resource;}

        struct ConnectionTypeInfo
        {
            std::string type;
            std::string possibleTypes;
        };

        struct ConnectionState
        {
            std::string status;
            std::string lastError;
            int uptime;
        };

        struct NATRSIPStatus
        {
            bool rsipAvailable;
            bool natEnabled;
        };

        void setConnectionType(std::string newConnectionType);
        ConnectionTypeInfo getConnectionTypeInfo();
        ConnectionState requestConnection();
        ConnectionState requestTermination();
        ConnectionState forceTermination();
        void setAutoDisconnectTime(int autoDisconnectTime);
        void setIdleDisconnectTime(int idleDisconnectTime);
        void setWarnDisconnectDelay(int warnDisconnectdelay);
        //? getStatusInfo();
        int getAutoDisconnectTime();
        int getIdleDisconnectTime();
        int getWarnDisconnectTime();
        NATRSIPStatus getNATRSIPStatus();

        //? getGenericPortMappingEntry();
        //? getSpecificPortMappingEntry();
        //? addPortMapping();
        //? deletePortMapping();
        std::string getExternalIPAddress();

    private:
        void onSetConnectionType(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetConnectionTypeInfo(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onRequestConnection(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onRequestTermination(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onForceTermination(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onSetAutoDisconnectTime(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onSetIdleDisconnectTime(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onSetWarnDisconnectDelay(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        //void onGetStatusInfo(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);;
        void onGetAutoDisconnectTime(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetIdleDisconnectTime(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetWarnDisconnectTime(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetNATRSIPStatus(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        //void onGetGenericPortMappingEntry(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        //void onGetSpecificPortMappingEntry(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        //void onAddPortMapping(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        //void onDeletePortMapping(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetExternalIPAddress(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);

        OC::PostCallback m_setConnectionTypeInfoCB;
        OC::GetCallback m_getConnectionTypeInfoCB;
        OC::PostCallback m_requestConnectionCB;
        OC::PostCallback m_requestTerminationCB;
        OC::PostCallback m_forceTerminationCB;
        OC::PostCallback m_setAutoDisconnectTimeCB;
        OC::PostCallback m_setIdleDisconnectTimeCB;
        OC::PostCallback m_setWarnDisconnectDelayCB;
        OC::GetCallback m_getAutoDisconnectTimeCB;
        OC::GetCallback m_getIdleDisconncetTimeCB;
        OC::GetCallback m_getWarnDisconnectTimeCB;
        OC::GetCallback m_getNATRSIPSStatusCB;
        OC::GetCallback m_getExternalIPAddressCB;

        // These are only used to hold return values for async calls
        ConnectionTypeInfo m_connectionTypeInfo;

        OC::OCResource::Ptr m_resource;
        ConnectionState m_connectionState;
        int m_autoDisconectTime;
        int m_idleDisconnectTime;
        int m_warnDisconnectTime;
        NATRSIPStatus m_natRSIPStatus;
        std::string m_externalIpAddress;

        //used to turn the async get call to a sync method call
        std::mutex m_mutex;
        std::condition_variable m_cv;
};

#endif /* WANIPCONNECTION_H_ */
