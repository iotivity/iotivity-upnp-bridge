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

#include "ConnectionManager.h"

#include "IotivityUtility.h"
#include <UpnpBridgeAttributes.h>

ConnectionManager::ConnectionManager() : m_resource(nullptr)
{
    m_protocolInfo = ProtocolInfo{"", ""};
}

ConnectionManager::ConnectionManager(OC::OCResource::Ptr resource) :
                m_resource(resource)
{
    m_protocolInfo = ProtocolInfo{"", ""};
}

ConnectionManager::~ConnectionManager()
{
}

ConnectionManager::ConnectionManager(const ConnectionManager &other)
{
    *this = other;
}

ConnectionManager &ConnectionManager::operator=(const ConnectionManager &other)
{
    // check for self-assignment
    if (&other == this)
        return *this;
    m_protocolInfo = other.m_protocolInfo;
    m_resource = other.m_resource;
    // do not copy the mutex or condition_variable
    return *this;
}

ConnectionManager::ProtocolInfo ConnectionManager::getProtocolInfo()
{
    std::unique_lock<std::mutex> protocolInfoLock(m_mutex);
    m_getProtocolInfoCB = bind(&ConnectionManager::onGetProtocolInfo, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
    m_resource->get(OC::QueryParamsMap(), m_getProtocolInfoCB);
    m_cv.wait(protocolInfoLock);
    return m_protocolInfo;
}
std::string ConnectionManager::getCurrentConnectionIDs()
{
    std::unique_lock<std::mutex> connectionIDsLock(m_mutex);
    m_getCurrentConnectionIDsCB = bind(&ConnectionManager::onGetCurrentConnectionIDs, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
    m_resource->get(OC::QueryParamsMap(), m_getCurrentConnectionIDsCB);
    m_cv.wait(connectionIDsLock);
    return m_connectionIDs;
}
ConnectionManager::ConnectionInfo ConnectionManager::getConnectionInfo(std::string connectionIDs)
{
    std::unique_lock<std::mutex> protocolInfoLock(m_mutex);
    m_getProtocolInfoCB = bind(&ConnectionManager::onGetCurrentConnectionInfo, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap param = {{"connectionId", connectionIDs}};
    m_resource->get(param, m_getProtocolInfoCB);
    m_cv.wait(protocolInfoLock);
    return m_connectionInfo;
}

//Overloaded operator used for putting into a 'set'
bool ConnectionManager::operator<(const ConnectionManager &other) const
{
    return ((*m_resource) < (*(other.m_resource)));
}

void ConnectionManager::onGetProtocolInfo(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
        const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OC::OCRepresentation protocolInfoRep;
            if (rep.getValue("protocolInfo", protocolInfoRep)) {
                protocolInfoRep.getValue("sink", m_protocolInfo.sink);
                protocolInfoRep.getValue("source", m_protocolInfo.source);
            }
        }
    }
    m_cv.notify_one();
}
void ConnectionManager::onGetCurrentConnectionIDs(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
        const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OC::OCRepresentation protocolInfoRep;
            rep.getValue("currentConnectionIds", m_connectionIDs);
        }
    }
    m_cv.notify_one();
}
void ConnectionManager::onGetCurrentConnectionInfo(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
        const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OC::OCRepresentation currentConnectionInfoRep;
            if (rep.getValue("currentConnectionInfo", currentConnectionInfoRep)) {
                currentConnectionInfoRep.getValue("rcsId", m_connectionInfo.rcsId);
                currentConnectionInfoRep.getValue("avTransportId", m_connectionInfo.avTransportId);
                currentConnectionInfoRep.getValue("protocolInfo", m_connectionInfo.protocolInfo);
                currentConnectionInfoRep.getValue("peerConnectionManager", m_connectionInfo.peerConnectionManager);
                currentConnectionInfoRep.getValue("peerConnectionId", m_connectionInfo.peerConnectionId);
                currentConnectionInfoRep.getValue("direction", m_connectionInfo.direction);
                currentConnectionInfoRep.getValue("status", m_connectionInfo.status);
            }
        }
    }
    m_cv.notify_one();
}
