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

#include "WANCommonInterfaceConfig.h"
#include "IotivityUtility.h"

#include <chrono>

using namespace std;
using namespace OC;

WANCommonInterfaceConfig::WANCommonInterfaceConfig() :
    m_inetEnabled(false),
    m_accessProvider(""),
    m_maxActiveConnections(0),
    m_totalBytesSent(0),
    m_totalBytesReceived(0),
    m_totalPacketsSent(0),
    m_totalPacketsReceived(0),
    m_numberOfConnections(0),
    m_resource(nullptr)
{
        m_linkProperties = {"", 0, 0, ""};
        m_activeConnection = {"", ""};
}

WANCommonInterfaceConfig::WANCommonInterfaceConfig(OCResource::Ptr resource) :
    m_inetEnabled(false),
    m_accessProvider(""),
    m_maxActiveConnections(0),
    m_totalBytesSent(0),
    m_totalBytesReceived(0),
    m_totalPacketsSent(0),
    m_totalPacketsReceived(0),
    m_numberOfConnections(0),
    m_resource(resource)
{
    m_linkProperties = {"", 0, 0, ""};
    m_activeConnection = {"", ""};
}

WANCommonInterfaceConfig::~WANCommonInterfaceConfig()
{
}

WANCommonInterfaceConfig::WANCommonInterfaceConfig(const WANCommonInterfaceConfig &other)
{
    *this = other;
}

WANCommonInterfaceConfig &WANCommonInterfaceConfig::operator=(const WANCommonInterfaceConfig &other)
{
    // check for self-assignment
    if (&other == this)
    {
        return *this;
    }
    m_inetEnabled = other.m_inetEnabled;
    m_accessProvider = other.m_accessProvider;
    m_maxActiveConnections = other.m_maxActiveConnections;
    m_totalBytesSent = other.m_totalBytesSent;
    m_totalBytesReceived = other.m_totalPacketsReceived;
    m_totalPacketsSent = other.m_totalPacketsSent;
    m_totalPacketsReceived = other.m_totalPacketsReceived;
    m_numberOfConnections = other.m_numberOfConnections;
    m_linkProperties = other.m_linkProperties;
    m_activeConnection = other.m_activeConnection;
    m_resource = other.m_resource;
    // do not copy the mutex or condition_variable
    return *this;
}

bool WANCommonInterfaceConfig::operator<(const WANCommonInterfaceConfig &other) const
{
    return ((*m_resource) < (*(other.m_resource)));
}

void WANCommonInterfaceConfig::setEnabledForInternet(bool inetEnabled)
{
    unique_lock<mutex> wanLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("inetEnabled", inetEnabled);
    m_setEnabledForInternetCB = bind(&WANCommonInterfaceConfig::onSetEnabledForInternet, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_setEnabledForInternetCB);
    if (m_cv.wait_for(wanLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void WANCommonInterfaceConfig::onSetEnabledForInternet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

bool WANCommonInterfaceConfig::getEnabledForInternet()
{
    unique_lock<mutex> wanLock(m_mutex);
    m_inetEnabled = false;
    m_getEnabledForInternetCB = bind(&WANCommonInterfaceConfig::onGetEnabledForInternet, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getEnabledForInternetCB);
    if (m_cv.wait_for(wanLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_inetEnabled;
}

void WANCommonInterfaceConfig::onGetEnabledForInternet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("inetEnabled", m_inetEnabled);
        }
    }
    m_cv.notify_one();
}

WANCommonInterfaceConfig::LinkProperties WANCommonInterfaceConfig::getCommonLinkProperties()
{
    unique_lock<mutex> wanLock(m_mutex);
    m_linkProperties = {"", 0, 0, ""};
    m_getCommonLinkPropertiesCB = bind(&WANCommonInterfaceConfig::onGetCommonLinkProperties, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getCommonLinkPropertiesCB);
    if (m_cv.wait_for(wanLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_linkProperties;
}

void WANCommonInterfaceConfig::onGetCommonLinkProperties(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation wanRep;
            if (rep.getValue("linkProperties", wanRep))
            {
                wanRep.getValue("accessType", m_linkProperties.accessType);
                wanRep.getValue("upMaxBitrate", m_linkProperties.upMaxBitrate);
                wanRep.getValue("downMaxBitrate", m_linkProperties.downMaxBitrate);
                wanRep.getValue("linkStatus", m_linkProperties.linkStatus);
            }
        }
    }
    m_cv.notify_one();
}

string WANCommonInterfaceConfig::getWANAccessProvider()
{
    unique_lock<mutex> wanLock(m_mutex);
    m_accessProvider = "";
    m_getWANAccessProviderCB = bind(&WANCommonInterfaceConfig::onGetWANAccessProvider, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getWANAccessProviderCB);
    if (m_cv.wait_for(wanLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_accessProvider;
}

void WANCommonInterfaceConfig::onGetWANAccessProvider(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("wanAccesssProvider", m_accessProvider);
        }
    }
    m_cv.notify_one();
}

int WANCommonInterfaceConfig::getMaximumActiveConnections()
{
    unique_lock<mutex> wanLock(m_mutex);
    m_maxActiveConnections = 0;
    m_getMaximumActiveConnectionsCB = bind(&WANCommonInterfaceConfig::onGetMaximumActiveConnections, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getMaximumActiveConnectionsCB);
    if (m_cv.wait_for(wanLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_maxActiveConnections;
}

void WANCommonInterfaceConfig::onGetMaximumActiveConnections(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("maxConnections", m_maxActiveConnections);
        }
    }
    m_cv.notify_one();
}

int WANCommonInterfaceConfig::getTotalBytesSent()
{
    unique_lock<mutex> wanLock(m_mutex);
    m_totalBytesSent = 0;
    m_getTotalBytesSentCB = bind(&WANCommonInterfaceConfig::onGetTotalBytesSent, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getTotalBytesSentCB);
    if (m_cv.wait_for(wanLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_totalBytesSent;
}

void WANCommonInterfaceConfig::onGetTotalBytesSent(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("bytesSent", m_totalBytesSent);
        }
    }
    m_cv.notify_one();
}

int WANCommonInterfaceConfig::getTotalBytesReceived()
{
    unique_lock<mutex> wanLock(m_mutex);
    m_totalBytesReceived = 0;
    m_getTotalBytesSentCB = bind(&WANCommonInterfaceConfig::onGetTotalBytesReceived, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getTotalBytesSentCB);
    if (m_cv.wait_for(wanLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_totalBytesReceived;
}

void WANCommonInterfaceConfig::onGetTotalBytesReceived(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("bytesReceived", m_totalBytesReceived);
        }
    }
    m_cv.notify_one();
}

int WANCommonInterfaceConfig::getTotalPacketsSent()
{
    unique_lock<mutex> wanLock(m_mutex);
    m_totalPacketsSent = 0;
    m_getTotalPacketsSentCB = bind(&WANCommonInterfaceConfig::onGetTotalPacketsSent, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getTotalPacketsSentCB);
    if (m_cv.wait_for(wanLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_totalPacketsSent;
}

void WANCommonInterfaceConfig::onGetTotalPacketsSent(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("packetsSent", m_totalPacketsSent);
        }
    }
    m_cv.notify_one();
}

int WANCommonInterfaceConfig::getTotalPacketsReceived()
{
    unique_lock<mutex> wanLock(m_mutex);
    m_totalPacketsReceived = 0;
    m_getTotalPacketsReceivedCB = bind(&WANCommonInterfaceConfig::onGetTotalPacketsReceived, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getTotalPacketsReceivedCB);
    if (m_cv.wait_for(wanLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_totalPacketsReceived;
}

void WANCommonInterfaceConfig::onGetTotalPacketsReceived(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("packetsReceived", m_totalPacketsReceived);
        }
    }
    m_cv.notify_one();
}

int WANCommonInterfaceConfig::getNumberOfActiveConnections()
{
    unique_lock<mutex> wanLock(m_mutex);
    m_numberOfConnections = 0;
    m_getNumberOfActiveConnectionsCB = bind(&WANCommonInterfaceConfig::onGetNumberOfActiveConnections, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getNumberOfActiveConnectionsCB);
    if (m_cv.wait_for(wanLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_numberOfConnections;
}

void WANCommonInterfaceConfig::onGetNumberOfActiveConnections(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("numConnections", m_numberOfConnections);
        }
    }
    m_cv.notify_one();
}

WANCommonInterfaceConfig::ConnectionInfo WANCommonInterfaceConfig::getActiveConnection()
{
    unique_lock<mutex> wanLock(m_mutex);
    m_activeConnection = {"", ""};
    m_getActiveConnectionCB = bind(&WANCommonInterfaceConfig::onGetActiveConnection, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getActiveConnectionCB);
    if (m_cv.wait_for(wanLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_activeConnection;
}

void WANCommonInterfaceConfig::onGetActiveConnection(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation wanRep;
            if (rep.getValue("connectionInfo", wanRep))
            {
                wanRep.getValue("deviceContainer", m_activeConnection.deviceContainer);
                wanRep.getValue("serviceId", m_activeConnection.serviceId);
            }
        }
    }
    m_cv.notify_one();
}
