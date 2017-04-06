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

#include "Brightness.h"
#include "MenuBrightness.h"

#include <UpnpConstants.h>


MenuBrightness::MenuBrightness() :
    m_quit(false), m_mutex()
{
    onFindResourceCb = std::bind(&MenuBrightness::onFindResource, this, std::placeholders::_1);
}

MenuBrightness::~MenuBrightness() {}

void MenuBrightness::init(std::set<std::shared_ptr<OC::OCResource>, OCResourceComp> brightnessSet)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_brightnessSet.clear();
    m_brightnessVector.clear();

    m_brightnessSet = brightnessSet;
    for (auto r : m_brightnessSet)
    {
        m_brightnessVector.push_back(r);
    }
    print();
}

std::string MenuBrightness::getName()
{
    return "Brightness";
}

void MenuBrightness::print()
{
    std::cout << "------------------------------------" << std::endl;
    std::cout << getName() << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "find) Discover any brightness services " << std::endl;
    std::cout << "list) list currently discovered brightness services (" << m_brightnessVector.size() <<
              " found)" << std::endl;
    std::cout << "get) get <#> or get all - get the brightness" << std::endl;
    std::cout << "set) set <#> <0-100> or set all - set the brightness %0-100" << std::endl;
    std::cout << "clear) clear all discovered brightness service(s)" << std::endl;
    std::cout << "b) Back" << std::endl;
    std::cout << "h) Help" << std::endl;
    std::cout << "q) Quit" << std::endl;
}

void MenuBrightness::help()
{
    print();
}
void MenuBrightness::run(const std::vector<std::string> &cmd,
                         std::stack<std::unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if (cmd[0] == "find")
        {
            OCStackResult result = OC::OCPlatform::findResource("",
                                   std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_BRIGHTNESS, CT_DEFAULT,
                                   onFindResourceCb);
            std::cout << "findResource(" + std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" +
                      UPNP_OIC_TYPE_BRIGHTNESS + ") - " << result << std::endl;
        }
        else if (cmd[0] == "list")
        {
            for (size_t i = 0; i < m_brightnessVector.size(); ++i)
            {
                std::cout << "[" << i << "]" << std::endl;
                printResourceCompact(m_brightnessVector[i]);
            }
        }
        else if (cmd[0] == "get")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_brightnessVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    Brightness b(m_brightnessVector[i]);
                    b.getBrightnessAsync([b](const OC::HeaderOptions & headerOptions, const OC::OCRepresentation & rep,
                                             const int eCode)
                    {
                        if (eCode == OC_STACK_OK)
                        {
                            int brightness;
                            std::cout << rep.getUri() << std::endl;
                            if (rep.getUri() == b.getResource()->uri())
                            {
                                rep.getValue("brightness", brightness);
                                std::cout << "\t" << brightness << "%" << std::endl;
                            }
                            else
                            {
                                std::cout << "\tUNKNOWN" << std::endl;
                            }

                        }
                    });

                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto r : m_brightnessVector)
                        {
                            Brightness b(r);
                            b.getBrightnessAsync([b](const OC::HeaderOptions & headerOptions, const OC::OCRepresentation & rep,
                                                     const int eCode)
                            {
                                if (eCode == OC_STACK_OK)
                                {
                                    int brightness;
                                    std::cout << rep.getUri() << std::endl;
                                    if (rep.getUri() == b.getResource()->uri())
                                    {
                                        rep.getValue("brightness", brightness);
                                        std::cout << "\t" << brightness << "%" << std::endl;
                                    }
                                    else
                                    {
                                        std::cout << "\tUNKNOWN" << std::endl;
                                    }
                                }
                            });
                        }
                    }
                }
            }
            else
            {
                std::cout << "get command requires an integer or key word all to work." << std::endl;
            }
        }
        else if (cmd[0] == "set")
        {
            if (cmd.size() > 2)
            {
                int brightness = 0;
                try
                {
                    brightness = std::stoi(cmd[2]);
                    if (brightness < 0 || brightness > 100)
                    {
                        std::cout << "Invalid brightness value must be between 0 and 100" << std::endl;
                        return;
                    }
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "set command requires an integer value for brightness." << std::endl;
                    return;
                }
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_brightnessVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    Brightness b(m_brightnessVector[i]);
                    b.setBrightnessAsync(brightness, [](const OC::HeaderOptions & headerOptions,
                    const OC::OCRepresentation & rep, const int eCode) {});
                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto r : m_brightnessVector)
                        {
                            Brightness b(r);
                            b.setBrightnessAsync(brightness, [](const OC::HeaderOptions & headerOptions,
                            const OC::OCRepresentation & rep, const int eCode) {});
                        }
                    }
                }
            }
            else
            {
                std::cout << "set command requires an integer or key word all to work." << std::endl;
            }
        }
        else if (cmd[0] == "clear")
        {
            m_brightnessSet.clear();
            m_brightnessVector.clear();
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

bool MenuBrightness::quit()
{
    return m_quit;
}

void MenuBrightness::onFindResource(std::shared_ptr< OC::OCResource > resource)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "Found resource" << std::endl;

    try
    {
        if (resource)
        {
            for (auto &resourceType : resource->getResourceTypes())
            {
                if (resourceType == UPNP_OIC_TYPE_BRIGHTNESS)
                {
                    if (m_brightnessSet.find(resource) == m_brightnessSet.end())
                    {
                        m_brightnessSet.insert(resource);
                        m_brightnessVector.push_back(resource);
                        if (m_brightnessSet.size() != m_brightnessVector.size())
                        {
                            std::cerr << "Mismatch in discovered Services. Reinitilizing.";
                            init(m_brightnessSet);
                        }
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
