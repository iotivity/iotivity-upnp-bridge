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

#include "MenuBinarySwitch.h"
#include "MenuBrightness.h"
#include "MenuConnectionManager.h"
#include "MenuContentDirectory.h"
#include "MenuServices.h"

MenuServices::MenuServices():
    m_quit(false)
{
    onFindResourceCb = std::bind(&MenuServices::onFindResource, this, std::placeholders::_1);
    print();
}

MenuServices::~MenuServices() {}

std::string MenuServices::getName()
{
    return "Services";
}

void MenuServices::print()
{
    std::cout << "------------------------------------" << std::endl;
    std::cout << getName() << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "find) find all known services" << std::endl;
    std::cout << "list) list all found services" << std::endl;
    std::cout << "av) " << UPNP_OIC_TYPE_AV_TRANSPORT << " (" << m_avTransport.size() << " found)" <<
              std::endl;
    std::cout << "brightness) " << UPNP_OIC_TYPE_BRIGHTNESS << " (" << m_brightness.size() << " found)"
              << std::endl;
    std::cout << "switch) " << UPNP_OIC_TYPE_POWER_SWITCH << " (" << m_binarySwitch.size() << " found)"
              << std::endl;
    std::cout << "connection) " << UPNP_OIC_TYPE_CONNECTION_MANAGER << " (" <<
              m_connectionManager.size() << " found)" << std::endl;
    std::cout << "content) " << UPNP_OIC_TYPE_CONTENT_DIRECTORY << " (" << m_contentDirectory.size() <<
              " found)" << std::endl;
    std::cout << "layer3) " << UPNP_OIC_TYPE_LAYER3_FORWARDING << " (" << m_layer3Forwarding.size() <<
              " found)" << std::endl;
    std::cout << "rendering) " << UPNP_OIC_TYPE_RENDERING_CONTROL << " (" << m_renderingControl.size()
              << " found)" << std::endl;
    std::cout << "recording) " << UPNP_OIC_TYPE_SCHEDULED_RECORDING << " (" <<
              m_scheduledRecording.size()
              << " found)" << std::endl;
    std::cout << "ifconfig) " << UPNP_OIC_TYPE_WAN_IF_CONFIG << " (" << m_wanIfConfig.size() <<
              " found)" << std::endl;
    std::cout << "cableconfig) " << UPNP_OIC_TYPE_WAN_CABLE_LINK_CONFIG << " (" <<
              m_wanCableLinkConfig.size() <<
              " found)" << std::endl;
    std::cout << "dslconfig) " << UPNP_OIC_TYPE_WAN_DSL_LINK_CONFIG << " (" << m_wanDSLLinkConfig.size()
              <<
              " found)" << std::endl;
    std::cout << "netconfig) " << UPNP_OIC_TYPE_WAN_ETHERNET_CONFIG << " (" <<
              m_wanEthernetConfig.size() <<
              " found)" << std::endl;
    std::cout << "potsconfig) " << UPNP_OIC_TYPE_WAN_POTS_LINK_CONFIG << " (" <<
              m_wanPotsLinkConfig.size() <<
              " found)" << std::endl;
    std::cout << "ipconnection) " << UPNP_OIC_TYPE_WAN_IP_CONNECTION << " (" << m_wanIPConnection.size()
              <<
              " found)" << std::endl;
    std::cout << "lanhost) " << UPNP_OIC_TYPE_LAN_HOST_CONFIG << " (" << m_lanHostConfig.size() <<
              " found)" << std::endl;
    std::cout << "clear) clear all found services" << std::endl;
    std::cout << "b) Back" << std::endl;
    std::cout << "h) Help" << std::endl;
    std::cout << "q) Quit" << std::endl;
}

void MenuServices::help()
{
    print();
}
void MenuServices::run(const std::vector<std::string> &cmd,
                       std::stack<std::unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if (cmd[0] == "find")
        {
            std::vector<std::string> knownServices =
            {
                // services
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_BRIGHTNESS,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_POWER_SWITCH,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_CONNECTION_MANAGER,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_AV_TRANSPORT,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_CONTENT_DIRECTORY,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_RENDERING_CONTROL,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_SCHEDULED_RECORDING,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_WAN_IF_CONFIG,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_LAYER3_FORWARDING,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_WAN_CABLE_LINK_CONFIG,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_WAN_DSL_LINK_CONFIG,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_WAN_ETHERNET_CONFIG,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_WAN_POTS_LINK_CONFIG,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_WAN_IP_CONNECTION,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_LAN_HOST_CONFIG

            };
            for (auto service : knownServices)
            {
                OCStackResult result = OC::OCPlatform::findResource("", service, CT_DEFAULT, onFindResourceCb);
                std::cout << "findResource(" + service + ") - " << result << std::endl;
            }

        }
        else if (cmd[0] == "list")
        {
            if (!m_avTransport.empty())
            {
                std::cout << "AV Transport Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_avTransport)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_brightness.empty())
            {
                std::cout << "Brightness Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_brightness)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_binarySwitch.empty())
            {
                std::cout << "Binary Switch Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_binarySwitch)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_connectionManager.empty())
            {
                std::cout << "Connection Manager Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_connectionManager)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_contentDirectory.empty())
            {
                std::cout << "Content Dirctory Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_contentDirectory)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_layer3Forwarding.empty())
            {
                std::cout << "Layer 3 Forwarding Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_layer3Forwarding)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_renderingControl.empty())
            {
                std::cout << "Rendering Control Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_renderingControl)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_scheduledRecording.empty())
            {
                std::cout << "Scheduled Recording Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_scheduledRecording)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_wanIfConfig.empty())
            {
                std::cout << "WAN IF Config Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_wanIfConfig)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_wanCableLinkConfig.empty())
            {
                std::cout << "WAN Cable Link Config Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_wanCableLinkConfig)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_wanDSLLinkConfig.empty())
            {
                std::cout << "WAN DSL Link Config Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_wanDSLLinkConfig)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_wanEthernetConfig.empty())
            {
                std::cout << "WAN Ethernet Config Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_wanEthernetConfig)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_wanPotsLinkConfig.empty())
            {
                std::cout << "WAN POTS Config Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_wanPotsLinkConfig)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_wanIPConnection.empty())
            {
                std::cout << "WAN IP Connection Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_wanIPConnection)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_lanHostConfig.empty())
            {
                std::cout << "LAN Host Config Service(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_lanHostConfig)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
        }
        else if ("av" == cmd[0])
        {
            std::cout << "AV Transport Menu not yet Implemented" << std::endl;
        }
        else if ("brightness" == cmd[0])
        {
            std::unique_ptr<MenuBrightness> menuItem(new MenuBrightness);
            menuItem->init(m_brightness);
            menuStack.push(std::move(menuItem));
        }
        else if ("switch" == cmd[0])
        {
            std::unique_ptr<MenuBinarySwitch> menuItem(new MenuBinarySwitch);
            menuItem->init(m_binarySwitch);
            menuStack.push(std::move(menuItem));
        }
        else if ("connection" == cmd[0])
        {
            std::unique_ptr<MenuConnectionManager> menuItem(new MenuConnectionManager);
            menuItem->init(m_connectionManager);
            menuStack.push(std::move(menuItem));
        }
        else if ("content" == cmd[0])
        {
            std::unique_ptr<MenuContentDirectory> menuItem(new MenuContentDirectory);
            menuItem->init(m_contentDirectory);
            menuStack.push(std::move(menuItem));
        }
        else if ("layer3" == cmd[0])
        {
            std::cout << "Layer 3 Forwarding Menu not yet Implemented" << std::endl;
        }
        else if ("rendering" == cmd[0])
        {
            std::cout << "Rendering Control Menu not yet Implemented" << std::endl;
        }
        else if ("recording" == cmd[0])
        {
            std::cout << "Scheduled Recording Menu not yet Implemented" << std::endl;
        }
        else if ("ifconfig" == cmd[0])
        {
            std::cout << "WAN IF Config Menu not yet Implemented" << std::endl;
        }
        else if ("cableconfig" == cmd[0])
        {
            std::cout << "WAN Cable Link Config Menu not yet Implemented" << std::endl;
        }
        else if ("dslconfig" == cmd[0])
        {
            std::cout << "WAN DSL Link Config Menu not yet Implemented" << std::endl;
        }
        else if ("netconfig" == cmd[0])
        {
            std::cout << "WAN Ethernet Config Menu not yet Implemented" << std::endl;
        }
        else if ("potsconfig" == cmd[0])
        {
            std::cout << "WAN POTS Config Menu not yet Implemented" << std::endl;
        }
        else if ("ipconnection" == cmd[0])
        {
            std::cout << "WAN IP Connection Menu not yet Implemented" << std::endl;
        }
        else if ("lanhost" == cmd[0])
        {
            std::cout << "LAN Host Config Menu not yet Implemented" << std::endl;
        }
        else if ("clear" == cmd[0])
        {
            m_avTransport.clear();
            m_binarySwitch.clear();
            m_brightness.clear();
            m_connectionManager.clear();
            m_contentDirectory.clear();
            m_layer3Forwarding.clear();
            m_renderingControl.clear();
            m_scheduledRecording.clear();
            m_wanIfConfig.clear();
            print();
        }
        else if ("q" == cmd[0])
        {
            m_quit = true;
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

bool MenuServices::quit()
{
    return m_quit;
}

void MenuServices::onFindResource(OC::OCResource::Ptr resource)
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
                if (resourceType == UPNP_OIC_TYPE_AV_TRANSPORT)
                {
                    if (m_avTransport.find(resource) == m_avTransport.end())
                    {
                        m_avTransport.insert(resource);
                        isNewServiceFound = true;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_POWER_SWITCH)
                {
                    if (m_binarySwitch.find(resource) == m_binarySwitch.end())
                    {
                        m_binarySwitch.insert(resource);
                        isNewServiceFound = true;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_BRIGHTNESS)
                {
                    if (m_brightness.find(resource) == m_brightness.end())
                    {
                        m_brightness.insert(resource);
                        isNewServiceFound = true;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_CONNECTION_MANAGER)
                {
                    if (m_connectionManager.find(resource) == m_connectionManager.end())
                    {
                        m_connectionManager.insert(resource);
                        isNewServiceFound = true;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_CONTENT_DIRECTORY)
                {
                    if (m_contentDirectory.find(resource) == m_contentDirectory.end())
                    {
                        m_contentDirectory.insert(resource);
                        isNewServiceFound = true;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_LAYER3_FORWARDING)
                {
                    if (m_layer3Forwarding.find(resource) == m_layer3Forwarding.end())
                    {
                        m_layer3Forwarding.insert(resource);
                        isNewServiceFound = true;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_RENDERING_CONTROL)
                {
                    if (m_renderingControl.find(resource) == m_renderingControl.end())
                    {
                        m_renderingControl.insert(resource);
                        isNewServiceFound = true;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_SCHEDULED_RECORDING)
                {
                    if (m_scheduledRecording.find(resource) == m_scheduledRecording.end())
                    {
                        m_scheduledRecording.insert(resource);
                        isNewServiceFound = true;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_WAN_IF_CONFIG)
                {
                    if (m_wanIfConfig.find(resource) == m_wanIfConfig.end())
                    {
                        m_wanIfConfig.insert(resource);
                        isNewServiceFound = true;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_WAN_CABLE_LINK_CONFIG)
                {
                    if (m_wanCableLinkConfig.find(resource) == m_wanCableLinkConfig.end())
                    {
                        m_wanCableLinkConfig.insert(resource);
                        isNewServiceFound = true;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_WAN_DSL_LINK_CONFIG)
                {
                    if (m_wanDSLLinkConfig.find(resource) == m_wanDSLLinkConfig.end())
                    {
                        m_wanDSLLinkConfig.insert(resource);
                        isNewServiceFound = true;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_WAN_ETHERNET_CONFIG)
                {
                    if (m_wanEthernetConfig.find(resource) == m_wanEthernetConfig.end())
                    {
                        m_wanEthernetConfig.insert(resource);
                        isNewServiceFound = true;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_WAN_POTS_LINK_CONFIG)
                {
                    if (m_wanPotsLinkConfig.find(resource) == m_wanPotsLinkConfig.end())
                    {
                        m_wanPotsLinkConfig.insert(resource);
                        isNewServiceFound = true;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_WAN_IP_CONNECTION)
                {
                    if (m_wanIPConnection.find(resource) == m_wanIPConnection.end())
                    {
                        m_wanIPConnection.insert(resource);
                        isNewServiceFound = true;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_LAN_HOST_CONFIG)
                {
                    if (m_lanHostConfig.find(resource) == m_lanHostConfig.end())
                    {
                        m_lanHostConfig.insert(resource);
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
