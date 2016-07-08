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


#include "MenuWANCommonInterface.h"
#include "WANCommonInterfaceConfig.h"

using namespace std;

MenuWANCommonInterface::MenuWANCommonInterface() :
    m_quit(false), m_mutex()
{
}

MenuWANCommonInterface::~MenuWANCommonInterface()
{
}

void MenuWANCommonInterface::init(set<OC::OCResource::Ptr, OCResourceComp> wanCommonInterfaceSet)
{
    lock_guard<mutex> lock(m_mutex);
    m_wanCommonInterfaceSet.clear();
    m_wanCommonInterfaceVector.clear();

    m_wanCommonInterfaceSet = wanCommonInterfaceSet;
    for (auto r : m_wanCommonInterfaceSet)
    {
        m_wanCommonInterfaceVector.push_back(r);
    }
    print();
}

string MenuWANCommonInterface::getName()
{
    return "WAN Common Interface Config";
}

void MenuWANCommonInterface::print()
{
    cout << "------------------------------------" << endl;
    cout << getName() << ": " << m_wanCommonInterfaceVector.size() << " found" << endl;
    cout << "------------------------------------" << endl;
    cout << "find) Discover any AV transport services " << endl;
    cout << "list) list currently discovered AV transport services " << endl;
    cout << "clear) clear all discovered content directory services" << endl;

    cout << "setefi) setefi <#> <inetEnnabled> - enable or disable access to and from the Internet" << endl;
    cout << "getefi) getefi <#> - retrieve the value of EnabledForInternet" << endl;
    cout << "getclp) getclp <#> - get common link properties" << endl;
    cout << "getwap) getwap <#> - get WAN access provider" << endl;
    cout << "getmac) getmac <#> - get maximum active connections" << endl;
    cout << "gettbs) gettbs <#> - get total bytes sent" << endl;
    cout << "gettbr) gettbr <#> - get total bytes received" << endl;
    cout << "gettps) gettps <#> - get total packets sent" << endl;
    cout << "gettpr) gettpr <#> - get total packets received" << endl;
    cout << "getnoc) getnoc <#> - get number of active connections" << endl;
    cout << "getac)  getac <#> - get active connection" << endl;

    cout << "b) Back" << endl;
    cout << "h) Help" << endl;
    cout << "q) Quit" << endl;
}

void MenuWANCommonInterface::help()
{
    print();
}

void MenuWANCommonInterface::run(const vector<string> &cmd,
                          stack<unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if ("find" == cmd[0])
        {
            onFindResourceCb = bind(&MenuWANCommonInterface::onFindResource, this, placeholders::_1);
            OC::OCPlatform::findResource("",
                                         string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_WAN_IF_CONFIG, CT_DEFAULT,
                                         onFindResourceCb);
        }
        else if (cmd[0] == "list")
        {
            for (size_t i = 0; i < m_wanCommonInterfaceVector.size(); ++i)
            {
                cout << "[" << i << "]" << endl;
                printResourceCompact(m_wanCommonInterfaceVector[i]);
            }
        }
        else if ("clear" == cmd[0])
        {
            m_wanCommonInterfaceSet.clear();
            m_wanCommonInterfaceVector.clear();
        }
        else if ("setefi" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanCommonInterfaceVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    bool newinetEnable = (cmd[2] == "false") ? false : true;
                    WANCommonInterfaceConfig wcic(m_wanCommonInterfaceVector[i]);
                    cout << m_wanCommonInterfaceVector[i]->uri() << endl;
                    cout << "Set Enable for Internet flag" <<  endl;
                    wcic.setEnabledForInternet(newinetEnable);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setefi command missing a required parameter." << endl;
            }
        }
        else if ("getefi" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanCommonInterfaceVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANCommonInterfaceConfig wcic(m_wanCommonInterfaceVector[i]);
                    cout << m_wanCommonInterfaceVector[i]->uri() << endl;
                    bool inetEnabled = wcic.getEnabledForInternet();
                    if(inetEnabled == true)
                    {
                        cout << "Access to and from the internet is enabled." << endl;
                    }
                    else
                    {
                        cout << "Access to and from the internet is NOT enabled." << endl;
                    }
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getefi command missing a required parameter." << endl;
            }
        }
        else if ("getclp" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanCommonInterfaceVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANCommonInterfaceConfig wcic(m_wanCommonInterfaceVector[i]);
                    cout << m_wanCommonInterfaceVector[i]->uri() << endl;
                    WANCommonInterfaceConfig::LinkProperties lp = wcic.getCommonLinkProperties();
                    cout << "Common Link Properties:" << endl;
                    cout << "\tAccess type: " << lp.accessType << endl;
                    cout << "\tMax bitrate upstream: " << lp.upMaxBitrate << endl;
                    cout << "\tMax bitrate downstream: " << lp.downMaxBitrate << endl;
                    cout << "\tPhysical link status: " << lp.linkStatus << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getclp command requires an integer." << endl;
            }
        }
        else if ("getwap" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanCommonInterfaceVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANCommonInterfaceConfig wcic(m_wanCommonInterfaceVector[i]);
                    cout << m_wanCommonInterfaceVector[i]->uri() << endl;
                    string wap = wcic.getWANAccessProvider();
                    cout << "WAN access provider: " << wap << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getwap command requires an integer." << endl;
            }
        }
        else if ("getmac" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanCommonInterfaceVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANCommonInterfaceConfig wcic(m_wanCommonInterfaceVector[i]);
                    cout << m_wanCommonInterfaceVector[i]->uri() << endl;
                    int mac = wcic.getMaximumActiveConnections();
                    cout << "Maximum active connections: " << mac << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getmac command requires an integer." << endl;
            }
        }
        else if ("gettbs" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanCommonInterfaceVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANCommonInterfaceConfig wcic(m_wanCommonInterfaceVector[i]);
                    cout << m_wanCommonInterfaceVector[i]->uri() << endl;
                    int tbs = wcic.getTotalBytesSent();
                    cout << "Total bytes sent: " << tbs << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "gettbs command requires an integer." << endl;
            }
        }
        else if ("gettbr" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanCommonInterfaceVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANCommonInterfaceConfig wcic(m_wanCommonInterfaceVector[i]);
                    cout << m_wanCommonInterfaceVector[i]->uri() << endl;
                    int tbr = wcic.getTotalBytesReceived();
                    cout << "Total bytes revieved: " << tbr << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "gettbr command requires an integer." << endl;
            }
        }
        else if ("gettps" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanCommonInterfaceVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANCommonInterfaceConfig wcic(m_wanCommonInterfaceVector[i]);
                    cout << m_wanCommonInterfaceVector[i]->uri() << endl;
                    int tps = wcic.getTotalPacketsSent();
                    cout << "Total packets sent: " << tps << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "gettps command requires an integer." << endl;
            }
        }
        else if ("gettpr" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanCommonInterfaceVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANCommonInterfaceConfig wcic(m_wanCommonInterfaceVector[i]);
                    cout << m_wanCommonInterfaceVector[i]->uri() << endl;
                    int tpr = wcic.getTotalPacketsReceived();
                    cout << "Total packets received: " << tpr << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "gettpr command requires an integer." << endl;
            }
        }
        else if ("getnoc" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanCommonInterfaceVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANCommonInterfaceConfig wcic(m_wanCommonInterfaceVector[i]);
                    cout << m_wanCommonInterfaceVector[i]->uri() << endl;
                    int noc = wcic.getNumberOfActiveConnections();
                    cout << "Number of active connections: " << noc << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getnoc command requires an integer." << endl;
            }
        }
        else if ("getac" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_wanCommonInterfaceVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    WANCommonInterfaceConfig wcic(m_wanCommonInterfaceVector[i]);
                    cout << m_wanCommonInterfaceVector[i]->uri() << endl;
                    WANCommonInterfaceConfig::ConnectionInfo ci = wcic.getActiveConnection();
                    cout << "Active Connection Info:" << endl;
                    cout << "\tDevice container: " << ci.deviceContainer << endl;
                    cout << "\tService ID: " << ci.serviceId << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getac command requires an integer." << endl;
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

bool MenuWANCommonInterface::quit()
{
    return m_quit;
}

void MenuWANCommonInterface::onFindResource(shared_ptr< OC::OCResource > resource)
{
    lock_guard<mutex> lock(m_mutex);
    cout << "Found resource" << endl;

    try
    {
        if (resource)
        {
            for (auto &resourceType : resource->getResourceTypes())
            {
                if (resourceType == UPNP_OIC_TYPE_WAN_IF_CONFIG)
                {
                    if (m_wanCommonInterfaceSet.find(resource) == m_wanCommonInterfaceSet.end())
                    {
                        m_wanCommonInterfaceSet.insert(resource);
                        m_wanCommonInterfaceVector.push_back(resource);
                        if (m_wanCommonInterfaceSet.size() != m_wanCommonInterfaceVector.size())
                        {
                            cerr << "Mismatch in discovered Services. Reinitializing.";
                            init(m_wanCommonInterfaceSet);
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
