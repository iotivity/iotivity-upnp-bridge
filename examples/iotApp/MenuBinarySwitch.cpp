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
#include "BinarySwitch.h"
#include "MenuBinarySwitch.h"

MenuBinarySwitch::MenuBinarySwitch() :
    m_quit(false), m_mutex()
{
    onFindResourceCb = std::bind(&MenuBinarySwitch::onFindResource, this, std::placeholders::_1);
}

MenuBinarySwitch::~MenuBinarySwitch() {}

void MenuBinarySwitch::init(std::set<std::shared_ptr<OC::OCResource>, OCResourceComp>
                            binarySwitchSet)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_binarySwitchSet.clear();
    m_binarySwitchVector.clear();

    m_binarySwitchSet = binarySwitchSet;
    for (auto r : m_binarySwitchSet)
    {
        m_binarySwitchVector.push_back(r);
    }
    print();
}

std::string MenuBinarySwitch::getName()
{
    return "Binary Switch";
}

void MenuBinarySwitch::print()
{
    std::cout << "------------------------------------" << std::endl;
    std::cout << getName() << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "find) Discover any binary switch " << std::endl;
    std::cout << "list) list currently discovered binary switch services (" <<
              m_binarySwitchVector.size() << " found)" << std::endl;
    std::cout << "ison) ison <#> or ison all - find the current switch state" << std::endl;
    std::cout << "on) on <#> or on all - turn on selected switch" << std::endl;
    std::cout << "off) off <#> or off all - turn off selected switch" << std::endl;
    std::cout << "toggle) toggle <#> or toggle all - toggle the selected switch" << std::endl;
    std::cout << "clear) clear all discovered binary switch(es)" << std::endl;
    std::cout << "b) Back" << std::endl;
    std::cout << "h) Help" << std::endl;
    std::cout << "q) Quit" << std::endl;
}

void MenuBinarySwitch::help()
{
    print();
}
void MenuBinarySwitch::run(const std::vector<std::string> &cmd,
                           std::stack<std::unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if (cmd[0] == "find")
        {
            OCStackResult result = OC::OCPlatform::findResource("",
                                   std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_POWER_SWITCH, CT_DEFAULT,
                                   onFindResourceCb);
            std::cout << "findResource(" + std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" +
                      UPNP_OIC_TYPE_POWER_SWITCH + ") - " << result << std::endl;
        }
        else if (cmd[0] == "list")
        {
            for (size_t i = 0; i < m_binarySwitchVector.size(); ++i)
            {
                std::cout << "[" << i << "]" << std::endl;
                printResourceCompact(m_binarySwitchVector[i]);
            }
        }
        else if (cmd[0] == "ison")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_binarySwitchVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    BinarySwitch bswitch(m_binarySwitchVector[i]);
                    bswitch.isOnAsync([bswitch](const OC::HeaderOptions & headerOptions,
                                                const OC::OCRepresentation & rep, const int eCode)
                    {
                        if (eCode == OC_STACK_OK)
                        {
                            bool powerState;
                            std::cout << rep.getUri() << std::endl;
                            if (rep.getUri() == bswitch.getResource()->uri())
                            {
                                rep.getValue("value", powerState);
                                std::cout << "\t" << ((powerState) ? "is ON" : "is OFF") << std::endl;
                            }
                            else
                            {
                                std::cout << "\t" << "is UNKNOWN" << std::endl;
                            }
                        }
                    });

                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto r : m_binarySwitchVector)
                        {
                            BinarySwitch b(r);
                            b.isOnAsync([b](const OC::HeaderOptions & headerOptions, const OC::OCRepresentation & rep,
                                            const int eCode)
                            {
                                if (eCode == OC_STACK_OK)
                                {
                                    bool powerState;
                                    std::cout << rep.getUri() << std::endl;
                                    if (rep.getUri() == b.getResource()->uri())
                                    {
                                        rep.getValue("value", powerState);
                                        std::cout << "\t" << ((powerState) ? "is ON" : "is OFF") << std::endl;
                                    }
                                    else
                                    {
                                        std::cout << "\t" << "is UNKNOWN" << std::endl;
                                    }
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
                    if (i >= m_binarySwitchVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    BinarySwitch bswitch(m_binarySwitchVector[i]);
                    bswitch.turnOnAsync(true, [](const OC::HeaderOptions & headerOptions,
                    const OC::OCRepresentation & rep, const int eCode) {});
                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto r : m_binarySwitchVector)
                        {
                            BinarySwitch b(r);
                            b.turnOnAsync(true, [](const OC::HeaderOptions & headerOptions, const OC::OCRepresentation & rep,
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
                    if (i >= m_binarySwitchVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    BinarySwitch bswitch(m_binarySwitchVector[i]);
                    bswitch.turnOnAsync(false, [](const OC::HeaderOptions & headerOptions,
                    const OC::OCRepresentation & rep, const int eCode) {});
                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto r : m_binarySwitchVector)
                        {
                            BinarySwitch b(r);
                            b.turnOnAsync(false, [](const OC::HeaderOptions & headerOptions, const OC::OCRepresentation & rep,
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
                    if (i >= m_binarySwitchVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    BinarySwitch bswitch(m_binarySwitchVector[i]);
                    bswitch.isOnAsync([bswitch](const OC::HeaderOptions & headerOptions,
                                                const OC::OCRepresentation & rep, const int eCode)
                    {
                        if (eCode == OC_STACK_OK)
                        {
                            bool powerState;
                            std::string uri;
                            rep.getValue("uri", uri);
                            if (uri == bswitch.getResource()->uri())
                            {
                                rep.getValue("value", powerState);
                            }
                            //std::cout << uri << std::endl;
                            //std::cout << "\t" << ((powerState) ? "is ON" : "is OFF") << std::endl;
                            bswitch.turnOnAsync(!powerState, [](const OC::HeaderOptions & headerOptions,
                            const OC::OCRepresentation & rep, const int eCode) {});
                        }
                    });

                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto r : m_binarySwitchVector)
                        {
                            BinarySwitch b(r);
                            b.isOnAsync([b](const OC::HeaderOptions & headerOptions, const OC::OCRepresentation & rep,
                                            const int eCode)
                            {
                                if (eCode == OC_STACK_OK)
                                {
                                    bool powerState;
                                    std::string uri;
                                    rep.getValue("uri", uri);
                                    if (uri == b.getResource()->uri())
                                    {
                                        rep.getValue("value", powerState);
                                    }
                                    //std::cout << uri << std::endl;
                                    //std::cout << "\t" << ((powerState) ? "is ON" : "is OFF") << std::endl;
                                    b.turnOnAsync(!powerState, [](const OC::HeaderOptions & headerOptions,
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
        else if (cmd[0] == "clear")
        {
            m_binarySwitchSet.clear();
            m_binarySwitchVector.clear();
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

bool MenuBinarySwitch::quit()
{
    return m_quit;
}

void MenuBinarySwitch::onFindResource(std::shared_ptr< OC::OCResource > resource)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "Found resource" << std::endl;

    try
    {
        if (resource)
        {
            for (auto &resourceType : resource->getResourceTypes())
            {
                if (resourceType == UPNP_OIC_TYPE_POWER_SWITCH)
                {
                    if (m_binarySwitchSet.find(resource) == m_binarySwitchSet.end())
                    {
                        m_binarySwitchSet.insert(resource);
                        m_binarySwitchVector.push_back(resource);
                        if (m_binarySwitchSet.size() != m_binarySwitchVector.size())
                        {
                            std::cerr << "Mismatch in discovered devices. Reinitilizing.";
                            init(m_binarySwitchSet);
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
