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

#include <UpnpConstants.h>

#include "ConnectionManager.h"
#include "MenuConnectionManager.h"

MenuConnectionManager::MenuConnectionManager() :
    m_quit(false), m_mutex()
{
    onFindResourceCb = std::bind(&MenuConnectionManager::onFindResource, this, std::placeholders::_1);

}

MenuConnectionManager::~MenuConnectionManager()
{
}

void MenuConnectionManager::init(std::set<OC::OCResource::Ptr, OCResourceComp> brightnessSet)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_connectionManagerSet.clear();
    m_connectionManagerVector.clear();

    m_connectionManagerSet = brightnessSet;
    for (auto r : m_connectionManagerSet)
    {
        m_connectionManagerVector.push_back(r);
    }
    print();
}

std::string MenuConnectionManager::getName()
{
    return "Connection Manager";
}

void MenuConnectionManager::print()
{
    std::cout << "------------------------------------" << std::endl;
    std::cout << getName() << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "find) Discover any connection manager service(s) " << std::endl;
    std::cout << "list) list currently discovered connection manager services (" << m_connectionManagerVector.size() <<
              " found)" << std::endl;
    std::cout << "getpi) getpi <#> or getpi all - get the get the protocol info" << std::endl;
    std::cout << "getccid) getccid <#> or getccid all - get the current connection IDs" << std::endl;
    std::cout << "getcci) getcci <#> [connectionID] or getcci all - get the current connection Info" << std::endl;
    std::cout << "        if connectionID is not provided, connectionID '0' will be used by default." << std::endl;
    std::cout << "clear) clear all discovered connection manager service(s)" << std::endl;
    std::cout << "b) Back" << std::endl;
    std::cout << "h) Help" << std::endl;
    std::cout << "q) Quit" << std::endl;
}

void MenuConnectionManager::help()
{
    print();
}
void MenuConnectionManager::run(const std::vector<std::string> &cmd,
                         std::stack<std::unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if (cmd[0] == "find")
        {
            OCStackResult result = OC::OCPlatform::findResource("",
                                   std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_CONNECTION_MANAGER, CT_DEFAULT,
                                   onFindResourceCb);
            std::cout << "findResource(" + std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" +
                    UPNP_OIC_TYPE_CONNECTION_MANAGER + ") - " << result << std::endl;
        }
        else if (cmd[0] == "list")
        {
            for (size_t i = 0; i < m_connectionManagerVector.size(); ++i)
            {
                std::cout << "[" << i << "]" << std::endl;
                printResourceCompact(m_connectionManagerVector[i]);
            }
        }
        else if (cmd[0] == "getpi")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_connectionManagerVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    ConnectionManager cm(m_connectionManagerVector[i]);
                    ConnectionManager::ProtocolInfo pi = cm.getProtocolInfo();
                    std::cout << m_connectionManagerVector[i]->uri() << std::endl;
                    std::cout << "Protocol Info:" << std::endl;
                    std::cout << "\tsink: " << pi.sink << std::endl;
                    std::cout << "\tsource :" << pi.source << std::endl << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto c : m_connectionManagerVector)
                        {
                            ConnectionManager cm(c);
                            ConnectionManager::ProtocolInfo pi = cm.getProtocolInfo();
                            std::cout << c->uri() << std::endl;
                            std::cout << "Protocol Info:" << std::endl;
                            std::cout << "\tsink: " << pi.sink << std::endl;
                            std::cout << "\tsource :" << pi.source << std::endl << std::endl;
                        }
                    }
                }
            }
            else
            {
                std::cout << "getpi command requires an integer or key word all to work." << std::endl;
            }
        }
        else if ("getccid" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_connectionManagerVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    ConnectionManager cm(m_connectionManagerVector[i]);
                    std::string currentConnIds = cm.getCurrentConnectionIDs();
                    std::cout << m_connectionManagerVector[i]->uri() << std::endl;
                    std::cout << "\tCurrent Connection IDs: " << currentConnIds << std::endl << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto c : m_connectionManagerVector)
                        {
                            ConnectionManager cm(c);
                            std::string currentConnIds = cm.getCurrentConnectionIDs();
                            std::cout << c->uri() << std::endl;
                            std::cout << "\tCurrent Connection IDs: " << currentConnIds << std::endl << std::endl;
                        }
                    }
                }
            }
            else
            {
                std::cout << "getccid command requires an integer or key word all to work." << std::endl;
            }
        }
        else if (cmd[0] == "getcci")
        {
            // If a connection ID is not provided use "0"
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_connectionManagerVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    ConnectionManager cm(m_connectionManagerVector[i]);
                    ConnectionManager::ConnectionInfo ci = cm.getConnectionInfo(cmd[2]);
                    std::cout << m_connectionManagerVector[i]->uri() << std::endl;
                    std::cout << "Connection Info:" << std::endl;
                    std::cout << "\tRcsID: " << ci.rcsId << std::endl;
                    std::cout << "\tAVTransportID :" << ci.avTransportId << std::endl;
                    std::cout << "\tProtocolInfo :" << ci.protocolInfo << std::endl;
                    std::cout << "\tPeerConnectionManager :" << ci.peerConnectionManager << std::endl;
                    std::cout << "\tPeerConnectionID :" << ci.peerConnectionId << std::endl;
                    std::cout << "\tDirection :" << ci.direction << std::endl;
                    std::cout << "\tStatus :" << ci.status << std::endl << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "getcci command required valid resource and a connectionID" << std::endl;
                }
            }
            else if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_connectionManagerVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    ConnectionManager cm(m_connectionManagerVector[i]);
                    ConnectionManager::ConnectionInfo ci = cm.getConnectionInfo();
                    std::cout << m_connectionManagerVector[i]->uri() << std::endl;
                    std::cout << "Connection Info:" << std::endl;
                    std::cout << "\tRcsID: " << ci.rcsId << std::endl;
                    std::cout << "\tAVTransportID :" << ci.avTransportId << std::endl;
                    std::cout << "\tProtocolInfo :" << ci.protocolInfo << std::endl;
                    std::cout << "\tPeerConnectionManager :" << ci.peerConnectionManager << std::endl;
                    std::cout << "\tPeerConnectionID :" << ci.peerConnectionId << std::endl;
                    std::cout << "\tDirection :" << ci.direction << std::endl;
                    std::cout << "\tStatus :" << ci.status << std::endl << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto c : m_connectionManagerVector)
                        {
                            ConnectionManager cm(c);
                            ConnectionManager::ConnectionInfo ci = cm.getConnectionInfo();
                            std::cout << c->uri() << std::endl;
                            std::cout << "Connection Info:" << std::endl;
                            std::cout << "\tRcsID: " << ci.rcsId << std::endl;
                            std::cout << "\tAVTransportID :" << ci.avTransportId << std::endl;
                            std::cout << "\tProtocolInfo :" << ci.protocolInfo << std::endl;
                            std::cout << "\tPeerConnectionManager :" << ci.peerConnectionManager << std::endl;
                            std::cout << "\tPeerConnectionID :" << ci.peerConnectionId << std::endl;
                            std::cout << "\tDirection :" << ci.direction << std::endl;
                            std::cout << "\tStatus :" << ci.status << std::endl << std::endl;
                        }
                    }
                }
            }
            else
            {
                std::cout << "getcci command requires an integer or key word all to work." << std::endl;
            }
        }
        else if (cmd[0] == "clear")
        {
            m_connectionManagerSet.clear();
            m_connectionManagerVector.clear();
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
            std::cout << "unknown command." << std::endl;
            print();
        }
    }
}

bool MenuConnectionManager::quit()
{
    return m_quit;
}

void MenuConnectionManager::onFindResource(std::shared_ptr< OC::OCResource > resource)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "Found resource" << std::endl;
    printResourceCompact(resource);

    try
    {
        if (resource)
        {
            for (auto &resourceType : resource->getResourceTypes())
            {
                bool isNewServiceFound = false;
                if (resourceType == UPNP_OIC_TYPE_CONNECTION_MANAGER)
                {
                    if (m_connectionManagerSet.find(resource) == m_connectionManagerSet.end())
                    {
                        m_connectionManagerSet.insert(resource);
                        m_connectionManagerVector.push_back(resource);
                        if (m_connectionManagerSet.size() != m_connectionManagerVector.size())
                        {
                            std::cerr << "Missmatch in discovered Services. Reinitilizing.";
                            init(m_connectionManagerSet);
                        }
                        isNewServiceFound = true;
                    }
                }
                if (isNewServiceFound)
                {
                    print();
                }
            }
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Caught exception in foundResource: " << e.what() << std::endl;
    }
}
