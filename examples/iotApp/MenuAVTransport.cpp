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

#include "MenuAVTransport.h"
#include "AVTransport.h"

MenuAVTransport::MenuAVTransport() :
    m_quit(false), m_mutex()
{
}

MenuAVTransport::~MenuAVTransport()
{
}

void MenuAVTransport::init(std::set<OC::OCResource::Ptr, OCResourceComp> avTransportSet)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_avTransportSet.clear();
    m_avTransportVector.clear();

    m_avTransportSet = avTransportSet;
    for (auto r : m_avTransportSet)
    {
        m_avTransportVector.push_back(r);
    }
    print();
}

std::string MenuAVTransport::getName()
{
    return "AV Transport";
}

void MenuAVTransport::print()
{
    std::cout << "------------------------------------" << std::endl;
    std::cout << getName() << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "find) Discover any AV transport service(s) " << std::endl;
    std::cout << "list) list currently discovered AV transport services (" << m_avTransportVector.size()
              << " found)" << std::endl;
    std::cout <<
              "setavturi) setavturi <#> <instanceId> <currentURI> <currentURIMetaData> - set the  transport URI"
              << std::endl;
    std::cout <<
              "setnextavturi) setnextavturi <#> <instanceId> <nextURI> <nextURIMetaData> - get the sort capabilities"
              << std::endl;
    std::cout << "getmi) getmi <#> <instanceId> - get media info" << std::endl;
    std::cout << "getti) getti <#> <instanceId> - get transport info" << std::endl;
    std::cout << "getpi) getpi <#> <instanceId> - get position info" << std::endl;
    std::cout << "getdc) getdc <#> <instanceId> - get device capabilities" << std::endl;
    std::cout << "getts) getts <#> <instanceId> - get transport settings" << std::endl;
    std::cout << "stop) stop <#> <instanceId> - stop" << std::endl;
    std::cout << "play) play <#> <instanceId> [playspeed]- play" << std::endl;
    std::cout << "pause) pause <#> <instanceId> - pause" << std::endl;
    std::cout << "seek) seek <#> <instanceId> <unit> <target> - seek" << std::endl;
    std::cout << "next) next <#> <instanceId> - next" << std::endl;
    std::cout << "pre) pre <#> <instanceId> - previous" << std::endl;
    std::cout << "setpm) setpm <#> <instanceId> <newPlayMode> - set play mode" << std::endl;
    std::cout << "getcta) getcta <#> <instanceId> - get current transport actions" << std::endl;
    std::cout << "clear) clear all discovered content directory service(s)" << std::endl;
    std::cout << "b) Back" << std::endl;
    std::cout << "h) Help" << std::endl;
    std::cout << "q) Quit" << std::endl;
}

void MenuAVTransport::help()
{
    print();
}

void MenuAVTransport::run(const std::vector<std::string> &cmd,
                          std::stack<std::unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if ("find" == cmd[0])
        {
            onFindResourceCb = std::bind(&MenuAVTransport::onFindResource, this, std::placeholders::_1);
            OC::OCPlatform::findResource("",
                                         std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_AV_TRANSPORT, CT_DEFAULT,
                                         onFindResourceCb);
        }
        else if (cmd[0] == "list")
        {
            for (size_t i = 0; i < m_avTransportVector.size(); ++i)
            {
                std::cout << "[" << i << "]" << std::endl;
                printResourceCompact(m_avTransportVector[i]);
            }
        }
        else if ("setavturi" == cmd[0])
        {
            if (cmd.size() > 4)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    std::cout << "Setting AV transport URI" <<  std::endl;
                    //[2] instanceId, [3]currentURI, [4]currentURIMetaData
                    avt.setAVTransportURI(instanceid, cmd[3], cmd[4]);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "setavturi command missing a required parameter." << std::endl;
            }
        }
        else if ("setnextavuri" == cmd[0])
        {
            if (cmd.size() > 4)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    std::cout << "Setting AV transport URI" <<  std::endl;
                    //[2] instanceId, [3]nextURI, [4]URIMetaData
                    avt.setNextAVTransportURI(instanceid, cmd[3], cmd[4]);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "setnextavuri command missing a required parameter." << std::endl;
            }
        }
        else if ("getmi" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    AVTransport::MediaInfo mediaInfo = avt.getMediaInfo(instanceid);
                    std::cout << "Media Info:" << std::endl;
                    std::cout << "\tAV Transport URI: " << mediaInfo.currentUri << std::endl;
                    std::cout << "\tAV Transport URI Metadata: " << mediaInfo.currentUriMetadata << std::endl;
                    std::cout << "\tNext AV Transport URI: " << mediaInfo.nextUri << std::endl;
                    std::cout << "\tNext AV Transport URI Metadata: " << mediaInfo.nextUriMetadata << std::endl;
                    std::cout << "\tCurrent Media Duration: " << mediaInfo.mediaDuration << std::endl;
                    std::cout << "\tPlayback Storage Medium: " << mediaInfo.playMedium << std::endl;
                    std::cout << "\tRecord Storage Medium: " << mediaInfo.recordMedium << std::endl;
                    std::cout << "\tRecore Mediumm Write Status: " << mediaInfo.writeStatus << std::endl;
                    std::cout << "\tNumber of Tracks: " << mediaInfo.nrTracks << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "getmi command requires an integer." << std::endl;
            }
        }
        else if ("getti" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    AVTransport::TransportInfo transportInfo = avt.getTransportInfo(instanceid);
                    std::cout << "Transport Info:" << std::endl;
                    std::cout << "\tTransport Play Speed: " << transportInfo.speed << std::endl;
                    std::cout << "\tTransport State: " << transportInfo.transportState << std::endl;
                    std::cout << "\tTransport Status: " << transportInfo.transportStatus << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "getti command requires an integer." << std::endl;
            }
        }
        else if ("getpi" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    AVTransport::PositionInfo positionInfo = avt.getPositionInfo(instanceid);
                    std::cout << "Position Info:" << std::endl;
                    std::cout << "\tAbsolute Counter Position: " << positionInfo.absCount << std::endl;
                    std::cout << "\tAbsolute Time Position: " << positionInfo.absTime << std::endl;
                    std::cout << "\tRelative Counter Position: " << positionInfo.relCount << std::endl;
                    std::cout << "\tRelative Time Position: " << positionInfo.relTime << std::endl;
                    std::cout << "\tCurrent Track: " << positionInfo.track << std::endl;
                    std::cout << "\tCurrent Track Duration: " << positionInfo.trackDuration << std::endl;
                    std::cout << "\tCurrent Track Metadata: " << positionInfo.trackMetadata << std::endl;
                    std::cout << "\tCurrent Track URI: " << positionInfo.trackUri << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "getpi command requires an integer." << std::endl;
            }
        }
        else if ("getdc" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    AVTransport::DeviceCapabilities deviceCap = avt.getDeviceCapabilities(instanceid);
                    std::cout << "Device Capabilities:" << std::endl;
                    std::cout << "\tPossible Playback Storage Media: " << deviceCap.playMedia << std::endl;
                    std::cout << "\tPossible Record Storage Media: " << deviceCap.recMedia << std::endl;
                    std::cout << "\tPossible Record Quality Modes: " << deviceCap.recQualityModes << std::endl;

                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "getdc command requires an integer." << std::endl;
            }
        }
        else if ("getts" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    AVTransport::TransportSettings transportSettings = avt.getTransportSettings(instanceid);
                    std::cout << "Transport Settings:" << std::endl;
                    std::cout << "\tCurrent Play Mode: " << transportSettings.playMode << std::endl;
                    std::cout << "\tCurrent Recore Quality Mode: " << transportSettings.recQualityMode << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "getts command requires an integer." << std::endl;
            }
        }
        else if ("stop" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    avt.stop(instanceid);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "stop command requires an integer." << std::endl;
            }
        }
        else if ("play" == cmd[0])
        {
            if (cmd.size() > 3)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    // [2] insanceid, [3] playspeed
                    std::cout << "Play, speed " << cmd[3] <<  std::endl;
                    avt.play(instanceid, cmd[3]);
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
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    std::cout << "Play, speed 1.0" << std::endl;
                    avt.play(instanceid);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "play command requires an integer." << std::endl;
            }
        }
        else if ("pause" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    avt.pause(instanceid);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "pause command requires an integer." << std::endl;
            }
        }
        else if ("seek" == cmd[0])
        {
            if (cmd.size() > 4)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    // [2]instanceId, [3]unit, [4]target
                    avt.seek(instanceid, cmd[3], cmd[4]);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "seek command requires an integer, and instanceId, seekUnit, and seekTarget." <<
                          std::endl;
            }
        }
        else if ("next" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    avt.next(instanceid);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "next command requires an integer." << std::endl;
            }
        }
        else if ("pre" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    avt.previous(instanceid);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "pre command requires an integer." << std::endl;
            }
        }
        else if ("setpm" == cmd[0])
        {
            if (cmd.size() > 3)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    // [2] insanceid, [3] newPlayMode
                    avt.setPlayMode(instanceid, cmd[3]);
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "setpm command requires an integer." << std::endl;
            }
        }
        else if ("getcta" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_avTransportVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    unsigned int instanceid = std::stoi(cmd[2]);
                    AVTransport avt(m_avTransportVector[i]);
                    std::cout << m_avTransportVector[i]->uri() << std::endl;
                    std::cout << "Current Transport Actions: " << avt.getCurrentTransportActions(
                                  instanceid) << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "unable to convert string to int." << std::endl;
                }
            }
            else
            {
                std::cout << "getcta command requires an integer." << std::endl;
            }
        }
        else if ("clear" == cmd[0])
        {
            m_avTransportSet.clear();
            m_avTransportVector.clear();
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

bool MenuAVTransport::quit()
{
    return m_quit;
}

void MenuAVTransport::onFindResource(std::shared_ptr< OC::OCResource > resource)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "Found resource" << std::endl;

    try
    {
        if (resource)
        {
            for (auto &resourceType : resource->getResourceTypes())
            {
                if (resourceType == UPNP_OIC_TYPE_AV_TRANSPORT)
                {
                    if (m_avTransportSet.find(resource) == m_avTransportSet.end())
                    {
                        m_avTransportSet.insert(resource);
                        m_avTransportVector.push_back(resource);
                        if (m_avTransportSet.size() != m_avTransportVector.size())
                        {
                            std::cerr << "Mismatch in discovered Services. Reinitializing.";
                            init(m_avTransportSet);
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
