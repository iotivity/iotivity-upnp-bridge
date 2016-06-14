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

#ifndef CONNECTIONMANAGER_H_
#define CONNECTIONMANAGER_H_

#include <vector>
#include <string>
#include <functional>
#include <mutex>
#include <condition_variable>

#include <OCResource.h>
#include <OCRepresentation.h>

class ConnectionManager
{
public:
    ConnectionManager();
    ConnectionManager(OC::OCResource::Ptr resource);
    virtual ~ConnectionManager();

    ConnectionManager(const ConnectionManager &other);
    ConnectionManager &operator=(const ConnectionManager &other);

    struct ProtocolInfo
    {
        std::string sink;
        std::string source;
    };

    struct ConnectionInfo
    {
        int rcsId;
        int avTransportId;
        int peerConnectionId;
        std::string direction;
        std::string peerConnectionManager;
        std::string protocolInfo;
        std::string status;
    };

    ProtocolInfo getProtocolInfo();
    std::string getCurrentConnectionIDs();
    ConnectionInfo getConnectionInfo(std::string connectionIDs = "0");

    //Overloaded operator used for putting into a 'set'
    bool operator<(const ConnectionManager &other) const;
    const std::shared_ptr<OC::OCResource> getResource() const {return m_resource;}
private:
    void onGetProtocolInfo(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                         const int eCode);
    void onGetCurrentConnectionIDs(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                          const int eCode);
    void onGetCurrentConnectionInfo(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                              const int eCode);

    OC::GetCallback m_getProtocolInfoCB;
    OC::GetCallback m_getCurrentConnectionIDsCB;
    OC::GetCallback m_getCurrentConnctionInfoCB;

    // These are only used to hold return values for async calls
    ProtocolInfo m_protocolInfo;
    std::string m_connectionIDs;
    ConnectionInfo m_connectionInfo;

    OC::OCResource::Ptr m_resource;

    //used to turn the async get call to a sync method call
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

#endif /* CONNECTIONMANAGER_H_ */
