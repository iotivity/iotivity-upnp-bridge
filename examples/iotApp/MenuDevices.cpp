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

#include "MenuDevices.h"
#include "MenuLight.h"

MenuDevices::MenuDevices():
    m_quit(false)
{
    onFindResourceCb = std::bind(&MenuDevices::onFindResource, this, std::placeholders::_1);
    print();
}

MenuDevices::~MenuDevices() {}

std::string MenuDevices::getName()
{
    return "Devices";
}

void MenuDevices::print()
{
    std::cout << "------------------------------------" << std::endl;
    std::cout << getName() << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "find) find all known devices" << std::endl;
    std::cout << "list) list all found devices" << std::endl;
    std::cout << "inetgateway) " << UPNP_OIC_TYPE_DEVICE_INET_GATEWAY << " (" << m_inetGateway.size() <<
              " found)" << std::endl;
    std::cout << "lan) " << UPNP_OIC_TYPE_DEVICE_LAN << " (" << m_lan.size() << " found)" << std::endl;
    std::cout << "light) " << UPNP_OIC_TYPE_DEVICE_LIGHT << " (" << m_light.size() << " found)" <<
              std::endl;
    std::cout << "mrenderer) " << UPNP_OIC_TYPE_DEVICE_MEDIA_RENDERER << " (" << m_mediaRenderer.size()
              << " found)" << std::endl;
    std::cout << "mserver) " << UPNP_OIC_TYPE_DEVICE_MEDIA_SERVER << " (" << m_mediaServer.size() <<
              " found)" << std::endl;
    std::cout << "wan) " << UPNP_OIC_TYPE_DEVICE_WAN << " (" << m_wan.size() << " found)" << std::endl;
    std::cout << "wanconn) " << UPNP_OIC_TYPE_DEVICE_WAN_CONNECTION << " (" << m_wanConnection.size() <<
              " found)" << std::endl;
    std::cout << "clear) clear all found devices" << std::endl;
    std::cout << "b) Back" << std::endl;
    std::cout << "h) Help" << std::endl;
    std::cout << "q) Quit" << std::endl;
}

void MenuDevices::help()
{
    print();
}
void MenuDevices::run(const std::vector<std::string> &cmd,
                      std::stack<std::unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if (cmd[0] == "find")
        {
            std::vector<std::string> knownDevices =
            {
                // devices
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_LIGHT,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_INET_GATEWAY,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_WAN,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_WAN_CONNECTION,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_LAN,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_MEDIA_RENDERER,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_MEDIA_SERVER
            };
            for (auto device : knownDevices)
            {
                OCStackResult result = OC::OCPlatform::findResource("", device, CT_DEFAULT, onFindResourceCb);
                std::cout << "findResource(" + device + ") - " << result << std::endl;
            }

        }
        else if (cmd[0] == "list")
        {
            if (!m_inetGateway.empty())
            {
                std::cout << "Internet Gateway Devices(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_inetGateway)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_lan.empty())
            {
                std::cout << "LAN Devices(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_lan)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_light.empty())
            {
                std::cout << "Light Devices(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_light)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_mediaRenderer.empty())
            {
                std::cout << "Media Renderer Device(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_mediaRenderer)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_mediaServer.empty())
            {
                std::cout << "Media Server Devices(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_mediaServer)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_wan.empty())
            {
                std::cout << "WAN Devices(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_wan)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
            if (!m_wanConnection.empty())
            {
                std::cout << "WAN Connection Device(s) found:" << std::endl;
                std::cout << "---------------------------------------------" << std::endl;
                for ( auto r : m_wanConnection)
                {
                    printResourceCompact(r);
                    std::cout << std::endl;
                }
            }
        }
        else if ("inetgateway" == cmd[0])
        {
            std::cout << "Internet Gateway Menu not yet Implemented" << std::endl;
        }
        else if ("lan" == cmd[0])
        {
            std::cout << "LAN Menu not yet Implemented" << std::endl;
        }
        else if ("light" == cmd[0])
        {
            std::unique_ptr<MenuLight> menuItem(new MenuLight);
            menuItem->init(m_light);
            menuStack.push(std::move(menuItem));
        }
        else if ("mrenderer" == cmd[0])
        {
            std::cout << "Media Renderer Menu not yet Implemented" << std::endl;
        }
        else if ("mserver" == cmd[0])
        {
            std::cout << "Media Server Menu not yet Implemented" << std::endl;
        }
        else if ("wan" == cmd[0])
        {
            std::cout << "WAN Menu not yet Implemented" << std::endl;
        }
        else if ("wanconn" == cmd[0])
        {
            std::cout << "WAN Connection Menu not yet Implemented" << std::endl;
        }
        else if ("clear" == cmd[0])
        {
            m_inetGateway.clear();
            m_lan.clear();
            m_light.clear();
            m_mediaRenderer.clear();
            m_mediaServer.clear();
            m_wan.clear();
            m_wanConnection.clear();
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

bool MenuDevices::quit()
{
    return m_quit;
}

void MenuDevices::onFindResource(std::shared_ptr< OC::OCResource > resource)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    try
    {
        if (resource)
        {
            for (auto &resourceType : resource->getResourceTypes())
            {
                std::cout << "Found "  << resourceType << " resource" << std::endl;
                if (resourceType == UPNP_OIC_TYPE_DEVICE_INET_GATEWAY)
                {
                    if (m_inetGateway.find(resource) == m_inetGateway.end())
                    {
                        m_inetGateway.insert(resource);
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_DEVICE_LAN)
                {
                    if (m_lan.find(resource) == m_lan.end())
                    {
                        m_lan.insert(resource);
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_DEVICE_LIGHT)
                {
                    if (m_light.find(resource) == m_light.end())
                    {
                        m_light.insert(resource);
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_DEVICE_MEDIA_RENDERER)
                {
                    if (m_mediaRenderer.find(resource) == m_mediaRenderer.end())
                    {
                        m_mediaRenderer.insert(resource);
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_DEVICE_MEDIA_SERVER)
                {
                    if (m_mediaServer.find(resource) == m_mediaServer.end())
                    {
                        m_mediaServer.insert(resource);
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_DEVICE_WAN)
                {
                    if (m_wan.find(resource) == m_wan.end())
                    {
                        m_wan.insert(resource);
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_DEVICE_WAN_CONNECTION)
                {
                    if (m_wanConnection.find(resource) == m_wanConnection.end())
                    {
                        m_wanConnection.insert(resource);
                    }
                }
            }
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Caught exception in foundResource: " << e.what() << std::endl;
    }
}
