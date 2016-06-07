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

#include "MenuLight.h"

MenuLight::MenuLight() :
    m_quit(false), m_mutex()
{
    onFindResourceCb = std::bind(&MenuLight::onFindResource, this, std::placeholders::_1);
}

MenuLight::~MenuLight() {}

void MenuLight::init(std::set<std::shared_ptr<OC::OCResource>, OCResourceComp> lightSet)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_lightSet.clear();
    m_lightVector.clear();

    m_lightSet = lightSet;
    for (auto r : m_lightSet)
    {
        m_lightVector.push_back(r);
        m_light_instance_vector.push_back(Light(r));
    }
    print();
}

std::string MenuLight::getName()
{
    return "Light";
}

void MenuLight::print()
{
    std::cout << "------------------------------------" << std::endl;
    std::cout << getName() << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "find) Discover any lights" << std::endl;
    std::cout << "list) list currently discovered lights (" << m_lightVector.size() << " found)" <<
              std::endl;
    std::cout << "ison) ison <#> or ison all - find the current on/off state of the light" << std::endl;
    std::cout << "on) on <#> or on all - turn on selected light" << std::endl;
    std::cout << "off) off <#> or off all - turn off selected light" << std::endl;
    std::cout << "toggle) toggle <#> or toggle all - toggle the selected light" << std::endl;
    std::cout << "getb) getb <#> or getb all - get the brightness for the light" << std::endl;
    std::cout << "setb) setb <#> <0-100> or setb all - set the brightness %0-100  for the light" <<
              std::endl;
    std::cout << "clear) clear all discovered binary switch(es)" << std::endl;
    std::cout << "b) Back" << std::endl;
    std::cout << "h) Help" << std::endl;
    std::cout << "q) Quit" << std::endl;
}

void MenuLight::help()
{
    print();
}
void MenuLight::run(const std::vector<std::string> &cmd,
                    std::stack<std::unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if (cmd[0] == "find")
        {
            OCStackResult result = OC::OCPlatform::findResource("",
                                   std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_LIGHT, CT_DEFAULT,
                                   onFindResourceCb);
            std::cout << "findResource(" + std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" +
                      UPNP_OIC_TYPE_DEVICE_LIGHT + ") - " << result << std::endl;
        }
        else if (cmd[0] == "list")
        {
            for (size_t i = 0; i < m_lightVector.size(); ++i)
            {
                std::cout << "[" << i << "]" << std::endl;
                printResourceCompact(m_lightVector[i]);
            }
        }
        else if (cmd[0] == "ison")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_light_instance_vector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    m_light_instance_vector[i].isOnAsync([](const OC::HeaderOptions & headerOptions,
                                                            const OC::OCRepresentation & rep, const int eCode)
                    {
                        if (eCode == OC_STACK_OK)
                        {
                            bool powerState;
                            rep.getValue("value", powerState);
                            std::cout << "\t" << ((powerState) ? "is ON" : "is OFF") << std::endl;
                        }
                    });

                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto r : m_light_instance_vector)
                        {
                            r.isOnAsync([](const OC::HeaderOptions & headerOptions, const OC::OCRepresentation & rep,
                                           const int eCode)
                            {
                                if (eCode == OC_STACK_OK)
                                {
                                    bool powerState;
                                    rep.getValue("value", powerState);
                                    std::cout << "\t" << ((powerState) ? "is ON" : "is OFF") << std::endl;
                                }
                            });
                        }
                    }
                }
            }
            else
            {
                std::cout << "ison command requires an integer or key word all to work." << std::endl;
            }
        }
        else if (cmd[0] == "on")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_light_instance_vector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    m_light_instance_vector[i].turnOnAsync(true, [](const OC::HeaderOptions & headerOptions,
                    const OC::OCRepresentation & rep, const int eCode) {});
                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto r : m_light_instance_vector)
                        {
                            r.turnOnAsync(true, [](const OC::HeaderOptions & headerOptions, const OC::OCRepresentation & rep,
                            const int eCode) {});
                        }
                    }
                }
            }
            else
            {
                std::cout << "on command requires an integer or key word all to work." << std::endl;
            }
        }
        else if (cmd[0] == "off")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_light_instance_vector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    m_light_instance_vector[i].turnOnAsync(false, [](const OC::HeaderOptions & headerOptions,
                    const OC::OCRepresentation & rep, const int eCode) {});
                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto r : m_light_instance_vector)
                        {
                            r.turnOnAsync(false, [](const OC::HeaderOptions & headerOptions, const OC::OCRepresentation & rep,
                            const int eCode) {});
                        }
                    }
                }
            }
            else
            {
                std::cout << "off command requires an integer or key word all to work." << std::endl;
            }
        }
        else if (cmd[0] == "toggle")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_light_instance_vector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    Light l = m_light_instance_vector[i];
                    l.isOnAsync([l](const OC::HeaderOptions & headerOptions, const OC::OCRepresentation & rep,
                                    const int eCode)
                    {
                        if (eCode == OC_STACK_OK)
                        {
                            bool powerState;
                            rep.getValue("value", powerState);
                            l.turnOnAsync(!powerState, [](const OC::HeaderOptions & headerOptions,
                            const OC::OCRepresentation & rep, const int eCode) {});
                        }
                    });

                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto r : m_light_instance_vector)
                        {
                            r.isOnAsync([r](const OC::HeaderOptions & headerOptions, const OC::OCRepresentation & rep,
                                            const int eCode)
                            {
                                if (eCode == OC_STACK_OK)
                                {
                                    bool powerState;
                                    rep.getValue("value", powerState);
                                    r.turnOnAsync(!powerState, [](const OC::HeaderOptions & headerOptions,
                                    const OC::OCRepresentation & rep, const int eCode) {});
                                }
                            });
                        }
                    }
                }
            }
            else
            {
                std::cout << "ison command requires an integer or key word all to work." << std::endl;
            }
        }
        else if (cmd[0] == "getb")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_light_instance_vector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    m_light_instance_vector[i].getBrightnessAsync([](const OC::HeaderOptions & headerOptions,
                            const OC::OCRepresentation & rep, const int eCode)
                    {
                        if (eCode == OC_STACK_OK)
                        {
                            int brightness;
                            rep.getValue("brightness", brightness);
                            std::cout << "\t" << brightness << "%" << std::endl;
                        }
                    });

                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto r : m_light_instance_vector)
                        {
                            r.getBrightnessAsync([](const OC::HeaderOptions & headerOptions, const OC::OCRepresentation & rep,
                                                    const int eCode)
                            {
                                if (eCode == OC_STACK_OK)
                                {
                                    int brightness;
                                    rep.getValue("brightness", brightness);
                                    std::cout << "\t" << brightness << "%" << std::endl;
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
        else if (cmd[0] == "setb")
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
                    if (i >= m_light_instance_vector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    m_light_instance_vector[i].setBrightnessAsync(brightness, [](const OC::HeaderOptions &
                    headerOptions, const OC::OCRepresentation & rep, const int eCode) {});
                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto r : m_light_instance_vector)
                        {
                            r.setBrightnessAsync(brightness, [](const OC::HeaderOptions & headerOptions,
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
            m_lightSet.clear();
            m_lightVector.clear();
            m_light_instance_vector.clear();
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

bool MenuLight::quit()
{
    return m_quit;
}

void MenuLight::onFindResource(std::shared_ptr< OC::OCResource > resource)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "Found resource" << std::endl;
    //printResourceCompact(resource);
    printResourceInformation(resource);

    try
    {
        if (resource)
        {
            for (auto &resourceType : resource->getResourceTypes())
            {
                bool isNewServiceFound = false;
                if (resourceType == UPNP_OIC_TYPE_DEVICE_LIGHT)
                {
                    if (m_lightSet.find(resource) == m_lightSet.end())
                    {
                        m_lightSet.insert(resource);
                        m_lightVector.push_back(resource);
                        m_light_instance_vector.push_back(Light(resource));
                        if (m_lightSet.size() != m_lightVector.size())
                        {
                            std::cerr << "Missmatch in discovered devices. Reinitilizing.";
                            init(m_lightSet);
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
