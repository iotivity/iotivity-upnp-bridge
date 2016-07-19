/*
 * LANHostConfigManagement.cpp
 *
 *  Created on: Jul 18, 2016
 *      Author: georgena
 */

#include "LANHostConfigManagement.h"
#include "IotivityUtility.h"
#include <chrono>

using namespace std;
using namespace OC;

LANHostConfigManagement::LANHostConfigManagement()
{
}

LANHostConfigManagement::LANHostConfigManagement(shared_ptr<OCResource> resource) :
        m_resource(resource)
{

}

LANHostConfigManagement::~LANHostConfigManagement()
{
}

LANHostConfigManagement::LANHostConfigManagement( const LANHostConfigManagement &other )
{
    *this = other;
}

LANHostConfigManagement &LANHostConfigManagement::operator=(const LANHostConfigManagement &other)
{
    // check for self-assignment
    if (&other == this)
        return *this;
    m_resource = other.m_resource;
    // do not copy the mutex or condition_variable
    return *this;
}
//Overloaded operator used for putting into a 'set'
bool LANHostConfigManagement::operator<(const LANHostConfigManagement &other) const
{
    return ((*m_resource) < (*(other.m_resource)));
}

void LANHostConfigManagement::setSubnetMask(string subnetMask)
{
    unique_lock<mutex> lhcmLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("subnetMask", subnetMask);
    m_setSubnetMaskCB = bind(&LANHostConfigManagement::onSetSubnetMask, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_setSubnetMaskCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void LANHostConfigManagement::onSetSubnetMask(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

string LANHostConfigManagement::getSubnetMask()
{
    unique_lock<mutex> lhcmLock(m_mutex);
    m_subnetMask = "";
    m_getSubnetMaskCB = bind(&LANHostConfigManagement::onGetSubnetMask, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getSubnetMaskCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_subnetMask;
}

void LANHostConfigManagement::onGetSubnetMask(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("subnetMask", m_subnetMask);
        }
    }
    m_cv.notify_one();
}

void LANHostConfigManagement::setIPRouter(string ipRouter)
{
    unique_lock<mutex> lhcmLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("ipRouters", ipRouter);
    m_setIPRouterCB = bind(&LANHostConfigManagement::onSetIPRouter, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_setIPRouterCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void LANHostConfigManagement::onSetIPRouter(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

string LANHostConfigManagement::getIPRoutersList()
{
    unique_lock<mutex> lhcmLock(m_mutex);
    m_ipRoutersList = "";
    m_getIPRoutersListCB = bind(&LANHostConfigManagement::onGetIPRoutersList, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getSubnetMaskCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_ipRoutersList;
}

void LANHostConfigManagement::onGetIPRoutersList(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("ipRouters", m_ipRoutersList);
        }
    }
    m_cv.notify_one();
}

void LANHostConfigManagement::setDomainName(string domanName)
{
    unique_lock<mutex> lhcmLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("domainName", domanName);
    m_setDomainNameCB = bind(&LANHostConfigManagement::onSetDomainName, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_setDomainNameCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void LANHostConfigManagement::onSetDomainName(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

string LANHostConfigManagement::getDomainName()
{
    unique_lock<mutex> lhcmLock(m_mutex);
    m_domainName = "";
    m_getDomainNameCB = bind(&LANHostConfigManagement::onGetDomainName, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getDomainNameCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_domainName;
}

void LANHostConfigManagement::onGetDomainName(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("domainName", m_domainName);
        }
    }
    m_cv.notify_one();
}

void LANHostConfigManagement::setAddressRange(const AddressRange& addressRange)
{
    unique_lock<mutex> lhcmLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    OCRepresentation attributes;
    attributes.setValue("minAddr", addressRange.minAddr);
    attributes.setValue("maxAddr", addressRange.maxAddr);
    rep.setValue("addrRange", attributes);
    m_setDomainNameCB = bind(&LANHostConfigManagement::onSetAddressRange, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_setDomainNameCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void LANHostConfigManagement::onSetAddressRange(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

LANHostConfigManagement::AddressRange LANHostConfigManagement::getAddressRange()
{
    unique_lock<mutex> lhcmLock(m_mutex);
    m_addressRange = {"", ""};
    m_getAddressRangeCB = bind(&LANHostConfigManagement::onGetAddressRange, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getAddressRangeCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_addressRange;
}

void LANHostConfigManagement::onGetAddressRange(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation addressRangeRep;
            if (rep.getValue("addrRange", addressRangeRep))
            {
                addressRangeRep.getValue("minAddr", m_addressRange.minAddr);
                addressRangeRep.getValue("maxAddr", m_addressRange.maxAddr);
            }
        }
    }
    m_cv.notify_one();
}

void LANHostConfigManagement::setReservedAddress(string reservedAddress)
{
    unique_lock<mutex> lhcmLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("reservedAddr", reservedAddress);
    m_setReservedAddressCB = bind(&LANHostConfigManagement::onSetReservedAddress, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_setReservedAddressCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void LANHostConfigManagement::onSetReservedAddress(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

string LANHostConfigManagement::getReservedAddress()
{
    unique_lock<mutex> lhcmLock(m_mutex);
    m_reservedAddress = "";
    m_getReservedAddressCB = bind(&LANHostConfigManagement::onGetReservedAddress, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getReservedAddressCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_reservedAddress;
}

void LANHostConfigManagement::onGetReservedAddress(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("reservedAddr", m_reservedAddress);
        }
    }
    m_cv.notify_one();
}

void LANHostConfigManagement::setDNSServer(string dnsServer)
{
    unique_lock<mutex> lhcmLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("dnsServers", dnsServer);
    m_setDNSServerCB = bind(&LANHostConfigManagement::onSetDNSServer, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_setDNSServerCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void LANHostConfigManagement::onSetDNSServer(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

string LANHostConfigManagement::getDNSServers()
{
    unique_lock<mutex> lhcmLock(m_mutex);
    m_dnsServers = "";
    m_getDNSServersCB = bind(&LANHostConfigManagement::onGetDNSServers, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getReservedAddressCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_dnsServers;
}

void LANHostConfigManagement::onGetDNSServers(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("dnsServers", m_dnsServers);
        }
    }
    m_cv.notify_one();
}

void LANHostConfigManagement::setDHCPServerConfigurable(bool isConfigurable)
{
    unique_lock<mutex> lhcmLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("configurable", isConfigurable);
    m_setDHCPServerConfigureableCB = bind(&LANHostConfigManagement::onSetDHCPServerConfigurable, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_setDHCPServerConfigureableCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void LANHostConfigManagement::onSetDHCPServerConfigurable(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}

bool LANHostConfigManagement::getDHCPServerConfigurable()
{
    unique_lock<mutex> lhcmLock(m_mutex);
    m_dhcpServerConfigurable = false;
    m_getDHCPServerConfigureableCB = bind(&LANHostConfigManagement::onGetDHCPServerConfigurable, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getDHCPServerConfigureableCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_dhcpServerConfigurable;
}

void LANHostConfigManagement::onGetDHCPServerConfigurable(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("configurable", m_dhcpServerConfigurable);
        }
    }
    m_cv.notify_one();
}

void LANHostConfigManagement::setDHCPRelay(bool dhcpRelay)
{
    unique_lock<mutex> lhcmLock(m_mutex);
    OCRepresentation rep;
    rep.setValue("uri", m_resource->uri());
    rep.setValue("dhcpRelay", dhcpRelay);
    m_setDHCPRelayCB = bind(&LANHostConfigManagement::onSetDHCPRelay, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_resource->post(rep, QueryParamsMap(), m_setDHCPRelayCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
}

void LANHostConfigManagement::onSetDHCPRelay(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    (void) eCode;
    m_cv.notify_one();
}
