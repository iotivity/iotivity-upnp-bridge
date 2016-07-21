/*
 * WANIpConnection.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: georgena
 */

#include "WANIpConnection.h"

using namespace std;
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

WANIpConnection::WANIpConnection &operator=(const WANIpConnection &other)
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

void WANIpConnection::setConnectionTypeInfo(string newConnectionType)
{
    unique_lock<mutex> wanIpLock(m_mutex);
    rep.setValue("uri", m_resource->uri());
    rep.setValue("type", newConnectionType);
    m_setConnectionTypeInfoCB = bind(&WANIpConnection::onSetConnectionTypeInfo, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(QueryParamsMap, m_setConnectionTypeInfoCB, rep);
    if (m_cv.wait_for(wanIpLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void onSetConnectionTypeInfo(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
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
    m_getConnectionTypeInfoCB = bind(&WANIpConnection::onGetConnectionTypeInfo, this, placeholders::_1, placeholders::_2, placeholders::_3);
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

}

void onRequestConnection(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{

}

WANIpConnection::ConnectionState WANIpConnection::requestTermination()
{

}

void onRequestTermination(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{

}

WANIpConnection::ConnectionState WANIpConnection::forceTermination()
{

}

void onForceTermination(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{

}

void WANIpConnection::setAutoDisconnectTime(int autoDisconnectTime)
{

}

void onSetAutoDisconnectTime(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{

}

void WANIpConnection::setIdleDisconnectTime(int idleDisconnectTime)
{

}

void onSetIdleDisconnectTime(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{

}

void WANIpConnection::setWarnDisconnectDelay(int warnDisconnectdelay)
{

}

void onSetWarnDisconnectDelay(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{

}

//? WANIpConnection::getStatusInfo();
//void onGetStatusInfo(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);;

int WANIpConnection::getAutoDisconnectTime()
{

}

void onGetAutoDisconnectTime(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{

}

int WANIpConnection::getIdleDisconnectTime()
{

}

void onGetIdleDisconnectTime(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{

}

int WANIpConnection::getWarnDisconnectTime()
{

}

void onGetWarnDisconnectTime(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{

}

WANIpConnection::NATRSIPSStatus WANIpConnection::getNATRSIPStatus()
{

}

void onGetNATRSIPStatus(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{

}

//? WANIpConnection::getGenericPortMappingEntry();
//void onGetGenericPortMappingEntry(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
//? WANIpConnection::getSpecificPortMappingEntry();
//void onGetSpecificPortMappingEntry(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
//? WANIpConnection::addPortMapping();
//void onAddPortMapping(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
//? WANIpConnection::deletePortMapping();
//void onDeletePortMapping(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);

string WANIpConnection::getExternalIPAddress()
{

}

void onGetExternalIPAddress(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{

}
