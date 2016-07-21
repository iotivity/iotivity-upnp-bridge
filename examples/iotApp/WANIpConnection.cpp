/*
 * WANIpConnection.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: georgena
 */

#include "WANIpConnection.h"
#include "IotivityUtility.h"

using namespace std;
using namespace std::placeholders;
using namespace OC;

WANIpConnection::WANIpConnection() :
            m_resource{nullptr}
{
}

WANIpConnection::WANIpConnection(OCResource::Ptr resource) :
            m_resource{resource}
{
}

WANIpConnection::~WANIpConnection()
{
}

WANIpConnection::WANIpConnection(const WANIpConnection &other)
{
    *this = other;
}

WANIpConnection &WANIpConnection::operator=(const WANIpConnection &other)
{
    // check for self-assignment
    if (&other == this)
    {
        return *this;
    }
    m_resource = other.m_resource;
    // do not copy the mutex or condition_variable
    return *this;
}

//Overloaded operator used for putting into a 'set'
bool WANIpConnection::operator<(const WANIpConnection &other) const
{
    return ((*m_resource) < (*(other.m_resource)));
}

void WANIpConnection::setConnectionType(string newConnectionType)
{
    unique_lock<mutex> wanIpLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("type", newConnectionType);
    m_setConnectionTypeInfoCB = bind(&WANIpConnection::onSetConnectionType, this, _1, _2, _3);
    m_resource->post(rep, QueryParamsMap(), m_setConnectionTypeInfoCB);
    if (m_cv.wait_for(wanIpLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void WANIpConnection::onSetConnectionType(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    if (eCode != OC_STACK_OK)
    {
        std::cerr << "POST request failure." << std::endl;
    }
    m_cv.notify_one();
}

WANIpConnection::ConnectionTypeInfo WANIpConnection::getConnectionTypeInfo()
{
    unique_lock<mutex> wanIpLock(m_mutex);
    m_connectionTypeInfo = {"", ""};
    m_getConnectionTypeInfoCB = bind(&WANIpConnection::onGetConnectionTypeInfo, this, _1, _2, _3);
    m_resource->get(QueryParamsMap(), m_getConnectionTypeInfoCB);
    if (m_cv.wait_for(wanIpLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_connectionTypeInfo;
}

void WANIpConnection::onGetConnectionTypeInfo(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation connectionTypeInfoRep;
            rep.getValue("connectionTypeInfo", connectionTypeInfoRep);
            connectionTypeInfoRep.getValue("type", m_connectionTypeInfo.type);
            connectionTypeInfoRep.getValue("allTypes", m_connectionTypeInfo.possibleTypes);
        }
    }
    m_cv.notify_one();
}

WANIpConnection::ConnectionState WANIpConnection::requestConnection()
{
    unique_lock<mutex> wanIpLock(m_mutex);
    m_connectionState = {"", "", 0};
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("statusUpdateRequest", "RequestConnection");
    rep.setValue("connectionState", attributes);
    m_requestConnectionCB = bind(&WANIpConnection::onRequestConnection, this, _1, _2, _3);
    m_resource->post(rep, QueryParamsMap(), m_requestConnectionCB);
    if (m_cv.wait_for(wanIpLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_connectionState;
}

void WANIpConnection::onRequestConnection(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    if (eCode != OC_STACK_OK)
    {
        std::cerr << "POST request failure." << std::endl;
    }
    else
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation connectionStateRep;
            rep.getValue("connectionState", connectionStateRep);
            connectionStateRep.getValue("status", m_connectionState.status);
            connectionStateRep.getValue("lastError", m_connectionState.lastError);
            connectionStateRep.getValue("uptime", m_connectionState.uptime);
        }
    }

    m_cv.notify_one();
}

WANIpConnection::ConnectionState WANIpConnection::requestTermination()
{
    unique_lock<mutex> wanIpLock(m_mutex);
    m_connectionState = {"", "", 0};
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("statusUpdateRequest", "RequestTermination");
    rep.setValue("connectionState", attributes);
    m_requestTerminationCB = bind(&WANIpConnection::onRequestTermination, this, _1, _2, _3);
    m_resource->post(rep, QueryParamsMap(), m_requestTerminationCB);
    if (m_cv.wait_for(wanIpLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_connectionState;
}

void WANIpConnection::onRequestTermination(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    if (eCode != OC_STACK_OK)
    {
        std::cerr << "POST request failure." << std::endl;
    }
    else
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation connectionStateRep;
            rep.getValue("connectionState", connectionStateRep);
            connectionStateRep.getValue("status", m_connectionState.status);
            connectionStateRep.getValue("lastError", m_connectionState.lastError);
            connectionStateRep.getValue("uptime", m_connectionState.uptime);
        }
    }
    m_cv.notify_one();
}

WANIpConnection::ConnectionState WANIpConnection::forceTermination()
{
    unique_lock<mutex> wanIpLock(m_mutex);
    m_connectionState = {"", "", 0};
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("statusUpdateRequest", "ForceTermination");
    rep.setValue("connectionState", attributes);
    m_forceTerminationCB = bind(&WANIpConnection::onForceTermination, this, _1, _2, _3);
    m_resource->post(rep, QueryParamsMap(), m_forceTerminationCB);
    if (m_cv.wait_for(wanIpLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_connectionState;
}

void WANIpConnection::onForceTermination(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    if (eCode != OC_STACK_OK)
    {
        std::cerr << "POST request failure." << std::endl;
    }
    else
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation connectionStateRep;
            rep.getValue("connectionState", connectionStateRep);
            connectionStateRep.getValue("status", m_connectionState.status);
            connectionStateRep.getValue("lastError", m_connectionState.lastError);
            connectionStateRep.getValue("uptime", m_connectionState.uptime);
        }
    }
    m_cv.notify_one();
}

void WANIpConnection::setAutoDisconnectTime(int autoDisconnectTime)
{
    unique_lock<mutex> wanIpLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("autoDiscoTime", autoDisconnectTime);
    m_setAutoDisconnectTimeCB = bind(&WANIpConnection::onSetAutoDisconnectTime, this, _1, _2, _3);
    m_resource->post(rep, QueryParamsMap(), m_setAutoDisconnectTimeCB);
    if (m_cv.wait_for(wanIpLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void WANIpConnection::onSetAutoDisconnectTime(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    if (eCode != OC_STACK_OK)
    {
        std::cerr << "POST request failure." << std::endl;
    }
    m_cv.notify_one();
}

void WANIpConnection::setIdleDisconnectTime(int idleDisconnectTime)
{
    unique_lock<mutex> wanIpLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("idleDiscoTime", idleDisconnectTime);
    m_setIdleDisconnectTimeCB = bind(&WANIpConnection::onSetIdleDisconnectTime, this, _1, _2, _3);
    m_resource->post(rep, QueryParamsMap(), m_setIdleDisconnectTimeCB);
    if (m_cv.wait_for(wanIpLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void WANIpConnection::onSetIdleDisconnectTime(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    if (eCode != OC_STACK_OK)
    {
        std::cerr << "POST request failure." << std::endl;
    }
    m_cv.notify_one();
}

void WANIpConnection::setWarnDisconnectDelay(int warnDisconnectdelay)
{
    unique_lock<mutex> wanIpLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("warnDiscoTime", warnDisconnectdelay);
    m_setWarnDisconnectDelayCB = bind(&WANIpConnection::onSetWarnDisconnectDelay, this, _1, _2, _3);
    m_resource->post(rep, QueryParamsMap(), m_setWarnDisconnectDelayCB);
    if (m_cv.wait_for(wanIpLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void WANIpConnection::onSetWarnDisconnectDelay(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    if (eCode != OC_STACK_OK)
    {
        std::cerr << "POST request failure." << std::endl;
    }
    m_cv.notify_one();
}

int WANIpConnection::getAutoDisconnectTime()
{
    unique_lock<mutex> wanIpLock(m_mutex);
    m_autoDisconectTime = 0;
    m_getAutoDisconnectTimeCB = bind(&WANIpConnection::onGetAutoDisconnectTime, this, _1, _2, _3);
    m_resource->get(QueryParamsMap(), m_getAutoDisconnectTimeCB);
    if (m_cv.wait_for(wanIpLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_autoDisconectTime;
}

void WANIpConnection::onGetAutoDisconnectTime(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("autoDiscoTime", m_autoDisconectTime);
        }
    }
    else
    {
        std::cerr << "GET request failure." << std::endl;
    }
    m_cv.notify_one();
}

int WANIpConnection::getIdleDisconnectTime()
{
    unique_lock<mutex> wanIpLock(m_mutex);
    m_idleDisconnectTime = 0;
    m_getIdleDisconncetTimeCB = bind(&WANIpConnection::onGetAutoDisconnectTime, this, _1, _2, _3);
    m_resource->get(QueryParamsMap(), m_getIdleDisconncetTimeCB);
    if (m_cv.wait_for(wanIpLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_idleDisconnectTime;
}

void WANIpConnection::onGetIdleDisconnectTime(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("idleDiscoTime", m_autoDisconectTime);
        }
    }
    else
    {
        std::cerr << "GET request failure." << std::endl;
    }
    m_cv.notify_one();
}

int WANIpConnection::getWarnDisconnectTime()
{
    unique_lock<mutex> wanIpLock(m_mutex);
    m_warnDisconnectTime = 0;
    m_getWarnDisconnectTimeCB = bind(&WANIpConnection::onGetAutoDisconnectTime, this, _1, _2, _3);
    m_resource->get(QueryParamsMap(), m_getWarnDisconnectTimeCB);
    if (m_cv.wait_for(wanIpLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_warnDisconnectTime;
}

void WANIpConnection::onGetWarnDisconnectTime(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("warnDiscoTime", m_warnDisconnectTime);
        }
    }
    else
    {
        std::cerr << "GET request failure." << std::endl;
    }
    m_cv.notify_one();
}

WANIpConnection::NATRSIPStatus WANIpConnection::getNATRSIPStatus()
{
    unique_lock<mutex> wanIpLock(m_mutex);
    m_natRSIPStatus = {true, true};
    m_getWarnDisconnectTimeCB = bind(&WANIpConnection::onGetAutoDisconnectTime, this, _1, _2, _3);
    m_resource->get(QueryParamsMap(), m_getWarnDisconnectTimeCB);
    if (m_cv.wait_for(wanIpLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_natRSIPStatus;
}

void WANIpConnection::onGetNATRSIPStatus(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation natRSIPStatusRep;
            rep.getValue("nat", natRSIPStatusRep);
            natRSIPStatusRep.getValue("rsip", m_natRSIPStatus.rsipAvailable);
            natRSIPStatusRep.getValue("enabled", m_natRSIPStatus.natEnabled);
        }
    }
    else
    {
        std::cerr << "GET request failure." << std::endl;
    }
    m_cv.notify_one();
}


string WANIpConnection::getExternalIPAddress()
{
    unique_lock<mutex> wanIpLock(m_mutex);
    m_externalIpAddress = "";
    m_getExternalIPAddressCB = bind(&WANIpConnection::onGetAutoDisconnectTime, this, _1, _2, _3);
    m_resource->get(QueryParamsMap(), m_getExternalIPAddressCB);
    if (m_cv.wait_for(wanIpLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_externalIpAddress;
}

void WANIpConnection::onGetExternalIPAddress(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("externAddr", m_externalIpAddress);
        }
    }
    else
    {
        std::cerr << "GET request failure." << std::endl;
    }
    m_cv.notify_one();
}
