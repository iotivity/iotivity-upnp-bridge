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

#include <chrono>

using namespace std;
using namespace OC;

ConnectionManager::ConnectionManager() : m_resource(nullptr)
{
    m_protocolInfo = ProtocolInfo{"", ""};
    m_connectionInfo = { 0, 0, 0, "", "", "", "" };
}

ConnectionManager::ConnectionManager(OCResource::Ptr resource) :
    m_resource(resource)
{
    m_protocolInfo = ProtocolInfo{"", ""};
    m_connectionInfo = { 0, 0, 0, "", "", "", "" };
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
    m_connectionInfo = other.m_connectionInfo;
    m_resource = other.m_resource;
    // do not copy the mutex or condition_variable
    return *this;
}

ConnectionManager::ProtocolInfo ConnectionManager::getProtocolInfo()
{
    unique_lock<mutex> protocolInfoLock(m_mutex);
    m_getProtocolInfoCB = bind(&ConnectionManager::onGetProtocolInfo, this, placeholders::_1,
                               placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getProtocolInfoCB);
    if (m_cv.wait_for(protocolInfoLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_protocolInfo;
}
string ConnectionManager::getCurrentConnectionIDs()
{
    unique_lock<mutex> connectionIDsLock(m_mutex);
    m_getCurrentConnectionIDsCB = bind(&ConnectionManager::onGetCurrentConnectionIDs, this,
                                       placeholders::_1,
                                       placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getCurrentConnectionIDsCB);
    if (m_cv.wait_for(connectionIDsLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_connectionIDs;
}
ConnectionManager::ConnectionInfo ConnectionManager::getConnectionInfo(string connectionIDs)
{
    unique_lock<mutex> protocolInfoLock(m_mutex);
    m_getCurrentConnctionInfoCB = bind(&ConnectionManager::onGetCurrentConnectionInfo, this,
                                       placeholders::_1,
                                       placeholders::_2, placeholders::_3);
    QueryParamsMap param = {{"connectionId", connectionIDs}};
    m_resource->get(param, m_getCurrentConnctionInfoCB);
    if (m_cv.wait_for(protocolInfoLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_connectionInfo;
}

//Overloaded operator used for putting into a 'set'
bool ConnectionManager::operator<(const ConnectionManager &other) const
{
    return ((*m_resource) < (*(other.m_resource)));
}

void ConnectionManager::onGetProtocolInfo(const HeaderOptions &headerOptions,
        const OCRepresentation &rep,
        const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation protocolInfoRep;
            if (rep.getValue("protocolInfo", protocolInfoRep))
            {
                protocolInfoRep.getValue("sink", m_protocolInfo.sink);
                protocolInfoRep.getValue("source", m_protocolInfo.source);
            }
        }
    }
    m_cv.notify_one();
}
void ConnectionManager::onGetCurrentConnectionIDs(const HeaderOptions &headerOptions,
        const OCRepresentation &rep,
        const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation protocolInfoRep;
            rep.getValue("currentConnectionIds", m_connectionIDs);
        }
    }
    m_cv.notify_one();
}
void ConnectionManager::onGetCurrentConnectionInfo(const HeaderOptions &headerOptions,
        const OCRepresentation &rep,
        const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation currentConnectionInfoRep;
            if (rep.getValue("currentConnectionInfo", currentConnectionInfoRep))
            {
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
