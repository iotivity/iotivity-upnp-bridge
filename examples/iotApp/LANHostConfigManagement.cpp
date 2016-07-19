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

#include "LANHostConfigManagement.h"
#include "IotivityUtility.h"
#include <chrono>

using namespace std;
using namespace std::placeholders;
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
    m_setSubnetMaskCB = bind(&LANHostConfigManagement::onSetSubnetMask, this, _1, _2, _3);
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
    m_getSubnetMaskCB = bind(&LANHostConfigManagement::onGetSubnetMask, this, _1, _2, _3);
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
    m_setIPRouterCB = bind(&LANHostConfigManagement::onSetIPRouter, this, _1, _2, _3);
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
    m_getIPRoutersListCB = bind(&LANHostConfigManagement::onGetIPRoutersList, this, _1, _2, _3);
    m_resource->get(QueryParamsMap(), m_getIPRoutersListCB);
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
    m_setDomainNameCB = bind(&LANHostConfigManagement::onSetDomainName, this, _1, _2, _3);
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
    m_getDomainNameCB = bind(&LANHostConfigManagement::onGetDomainName, this, _1, _2, _3);
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
    m_setDomainNameCB = bind(&LANHostConfigManagement::onSetAddressRange, this, _1, _2, _3);
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
    m_getAddressRangeCB = bind(&LANHostConfigManagement::onGetAddressRange, this, _1, _2, _3);
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
    m_setReservedAddressCB = bind(&LANHostConfigManagement::onSetReservedAddress, this, _1, _2, _3);
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

string LANHostConfigManagement::getReservedAddresses()
{
    unique_lock<mutex> lhcmLock(m_mutex);
    m_reservedAddress = "";
    m_getReservedAddressCB = bind(&LANHostConfigManagement::onGetReservedAddresses, this, _1, _2, _3);
    m_resource->get(QueryParamsMap(), m_getReservedAddressCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_reservedAddress;
}

void LANHostConfigManagement::onGetReservedAddresses(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
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
    m_setDNSServerCB = bind(&LANHostConfigManagement::onSetDNSServer, this, _1, _2, _3);
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
    m_getDNSServersCB = bind(&LANHostConfigManagement::onGetDNSServers, this, _1, _2, _3);
    m_resource->get(QueryParamsMap(), m_getDNSServersCB);
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
    m_setDHCPServerConfigureableCB = bind(&LANHostConfigManagement::onSetDHCPServerConfigurable, this, _1, _2, _3);
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
    m_getDHCPServerConfigureableCB = bind(&LANHostConfigManagement::onGetDHCPServerConfigurable, this, _1, _2, _3);
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
    m_setDHCPRelayCB = bind(&LANHostConfigManagement::onSetDHCPRelay, this, _1, _2, _3);
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

bool LANHostConfigManagement::getDHCPRelay()
{
    unique_lock<mutex> lhcmLock(m_mutex);
    m_dhcpRelay = false;
    m_getDHCPRelayCB = bind(&LANHostConfigManagement::onGetDHCPRelay, this, _1, _2, _3);
    m_resource->get(QueryParamsMap(), m_getDHCPRelayCB);
    if (m_cv.wait_for(lhcmLock, chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_dhcpRelay;
}
void LANHostConfigManagement::onGetDHCPRelay(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            rep.getValue("dhcpRelay", m_dhcpRelay);
        }
    }
    m_cv.notify_one();
}
