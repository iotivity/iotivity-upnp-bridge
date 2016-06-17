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

#include "ContentDirectory.h"
#include "MenuContentDirectory.h"

MenuContentDirectory::MenuContentDirectory() :
    m_quit(false), m_mutex()
{
}

MenuContentDirectory::~MenuContentDirectory()
{
}

#include "ContentDirectory.h"
#include "MenuConnectionManager.h"

void MenuContentDirectory::init(std::set<OC::OCResource::Ptr, OCResourceComp> brightnessSet)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_contentDirectorySet.clear();
    m_contentDirectoryVector.clear();

    m_contentDirectorySet = brightnessSet;
    for (auto r : m_contentDirectorySet)
    {
        m_contentDirectoryVector.push_back(r);
    }
    print();
}

std::string MenuContentDirectory::getName()
{
    return "Content Directory";
}

void MenuContentDirectory::print()
{
    std::cout << "------------------------------------" << std::endl;
    std::cout << getName() << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "find) Discover any content directory service(s) " << std::endl;
    std::cout << "list) list currently discovered content directory services (" <<
              m_contentDirectoryVector.size() <<
              " found)" << std::endl;
    std::cout << "getsearchcap) getsearchcap <#> or getsearchcap all - get the search capabilities" <<
              std::endl;
    std::cout << "getsortcap) getsortcaps <#> or getsortcap all - get the sort capabilities" <<
              std::endl;
    std::cout << "getsuid) getsuid <#> or getsuid all - get the system update ID" << std::endl;
    std::cout << "getsrt) getsrt <#> or getsuid all - get the service reset token" << std::endl;
    std::cout <<
              "browse) browse <#> <objectId> <browseFlag> <filter> <startIndex> <requestedCount> <sortCriteria>"
              << std::endl;
    std::cout <<
              "search) search <#> <containerId> <searchCriteria> <filter> <startingIndex> <requestCount> <sortCriteria>"
              << std::endl;
    std::cout << "clear) clear all discovered content directory service(s)" << std::endl;
    std::cout << "b) Back" << std::endl;
    std::cout << "h) Help" << std::endl;
    std::cout << "q) Quit" << std::endl;
}

void MenuContentDirectory::help()
{
    print();
}
void MenuContentDirectory::run(const std::vector<std::string> &cmd,
                               std::stack<std::unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if ("find" == cmd[0])
        {
            onFindResourceCb = std::bind(&MenuContentDirectory::onFindResource, this, std::placeholders::_1);
            OCStackResult result = OC::OCPlatform::findResource("",
                                   std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_CONTENT_DIRECTORY, CT_DEFAULT,
                                   onFindResourceCb);
            std::cout << "findResource(" + std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" +
                      UPNP_OIC_TYPE_CONTENT_DIRECTORY + ") - " << result << std::endl;
        }
        else if (cmd[0] == "list")
        {
            for (size_t i = 0; i < m_contentDirectoryVector.size(); ++i)
            {
                std::cout << "[" << i << "]" << std::endl;
                printResourceCompact(m_contentDirectoryVector[i]);
            }
        }
        else if ("getsearchcap" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_contentDirectoryVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    ContentDirectory cd(m_contentDirectoryVector[i]);
                    std::cout << m_contentDirectoryVector[i]->uri() << std::endl;
                    std::cout << "Search Capabilities: " << cd.getSearchCapabilities() << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto c : m_contentDirectoryVector)
                        {
                            ContentDirectory cd(c);
                            std::cout << c->uri() << std::endl;
                            std::cout << "Search Capabilities: " << cd.getSearchCapabilities() << std::endl;
                        }
                    }
                }
            }
            else
            {
                std::cout << "getsearchcap command requires an integer or key word all to work." << std::endl;
            }
        }
        else if ("getsortcap" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_contentDirectoryVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    ContentDirectory cd(m_contentDirectoryVector[i]);
                    std::cout << m_contentDirectoryVector[i]->uri() << std::endl;
                    std::cout << "Sort Capabilities: " << cd.getSortCapabilites() << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto c : m_contentDirectoryVector)
                        {
                            ContentDirectory cd(c);
                            std::cout << c->uri() << std::endl;
                            std::cout << "Sort Capabilities: " << cd.getSortCapabilites() << std::endl;
                        }
                    }
                }
            }
            else
            {
                std::cout << "getsortcap command requires an integer or key word all to work." << std::endl;
            }
        }
        else if ("getsuid" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_contentDirectoryVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    ContentDirectory cd(m_contentDirectoryVector[i]);
                    std::cout << m_contentDirectoryVector[i]->uri() << std::endl;
                    std::cout << "System Update ID: " << cd.getSystemUpdateId() << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto c : m_contentDirectoryVector)
                        {
                            ContentDirectory cd(c);
                            std::cout << c->uri() << std::endl;
                            std::cout << "System Update ID: " << cd.getSystemUpdateId() << std::endl;
                        }
                    }
                }
            }
            else
            {
                std::cout << "getsuid command requires an integer or key word all to work." << std::endl;
            }
        }
        else if ("getsrt" == cmd[0])
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_contentDirectoryVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    ContentDirectory cd(m_contentDirectoryVector[i]);
                    std::cout << m_contentDirectoryVector[i]->uri() << std::endl;
                    std::cout << "Service Reset Token: " << cd.getServiceResetToken() << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    if ("all" == cmd[1])
                    {
                        for (auto c : m_contentDirectoryVector)
                        {
                            ContentDirectory cd(c);
                            std::cout << c->uri() << std::endl;
                            std::cout << "Service Reset Token: " << cd.getServiceResetToken() << std::endl;
                        }
                    }
                }
            }
            else
            {
                std::cout << "getsrt command requires an integer or key word all to work." << std::endl;
            }
        }
        else if ("browse" == cmd[0])
        {
#if 0
            if (cmd.size() > 7)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_contentDirectoryVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    ContentDirectory cd(m_contentDirectoryVector[i]);
                    int startingIndex = std::stoi(cmd[5]);
                    int requestedCount = std::stoi(cmd[6]);
                    // [2]objectId, [3]browseFlag, [4]filter, [5]startingIndex, [6]requestedCount, [7]sourtCriteria
                    ContentDirectory::SearchResult br = cd.browse(cmd[2], cmd[3], cmd[4], startingIndex, requestedCount,
                                                        cmd[7]);
                    std::cout << m_contentDirectoryVector[i]->uri() << std::endl;
                    std::cout << "Browse Results:" << std::endl;
                    std::cout << "\tresults: " << br.result << std::endl;
                    std::cout << "\tnumber returned: " << br.numberReturned << std::endl;
                    std::cout << "\ttotal matches: " << br.totalMatches << std::endl;
                    std::cout << "\tupdate ID: " << br.updateId << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "invalid argument entered into the browse command" << std::endl;
                }
            }
            else
            {
                std::cout << "browse command missing required argument." << std::endl;
            }
#else
            std::cout << "browse command not yet implemented." << std::endl;
#endif
        }
        else if ("search" == cmd[0])
        {
#if 0
            if (cmd.size() > 7)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (i >= m_contentDirectoryVector.size())
                    {
                        std::cout << "Invalid resource specified." << std::endl;
                        return;
                    }
                    ContentDirectory cd(m_contentDirectoryVector[i]);
                    int startingIndex = std::stoi(cmd[5]);
                    int requestedCount = std::stoi(cmd[6]);
                    // [2]containerId, [3]searchCriteria, [4]filter, [5]startingIndex, [6]requestedCount, [7]sourtCriteria
                    ContentDirectory::SearchResult br = cd.browse(cmd[2], cmd[3], cmd[4], startingIndex, requestedCount,
                                                        cmd[7]);
                    std::cout << m_contentDirectoryVector[i]->uri() << std::endl;
                    std::cout << "Search Results:" << std::endl;
                    std::cout << "\tresults: " << br.result << std::endl;
                    std::cout << "\tnumber returned: " << br.numberReturned << std::endl;
                    std::cout << "\ttotal matches: " << br.totalMatches << std::endl;
                    std::cout << "\tupdate ID: " << br.updateId << std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "invalid argument entered into the browse command" << std::endl;
                }
            }
            else
            {
                std::cout << "search command missing required argument." << std::endl;
            }
#else
            std::cout << "search command not yet implemented." << std::endl;
#endif
        }
        else if ("clear" == cmd[0])
        {
            m_contentDirectorySet.clear();
            m_contentDirectoryVector.clear();
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

bool MenuContentDirectory::quit()
{
    return m_quit;
}

void MenuContentDirectory::onFindResource(std::shared_ptr< OC::OCResource > resource)
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
                if (resourceType == UPNP_OIC_TYPE_CONTENT_DIRECTORY)
                {
                    if (m_contentDirectorySet.find(resource) == m_contentDirectorySet.end())
                    {
                        m_contentDirectorySet.insert(resource);
                        m_contentDirectoryVector.push_back(resource);
                        if (m_contentDirectorySet.size() != m_contentDirectoryVector.size())
                        {
                            std::cerr << "Mismatch in discovered Services. Reinitializing.";
                            init(m_contentDirectorySet);
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
