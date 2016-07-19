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

#include "MenuLANHostConfigManagement.h"
#include "LANHostConfigManagement.h"

#include <UpnpConstants.h>

using namespace std;
using namespace OC;

MenuLANHostConfigManagement::MenuLANHostConfigManagement() :
        m_quit(false), m_mutex()
{
}

MenuLANHostConfigManagement::~MenuLANHostConfigManagement()
{
}

void MenuLANHostConfigManagement::init(set<shared_ptr<OCResource>, OCResourceComp> l3fSet)
{
    lock_guard<mutex> lock(m_mutex);
    lanHostConfigSet.clear();
    lanHostConfigVector.clear();

    lanHostConfigSet = l3fSet;
    for (auto r : lanHostConfigSet)
    {
        lanHostConfigVector.push_back(r);
    }
    print();
}

string MenuLANHostConfigManagement::getName()
{
    return "LAN Host Configuration Management";
}

void MenuLANHostConfigManagement::print()
{
    cout << "------------------------------------" << endl;
    cout << getName() << ": " << lanHostConfigVector.size() << " found" << endl;
    cout << "------------------------------------" << endl;
    cout << "find) Discover any layer 3 forwarding services " << endl;
    cout << "list) list currently discovered layer 3 forwarding services" << endl;
    cout << "clear) clear all discovered layer 3 forwarding service(s)" << endl;

    cout << "setsm) setsm <#> <subnetMask>- Set Subnet Mask" << endl;
    cout << "getsm) getsm <#> - Get Subnet Mask" << endl;
    cout << "setir) setir <#> <ipRouter> - Set IP Router" << endl;
    cout << "getir) getir <#> - Get IP Routers List" << endl;
    cout << "setdn) setdn <#> <domainName> - Set Domain Name" << endl;
    cout << "getdn) getdn <#> - Get Domain Name" << endl;
    cout << "setar) setar <#> <minAddr> <maxAddr> - Set Address Range" << endl;
    cout << "getar) getar <#> - Get Address Range" << endl;
    cout << "setra) setra <#> <reservedAddr> - Set Reserved Address" << endl;
    cout << "getra) getra <#> - Get Reserved Address" << endl;
    cout << "setdns) setdns <#> <dnsServer>- Set DNS Server" << endl;
    cout << "getdns) getdns <#> - Get DNS Servers" << endl;
    cout << "setdhcps) setdhcps <#> <true/false> - Set DHCP Server Configurable flag" << endl;
    cout << "getdhcps) getdhcps <#> - Get DHCP Sercer Configurable flag" << endl;
    cout << "setdhcpr) setdhcpr <#> <true/false> - Set DHCP Relay flag" << endl;
    cout << "getdhcpr) getdhcpr <#> - Get DHCP Relay flag" << endl;

    cout << "b) Back" << endl;
    cout << "h) Help" << endl;
    cout << "q) Quit" << endl;
}

void MenuLANHostConfigManagement::help()
{
    print();
}
void MenuLANHostConfigManagement::run(const vector<string> &cmd,
                         stack<unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if (cmd[0] == "find")
        {
            onFindResourceCb = bind(&MenuLANHostConfigManagement::onFindResource, this, placeholders::_1);
            OCStackResult result = OCPlatform::findResource("",
                                   string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_LAN_HOST_CONFIG, CT_DEFAULT,
                                   onFindResourceCb);
            cout << "findResource(" + string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" +
                    UPNP_OIC_TYPE_LAN_HOST_CONFIG + ") - " << result << endl;
        }
        else if (cmd[0] == "list")
        {
            for (size_t i = 0; i < lanHostConfigVector.size(); ++i)
            {
                cout << "[" << i << "]" << endl;
                printResourceCompact(lanHostConfigVector[i]);
            }
        }
        else if (cmd[0] == "clear")
        {
            lanHostConfigSet.clear();
            lanHostConfigVector.clear();
        }
        else if (cmd[0] == "setsm")
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    lhcm.setSubnetMask(cmd[2]);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setsm command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "getsm")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    string subnetMask = lhcm.getSubnetMask();
                    cout << lhcm.getResource() << endl;
                    cout << "Subnet Mask: " << subnetMask << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getsm command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "setir")
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    lhcm.setIPRouter(cmd[2]);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setir command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "getir")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    string ipRoutersList = lhcm.getIPRoutersList();
                    cout << lhcm.getResource() << endl;
                    cout << "IP Routers List: " << ipRoutersList << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getir command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "setdn")
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    lhcm.setDomainName(cmd[2]);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setdn command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "getdn")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    string domainName = lhcm.getDomainName();
                    cout << lhcm.getIPRoutersList() << endl;
                    cout << "Domain Name: " << domainName << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getdn command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "setar")
        {
            if (cmd.size() > 3)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    LANHostConfigManagement::AddressRange addressRange = {cmd[2], cmd[3]};
                    lhcm.setAddressRange(addressRange);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setar command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "getar")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    LANHostConfigManagement::AddressRange addressRange = lhcm.getAddressRange();
                    cout << "Address range: " << endl;
                    cout << "\t" << addressRange.minAddr << endl;
                    cout << "\t" << addressRange.maxAddr << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getar command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "setra")
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    lhcm.setReservedAddress(cmd[2]);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setra command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "getra")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    string reserveAddrs = lhcm.getReservedAddresses();
                    cout << lhcm.getResource() << endl;
                    cout << "Reserved Addresses: " << reserveAddrs << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getra command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "setdns")
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    lhcm.setDNSServer(cmd[2]);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setdns command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "getdns")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    string dnsServers = lhcm.getDNSServers();
                    cout << lhcm.getResource() << endl;
                    cout << "DNS Servers: " << dnsServers << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getdns command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "setdhcps")
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }

                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    bool serverConfigurable = false;
                    if(cmd[2] == "true") {
                        serverConfigurable = true;
                    }
                    lhcm.setDHCPServerConfigurable(serverConfigurable);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setdhcps command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "getdhcps")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    bool dnsServers = lhcm.getDHCPServerConfigurable();
                    cout << lhcm.getResource() << endl;
                    cout << "DHCP Server Configurable: " << ((dnsServers == true) ? "true" : "false") << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getdhcps command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "setdhcpr")
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }

                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    bool dhcpRelay = false;
                    if(cmd[2] == "true") {
                        dhcpRelay = true;
                    }
                    lhcm.setDHCPRelay(dhcpRelay);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setdhcpr command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if (cmd[0] == "getdhcpr")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= lanHostConfigVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    LANHostConfigManagement lhcm(lanHostConfigVector[i]);
                    bool dhcpRelay = lhcm.getDHCPRelay();
                    cout << lhcm.getResource() << endl;
                    cout << "DHCP Relay: " << ((dhcpRelay == true) ? "true" : "false") << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getdhcpr command requires an integer indicating the LAN host configuration management resource." << endl;
            }
        }
        else if ("b" == cmd[0])
        {
            if (menuStack.size() <= 1)
            {
                print();
                return; //do nothing
            }
            else
            {
                menuStack.pop();
                menuStack.top()->print();
            }
        }
        else if ("q" == cmd[0])
        {
            m_quit = true;
        }
        else if ("h" == cmd[0])
        {
            help();
        }
        else
        {
            cout << "unknown command." << endl;
            print();
        }
    }
}

bool MenuLANHostConfigManagement::quit()
{
    return m_quit;
}

void MenuLANHostConfigManagement::onFindResource(shared_ptr< OCResource > resource)
{
    lock_guard<mutex> lock(m_mutex);
    cout << "Found resource" << endl;

    try
    {
        if (resource)
        {
            for (auto &resourceType : resource->getResourceTypes())
            {
                if (resourceType == UPNP_OIC_TYPE_LAN_HOST_CONFIG)
                {
                    if (lanHostConfigSet.find(resource) == lanHostConfigSet.end())
                    {
                        lanHostConfigSet.insert(resource);
                        lanHostConfigVector.push_back(resource);
                        if (lanHostConfigSet.size() != lanHostConfigVector.size())
                        {
                            cerr << "Mismatch in discovered Services. Reinitializing.";
                            init(lanHostConfigSet);
                        }
                    }
                }
            }
        }
    }
    catch (exception &e)
    {
        cerr << "Caught exception in foundResource: " << e.what() << endl;
    }
}
