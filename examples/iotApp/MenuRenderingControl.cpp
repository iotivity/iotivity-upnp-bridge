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

#include "RenderingControl.h"
#include "MenuRenderingControl.h"

MenuRenderingControl::MenuRenderingControl() :
    m_quit(false), m_mutex()
{
}

MenuRenderingControl::~MenuRenderingControl()
{
}

void MenuRenderingControl::init(std::set<OC::OCResource::Ptr, OCResourceComp> brightnessSet)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_renderingControlSet.clear();
    m_renderingControlVector.clear();

    m_renderingControlSet = brightnessSet;
    for (auto r : m_renderingControlSet)
    {
        m_renderingControlVector.push_back(r);
    }
    print();
}

std::string MenuRenderingControl::getName()
{
    return "Rendering Control";
}

void MenuRenderingControl::print()
{
    std::cout << "------------------------------------" << std::endl;
    std::cout << getName() << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "find) Discover any rendering control service(s) " << std::endl;
    std::cout << "list) list currently discovered content directory services (" << m_renderingControlVector.size() << " found)" << std::endl;
    //TODO add last change
    std::cout << "listpresets) listpresets <#> <instanceId> - obtain the preset name list" << std::endl;
    std::cout << "selectpreset) selectpreset <#> <instanceId> <presetName> - select preset" << std::endl;
    std::cout << "getmute) getmute <#> <instanceId> [channel] - get mute (Master channel is used if channel is unspecified)" << std::endl;
    std::cout << "setmute) setmute <#> <instanceId> [channel] <desiredMute> - set mute (Master channel is used if channel is unspecified)" << std::endl;
    std::cout << "getvol) getvol <#> <instanceId> [channel] - get volume (Master channel is used if channel is unspecified)" << std::endl;
    std::cout << "setvol) setvol <#> <instanceId> [channel] <desiredMute> - set volume (Master channel is used if channel is unspecified)" << std::endl;
    std::cout << "clear) clear all discovered content directory service(s)" << std::endl;
    std::cout << "b) Back" << std::endl;
    std::cout << "h) Help" << std::endl;
    std::cout << "q) Quit" << std::endl;
}

void MenuRenderingControl::help()
{
    print();
}
void MenuRenderingControl::run(const std::vector<std::string> &cmd,
                               std::stack<std::unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if ("find" == cmd[0])
        {
            onFindResourceCb = std::bind(&MenuRenderingControl::onFindResource, this, std::placeholders::_1);
            OC::OCPlatform::findResource("",
                                         std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_RENDERING_CONTROL, CT_DEFAULT,
                                         onFindResourceCb);
        }
        else if (cmd[0] == "list")
        {
            for (size_t i = 0; i < m_renderingControlVector.size(); ++i)
            {
                std::cout << "[" << i << "]" << std::endl;
                printResourceCompact(m_renderingControlVector[i]);
            }
        }
        else if ("listpresets" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    std::cout << m_renderingControlVector[i]->uri() << std::endl;
                    std::cout << "Present Name List: " << rc.listPresets(instanceid) << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "listpresets command requires an integer or key word all to work." << std::endl;
            }
        }
        else if ("selectpreset" == cmd[0])
        {
            if (cmd.size() > 3)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    std::cout << m_renderingControlVector[i]->uri() << std::endl;
                    // [2]instanceId, [3]presetName
                    rc.selectPreset(instanceid, cmd[3]);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "listpresets command missing required parameter." << std::endl;
            }
        }
        else if ("getmute" == cmd[0])
        {
            if (cmd.size() > 3)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    std::cout << m_renderingControlVector[i]->uri() << std::endl;
                    // [2]instanceId, [3]channel
                    std::cout << "Current Mute: " << rc.getMute(instanceid, cmd[3]) << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else if (cmd.size() > 2)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    std::cout << m_renderingControlVector[i]->uri() << std::endl;
                    // [2]instanceId, channel
                    std::cout << "Current Mute: " << rc.getMute(instanceid, std::string("Master")) << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "getmute command missing required parameter." << std::endl;
            }
        }
        else if ("setmute" == cmd[0])
        {
            if (cmd.size() > 4)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    std::cout << m_renderingControlVector[i]->uri() << std::endl;
                    // [2]instanceId, [3]channel, [4]desiredMute
                    bool desiredMute = ((cmd[4] == "true") ? true : false);
                    rc.setMute(instanceid, cmd[3], desiredMute);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else if (cmd.size() > 3)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    std::cout << m_renderingControlVector[i]->uri() << std::endl;
                    // [2]instanceId, channel, [3]desiredMute
                    bool desiredMute = ((cmd[3] == "true") ? true : false);
                    rc.setMute(instanceid, std::string("Master"), desiredMute);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "setmute command missing required parameter." << std::endl;
            }
        }
        else if ("getvol" == cmd[0])
        {
            if (cmd.size() > 3)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    std::cout << m_renderingControlVector[i]->uri() << std::endl;
                    // [2]instanceId, [3]channel
                    std::cout << "Current Volume: " << rc.getVolume(instanceid, cmd[3]) << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else if (cmd.size() > 2)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    std::cout << m_renderingControlVector[i]->uri() << std::endl;
                    // [2]instanceId, channel
                    std::cout << "Current Volume: " << rc.getVolume(instanceid, std::string("Master")) << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "getvol command missing required parameter." << std::endl;
            }
        }
        else if ("setvol" == cmd[0])
        {
            if (cmd.size() > 4)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    int desiredVolume = std::stoi(cmd[4]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    std::cout << m_renderingControlVector[i]->uri() << std::endl;
                    // [2]instanceId, [3]channel, [4]desiredVoluem
                    rc.setVolume(instanceid, cmd[3], desiredVolume);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            if (cmd.size() > 3)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    int desiredVolume = std::stoi(cmd[3]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    std::cout << m_renderingControlVector[i]->uri() << std::endl;
                    // [2]instanceId, channel, [3]desiredVoluem
                    rc.setVolume(instanceid, std::string("Master"), desiredVolume);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "setvol command missing required parameter." << std::endl;
            }
        }
        else if ("clear" == cmd[0])
        {
            m_renderingControlSet.clear();
            m_renderingControlVector.clear();
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

bool MenuRenderingControl::quit()
{
    return m_quit;
}

void MenuRenderingControl::onFindResource(std::shared_ptr< OC::OCResource > resource)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "Found resource" << std::endl;

    try
    {
        if (resource)
        {
            for (auto &resourceType : resource->getResourceTypes())
            {
                if (resourceType == UPNP_OIC_TYPE_RENDERING_CONTROL)
                {
                    if (m_renderingControlSet.find(resource) == m_renderingControlSet.end())
                    {
                        m_renderingControlSet.insert(resource);
                        m_renderingControlVector.push_back(resource);
                        if (m_renderingControlSet.size() != m_renderingControlVector.size())
                        {
                            std::cerr << "Mismatch in discovered Services. Reinitializing.";
                            init(m_renderingControlSet);
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
