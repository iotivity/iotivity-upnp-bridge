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

#include "MenuWANIpConnection.h"
#include "WANIpConnection.h"

using namespace std;

MenuWANIpConnection::MenuWANIpConnection() :
    m_quit(false), m_mutex()
{
}

MenuWANIpConnection::~MenuWANIpConnection()
{
}

void MenuWANIpConnection::init(set<OC::OCResource::Ptr, OCResourceComp> wanCommonInterfaceSet)
{
    lock_guard<mutex> lock(m_mutex);
    m_wanIpConnectSet.clear();
    m_wanIpConnectVector.clear();

    m_wanIpConnectSet = wanCommonInterfaceSet;
    for (auto r : m_wanIpConnectSet)
    {
        m_wanIpConnectVector.push_back(r);
    }
    print();
}

string MenuWANIpConnection::getName()
{
    return "WAN IP Connection";
}

void MenuWANIpConnection::print()
{
    cout << "------------------------------------" << endl;
    cout << getName() << ": " << m_wanIpConnectVector.size() << " found" << endl;
    cout << "------------------------------------" << endl;
    cout << "find) Discover any WAN IP Connection services" << endl;
    cout << "list) list currently discovered WAN IP Connection services " << endl;
    cout << "clear) clear all discovered WAN IP Connection services" << endl;

    cout << "setct) setct <#> <NewConnectionType> - set connection type" << endl;
    cout << "getct) getct <#> - get current connection type and possible connection types" <<  endl;
    cout << "rc) rc <#> - request connection" << endl;
    cout << "rt) rt <#> - request termination" << endl;
    cout << "ft) ft <#> - force termination" << endl;
    cout << "setadt) setadt <#> <time_in_sec> - set auto disconnect time" << endl;
    cout << "setidt) setidt <#> <time_in_sec> - set idle disconnect time" << endl;
    cout << "setwdd) setwdd <#> <time_in_sec> - set warn disconnect delay" << endl;
    cout << "getadt) getadt <#> - get auto disconnect time" << endl;
    cout << "getidt) getidt <#> - get idle disconnect time" << endl;
    cout << "getwdt) getwdt <#> - get warn disconnect time" << endl;
    cout << "getnss) getnss <#> - get NAP/RSIP status" << endl;
    cout << "geteia) geteia <#> - get external IP address" << endl;

    cout << "b) Back" << endl;
    cout << "h) Help" << endl;
    cout << "q) Quit" << endl;
}

void MenuWANIpConnection::help()
{
    print();
}

void MenuWANIpConnection::run(const vector<string> &cmd,
                                 stack<unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if ("find" == cmd[0])
        {
            onFindResourceCb = bind(&MenuWANIpConnection::onFindResource, this, placeholders::_1);
            OC::OCPlatform::findResource("",
                                         string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_WAN_IP_CONNECTION, CT_DEFAULT,
                                         onFindResourceCb);
        }
        else if (cmd[0] == "list")
        {
            for (size_t i = 0; i < m_wanIpConnectVector.size(); ++i)
            {
                cout << "[" << i << "]" << endl;
                printResourceCompact(m_wanIpConnectVector[i]);
            }
        }
        else if ("clear" == cmd[0])
        {
            m_wanIpConnectSet.clear();
            m_wanIpConnectVector.clear();
        }
        else if ("setct" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanIpConnectVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANIpConnection wic(m_wanIpConnectVector[i]);
                    cout << m_wanIpConnectVector[i]->uri() << endl;
                    wic.setConnectionType(cmd[2]);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setct command missing a required parameter." << endl;
            }
        }
        else if ("getct" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanIpConnectVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANIpConnection wic(m_wanIpConnectVector[i]);
                    cout << m_wanIpConnectVector[i]->uri() << endl;
                    WANIpConnection::ConnectionTypeInfo cti = wic.getConnectionTypeInfo();
                    cout << "Connection Type Info:" << endl;
                    cout << "\ttype: " << cti.type << endl;
                    cout << "\tpossible types: " << cti.possibleTypes << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getct command missing a required parameter." << endl;
            }
        }
        else if ("rc" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanIpConnectVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANIpConnection wic(m_wanIpConnectVector[i]);
                    cout << m_wanIpConnectVector[i]->uri() << endl;
                    WANIpConnection::ConnectionState cs = wic.requestConnection();
                    cout << "Connection State:" << endl;
                    cout << "\tstatus: " << cs.status << endl;
                    cout << "\tlast error: " << cs.lastError << endl;
                    cout << "\tuptime: " << cs.uptime << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "rc command requires an integer." << endl;
            }
        }
        else if ("rt" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanIpConnectVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANIpConnection wic(m_wanIpConnectVector[i]);
                    cout << m_wanIpConnectVector[i]->uri() << endl;
                    WANIpConnection::ConnectionState cs = wic.requestTermination();
                    cout << "Connection State:" << endl;
                    cout << "\tstatus: " << cs.status << endl;
                    cout << "\tlast error: " << cs.lastError << endl;
                    cout << "\tuptime: " << cs.uptime << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "rt command requires an integer." << endl;
            }
        }
        else if ("ft" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanIpConnectVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANIpConnection wic(m_wanIpConnectVector[i]);
                    cout << m_wanIpConnectVector[i]->uri() << endl;
                    WANIpConnection::ConnectionState cs = wic.forceTermination();
                    cout << "Connection State:" << endl;
                    cout << "\tstatus: " << cs.status << endl;
                    cout << "\tlast error: " << cs.lastError << endl;
                    cout << "\tuptime: " << cs.uptime << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "ft command requires an integer." << endl;
            }
        }
        else if ("setadt" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanIpConnectVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    int time_in_sec = stoi(cmd[2]);
                    WANIpConnection wic(m_wanIpConnectVector[i]);
                    cout << m_wanIpConnectVector[i]->uri() << endl;
                    wic.setAutoDisconnectTime(time_in_sec);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setadt command missing a required parameter." << endl;
            }
        }
        else if ("setidt" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanIpConnectVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    int time_in_sec = stoi(cmd[2]);
                    WANIpConnection wic(m_wanIpConnectVector[i]);
                    cout << m_wanIpConnectVector[i]->uri() << endl;
                    wic.setIdleDisconnectTime(time_in_sec);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setidt command missing a required parameter." << endl;
            }
        }
        else if ("setwdd" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanIpConnectVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    int time_in_sec = stoi(cmd[2]);
                    WANIpConnection wic(m_wanIpConnectVector[i]);
                    cout << m_wanIpConnectVector[i]->uri() << endl;
                    wic.setWarnDisconnectDelay(time_in_sec);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setwdd command missing a required parameter." << endl;
            }
        }
        else if ("getadt" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanIpConnectVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANIpConnection wic(m_wanIpConnectVector[i]);
                    cout << m_wanIpConnectVector[i]->uri() << endl;
                    int disconnect_time = wic.getAutoDisconnectTime();
                    cout << "Auto disconnect time:" << disconnect_time << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getadt command requires an integer." << endl;
            }
        }
        else if ("getidt" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanIpConnectVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANIpConnection wic(m_wanIpConnectVector[i]);
                    cout << m_wanIpConnectVector[i]->uri() << endl;
                    int disconnect_time = wic.getIdleDisconnectTime();
                    cout << "Idle disconnect time:" << disconnect_time << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getidt command requires an integer." << endl;
            }
        }
        else if ("getwdt" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanIpConnectVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANIpConnection wic(m_wanIpConnectVector[i]);
                    cout << m_wanIpConnectVector[i]->uri() << endl;
                    int disconnect_time = wic.getWarnDisconnectTime();
                    cout << "Warn disconnect time:" << disconnect_time << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getwdt command requires an integer." << endl;
            }
        }
        else if ("getnss" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanIpConnectVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANIpConnection wic(m_wanIpConnectVector[i]);
                    cout << m_wanIpConnectVector[i]->uri() << endl;
                    WANIpConnection::NATRSIPStatus nat_rsip_status = wic.getNATRSIPStatus();
                    cout << "NAT/RSIP status:" << endl;
                    cout << "\tNAT enabled: " << ((nat_rsip_status.natEnabled == true) ? "true" : "false") << endl;
                    cout << "\tRSIP available: " << ((nat_rsip_status.rsipAvailable == true) ? "true" : "false") << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getnss command missing a required parameter." << endl;
            }
        }
        else if ("geteia" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanIpConnectVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANIpConnection wic(m_wanIpConnectVector[i]);
                    cout << m_wanIpConnectVector[i]->uri() << endl;
                    string external_ip_address = wic.getExternalIPAddress();
                    cout << "External IP address:" << external_ip_address << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "geteia command requires an integer." << endl;
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

bool MenuWANIpConnection::quit()
{
    return m_quit;
}

void MenuWANIpConnection::onFindResource(shared_ptr< OC::OCResource > resource)
{
    lock_guard<mutex> lock(m_mutex);
    cout << "Found resource" << endl;

    try
    {
        if (resource)
        {
            for (auto &resourceType : resource->getResourceTypes())
            {
                if (resourceType == UPNP_OIC_TYPE_WAN_IP_CONNECTION)
                {
                    if (m_wanIpConnectSet.find(resource) == m_wanIpConnectSet.end())
                    {
                        m_wanIpConnectSet.insert(resource);
                        m_wanIpConnectVector.push_back(resource);
                        if (m_wanIpConnectSet.size() != m_wanIpConnectVector.size())
                        {
                            cerr << "Mismatch in discovered Services. Reinitializing.";
                            init(m_wanIpConnectSet);
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
