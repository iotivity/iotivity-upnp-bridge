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
#include <UpnpBridgeAttributes.h>

#include "MenuIotivity.h"
#include "IotivityUtility.h"

MenuIotivity::MenuIotivity():
    m_resources(), m_quit(false)
{
    onFindResourceCb = std::bind(&MenuIotivity::onFindResource, this, std::placeholders::_1);
    onResponceCb = std::bind(&MenuIotivity::onResponse, this, std::placeholders::_1,
                             std::placeholders::_2, std::placeholders::_3);
    onObserveCb = std::bind(&MenuIotivity::onObserve, this, std::placeholders::_1,
                            std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    print();
}

MenuIotivity::~MenuIotivity() {}

std::string MenuIotivity::getName()
{
    return "Iotivity";
}

void MenuIotivity::print()
{
    std::cout << "------------------------------------" << std::endl;
    std::cout << getName() << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "find) find <requestURI>" << std::endl;
    std::cout << "list) list all found resources" << std::endl;
    std::cout << "get) get <#>" << std::endl;
    std::cout << "post) post <#> <name> <type> <value>" << std::endl;
    std::cout << "observe) observe <#>" << std::endl;
    std::cout << "clear) clear all found resources" << std::endl;
    std::cout << "b) Back" << std::endl;
    std::cout << "h) Help" << std::endl;
    std::cout << "q) Quit" << std::endl;
}

void MenuIotivity::help()
{
    print();
}
void MenuIotivity::run(const std::vector<std::string> &cmd,
                       std::stack<std::unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if (cmd[0] == "find")
        {
            std::vector<std::string> providedFindStrings =
            {
                std::string(OC_RSRVD_WELL_KNOWN_URI),
                // servics
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_BRIGHTNESS,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_POWER_SWITCH,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_CONNECTION_MANAGER,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_AV_TRANSPORT,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_CONTENT_DIRECTORY,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_RENDERING_CONTROL,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_SCHEDULED_RECORDING,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_WAN_IF_CONFIG,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_LAYER3_FORWARDING,
                // devices
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_LIGHT,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_INET_GATEWAY,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_WAN,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_WAN_CONNECTION,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_LAN,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_MEDIA_RENDERER,
                std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_DEVICE_MEDIA_SERVER
            };
            if (cmd.size() > 1)
            {
                std::string requestURI = "";
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    requestURI = providedFindStrings[i];
                }
                catch (const std::invalid_argument &ia)
                {
                    requestURI = cmd[1];
                }
                OC::OCPlatform::findResource("", requestURI, CT_DEFAULT, onFindResourceCb);
            }
            else
            {
                std::cout << "find command requires a request URI string or an integer to use the\n"
                          "pre-provided request URI.\n"
                          "Provided request URIs are:\n"
                          "--------------------------" << std::endl;
                for (size_t k = 0; k < providedFindStrings.size() ; ++k)
                {
                    std::cout << "\t[" << k << "] " << providedFindStrings[k] << std::endl;
                }
            }
        }
        else if (cmd[0] == "list")
        {
            for (size_t i = 0; i < m_resources.size(); ++i)
            {
                std::cout << "[" << i << "]" << std::endl;
                printResourceCompact(m_resources[i]);
            }
        }
        else if (cmd[0] == "get")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = std::stoi(cmd[1]);
                    if (!(i < m_resources.size()))
                    {
                        throw std::invalid_argument("value does not specify a resource");
                    }
                    OC::QueryParamsMap queryParams;
                    OCStackResult result = m_resources[i]->get(queryParams, onResponceCb);
                    std::cout << "get " << m_resources[i]->host() << m_resources[i]->uri() << " - " << result <<
                              std::endl;
                }
                catch (const std::invalid_argument &ia)
                {
                    std::cout << "Error either no number was given or the number does not represent a\n"
                              "found resource. Try the 'list' or 'find' command." << std::endl;
                }
            }
            else
            {
                std::cout << "The get command requires a second number parameter the number must\n"
                          "represent a found resource. trye 'list' or 'find' command." << std::endl;
            }
        }
        else if (cmd[0] == "put")
        {
            // currently an undocumented command since code not fully working
            try
            {
                size_t i = std::stoi(cmd[1]);
                if (!(i < m_resources.size()))
                {
                    throw std::invalid_argument("value does not specify a resource");
                }
                OC::OCRepresentation rep;
                // TODO parse rep, as below
                OC::QueryParamsMap queryParams;
                OCStackResult result = m_resources[i]->put(rep, queryParams, onResponceCb);
                std::cout << "put " << m_resources[i]->host() << m_resources[i]->uri() << " - " << result <<
                          std::endl;
            }
            catch (const std::invalid_argument &ia)
            {
                std::cout << "Command requires a second number parameter the number must\n"
                          "represent a found resource. try 'list' or 'find' command." << std::endl;
            }
        }
        else if (cmd[0] == "post")
        {
            try
            {
                size_t i = std::stoi(cmd[1]);
                if (!(i < m_resources.size()))
                {
                    throw std::invalid_argument("value does not specify a resource");
                }
                OC::OCRepresentation rep;
                size_t k = 2;
                while (k < cmd.size())
                {
                    if ((k + 3) > cmd.size())
                    {
                        std::cout << "The 'post' command requires three values for each variable.\n"
                                  "\t1. The name of the IoTivity attribute you wish to change.\n"
                                  "\t2. The attribute data type. (int, double, bool, or string)\n"
                                  "\t3. The value you want to change the attribute to.\n"
                                  "example: 'post 1 value bool true'" << std::endl;
                        break;
                    }
                    std::string name = cmd[k++];
                    std::string type = cmd[k++];
                    if (type == "int")
                    {
                        try
                        {
                            int value = std::stoi(cmd[k++]);
                            rep.setValue(name, value);
                        }
                        catch (const std::invalid_argument &ia)
                        {
                            std::cout << "int value specified but not provided." << std::endl;
                            break;
                        }
                    }
                    else if (type == "double")
                    {
                        try
                        {
                            double value = std::stod(cmd[k++]);
                            rep.setValue(name, value);
                        }
                        catch (const std::invalid_argument &ia)
                        {
                            std::cout << "double value specified but not provided." << std::endl;
                            break;
                        }
                    }
                    else if (type == "bool")
                    {
                        bool value = (cmd[k++] == "false") ? false : true;
                        rep.setValue(name, value);
                    }
                    else if (type == "string")
                    {
                        std::string value = cmd[k++];
                        rep.setValue(name, value);
                    }
                    else
                    {
                        std::cout << "Unsupported parameter type type must be 'int', 'double', 'bool' or 'string'." <<
                                  std::endl;
                    }
                }
                OC::QueryParamsMap queryParams;
                OCStackResult result = m_resources[i]->post(rep, queryParams, onResponceCb);
                std::cout << "post " << m_resources[i]->host() << m_resources[i]->uri() << " - " << result <<
                          std::endl;
            }
            catch (const std::invalid_argument &ia)
            {
                std::cout << "The post command requires a second number parameter the number must\n"
                          "represent a found resource. try 'list' or 'find' command." << std::endl;
            }
        }
        else if (cmd[0] == "observe")
        {
            try
            {
                size_t i = std::stoi(cmd[1]);
                if (!(i < m_resources.size()))
                {
                    throw std::invalid_argument("value does not specify a resource");
                }
                OC::QueryParamsMap queryParams;
                OCStackResult result = m_resources[i]->observe(OC::ObserveType::Observe, queryParams, onObserveCb);
                std::cout << "observe " << m_resources[i]->host() << m_resources[i]->uri() << " - " << result <<
                          std::endl;
            }
            catch (const std::invalid_argument &ia)
            {
                std::cout << "The post command requires a second number parameter the number must\n"
                          "represent a found resource. try 'list' or 'find' command." << std::endl;
            }
            //        } else if (cmd == "delete") {
            //            // TODO
        }
        else if ("clear" == cmd[0])
        {
            m_resources.clear();
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

bool MenuIotivity::quit()
{
    return m_quit;
}

void MenuIotivity::onFindResource(OC::OCResource::Ptr resource)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "Found resource" << std::endl;
    m_resources.push_back(resource);
}

void MenuIotivity::onResponse(const OC::HeaderOptions &, const OC::OCRepresentation &rep,
                              const int eCode)
{
    std::cout << "\teCode " << eCode << std::endl;
    if (eCode == OC_STACK_OK)
    {
        std::cout << "\t" << rep.getHost() << rep.getUri() << std::endl;
        // TODO getPayload()
        // TODO getChildren()
        if (!rep.getResourceTypes().empty())
        {
            std::cout << "\tresourceTypes ";
            for (auto &rt : rep.getResourceTypes())
            {
                std::cout << rt << " ";
            }
            std::cout << std::endl;
        }
        if (!rep.getResourceInterfaces().empty())
        {
            std::cout << "\tresourceInterfaces ";
            for (auto ri : rep.getResourceInterfaces())
            {
                std::cout << ri << " ";
            }
            std::cout << std::endl;
        }
        for (OC::OCRepresentation::const_iterator it = rep.begin(); it != rep.end(); ++it)
        {
            std::cout << "\t" << it->attrname() << " " << it->getValueToString() << std::endl;
            if ("links" == it->attrname())
            {
                OC::AttributeValue links;
                rep.getAttributeValue("links", links);
                auto rep_links = boost::get<std::vector<OC::OCRepresentation> >(links);
                // TODO investigate why this fails compilation
                //std::vector<OC::OCRepresentation> rep_links = it->getValue();
                size_t linkCount = 0;
                for (auto r : rep_links)
                {
                    std::cout << "\t[link " << linkCount++ << "]" << std::endl;
                    if (!r.getResourceTypes().empty())
                    {
                        std::cout << "\t\tresourceTypes ";
                        for (auto rt : r.getResourceTypes())
                        {
                            std::cout << rt << " ";
                        }
                        std::cout << std::endl;
                    }
                    if (!r.getResourceInterfaces().empty())
                    {
                        std::cout << "\t\tresourceInterfaces ";
                        for (auto ri : r.getResourceInterfaces())
                        {
                            std::cout << ri << " ";
                        }
                        std::cout << std::endl;
                    }
                    for (OC::OCRepresentation::const_iterator rit = r.begin(); rit != r.end(); ++rit)
                    {
                        std::cout << "\t\t" << rit->attrname() << " " << rit->getValueToString() << std::endl;
                    }
                }
            }
            // TODO type, base_type, depth
        }
        for (auto foundResource : m_resources)
        {
            if (foundResource->uri() == rep.getUri())
            {
                std::cout << "\n*********************** RESOURCE DETAILS *****************\n";
                for (auto &rt : foundResource->getResourceTypes())
                {
                    std::cout << "\tResource Type:\n\t\t" <<  rt << std::endl;
                    auto it = ResourceAttrMap.find(rt);

                    if (it == ResourceAttrMap.end())
                    {
                        std::cout << "\tRESOURCE MAP NOT FOUND" << std::endl;
                        std::cout << "\n***********************************************************\n";
                        return;
                    }

                    auto &attrMap = it->second;

                    processAttributes(rep, &attrMap, "\t");

                    std::cout << "\n***********************************************************\n";
                }
            }
        }
    }
}

void MenuIotivity::onObserve(const OC::HeaderOptions &, const OC::OCRepresentation &rep,
                             const int eCode, const int sequenceNumber)
{
    std::cout << "observe" << std::endl;
    std::cout << "\teCode=" << eCode << std::endl;
    if (eCode == OC_STACK_OK)
    {
        std::cout << "\tsequencenumber " << sequenceNumber << std::endl
                  << "\t" << rep.getHost() << rep.getUri() << std::endl;
        // TODO getPayload()
        // TODO getChildren()
        if (!rep.getResourceTypes().empty())
        {
            std::cout << "\tresourceTypes ";
            for (auto rt : rep.getResourceTypes())
            {
                std::cout << rt << " ";
            }
            std::cout << std::endl;
        }
        if (!rep.getResourceInterfaces().empty())
        {
            std::cout << "\tresourceInterfaces ";
            for (auto ri : rep.getResourceInterfaces())
            {
                std::cout << ri << " ";
            }
            std::cout << std::endl;
        }
        for (OC::OCRepresentation::const_iterator it = rep.begin(); it != rep.end(); ++it)
        {
            std::cout << "\t" << it->attrname() << " " << it->getValueToString() << std::endl;
            // TODO type, base_type, depth
        }
    }
}
