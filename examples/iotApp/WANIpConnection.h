/*
 * WANIpConnection.h
 *
 *  Created on: Jul 13, 2016
 *      Author: georgena
 */

#ifndef WANIPCONNECTION_H_
#define WANIPCONNECTION_H_

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
        }

        struct ConnectionState
        {
            std::string status;
            std::string lastError;
            int uptime;
            std::string statusUpdateRequest;
        }

        struct NATRSIPStatus
        {
            bool rsipAvailable;
            bool natEnabled;
        }

        void setConnectionTypeInfo(std::string newConnectionType);
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
        NATRSIPSStatus getNATRSIPStatus();

        //? getGenericPortMappingEntry();
        //? getSpecificPortMappingEntry();
        //? addPortMapping();
        //? deletePortMapping();
        std::string getExternalIPAddress();

    private:
        void onSetConnectionTypeInfo(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
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

        // These are only used to hold return values for async calls
        ConnectionTypeInfo m_connectionTypeInfo;

        OC::OCResource::Ptr m_resource;

        //used to turn the async get call to a sync method call
        std::mutex m_mutex;
        std::condition_variable m_cv;
};

#endif /* WANIPCONNECTION_H_ */
