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

#include "Layer3Forwarding.h"
#include "MenuLayer3Forwarding.h"

#include <UpnpConstants.h>

using namespace std;
using namespace OC;

MenuLayer3Forwarding::MenuLayer3Forwarding() :
    m_quit(false), m_mutex()
{
    onFindResourceCb = bind(&MenuLayer3Forwarding::onFindResource, this, placeholders::_1);
}

MenuLayer3Forwarding::~MenuLayer3Forwarding() {}

void MenuLayer3Forwarding::init(set<shared_ptr<OCResource>, OCResourceComp> l3fSet)
{
    lock_guard<mutex> lock(m_mutex);
    m_l3fSet.clear();
    m_l3fVector.clear();

    m_l3fSet = l3fSet;
    for (auto r : m_l3fSet)
    {
        m_l3fVector.push_back(r);
    }
    print();
}

string MenuLayer3Forwarding::getName()
{
    return "Layer 3 Forwarding";
}

void MenuLayer3Forwarding::print()
{
    cout << "------------------------------------" << endl;
    cout << getName() << ": " << m_l3fVector.size() << " found" << endl;
    cout << "------------------------------------" << endl;
    cout << "find) Discover any layer 3 forwarding services " << endl;
    cout << "list) list currently discovered layer 3 forwarding services" << endl;
    cout << "clear) clear all discovered layer 3 forwarding service(s)" << endl;
    cout << "getdcs) getdcs <#> - get the default connection service" << endl;
    cout << "setdcs) setdcs <#> <newDefaultConectionService> - set the default connection service" << endl;
    cout << "b) Back" << endl;
    cout << "h) Help" << endl;
    cout << "q) Quit" << endl;
}

void MenuLayer3Forwarding::help()
{
    print();
}
void MenuLayer3Forwarding::run(const vector<string> &cmd,
                         stack<unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if (cmd[0] == "find")
        {
            OCStackResult result = OCPlatform::findResource("",
                                   string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_LAYER3_FORWARDING, CT_DEFAULT,
                                   onFindResourceCb);
            cout << "findResource(" + string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" +
                    UPNP_OIC_TYPE_LAYER3_FORWARDING + ") - " << result << endl;
        }
        else if (cmd[0] == "list")
        {
            for (size_t i = 0; i < m_l3fVector.size(); ++i)
            {
                cout << "[" << i << "]" << endl;
                printResourceCompact(m_l3fVector[i]);
            }
        }
        else if (cmd[0] == "clear")
        {
            m_l3fSet.clear();
            m_l3fVector.clear();
        }
        else if (cmd[0] == "getdcs")
        {
            if (cmd.size() > 1)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_l3fVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    Layer3Forwarding l3f(m_l3fVector[i]);
                    string currentDefaultConnectionService = l3f.getDefaultConnectionService();
                    cout << l3f.getResource() << endl;
                    cout << "Default connection service: " << currentDefaultConnectionService << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setdcs command requires an integer indicating the layer 3 resource." << endl;
            }
        }
        else if (cmd[0] == "setdcs")
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_l3fVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    Layer3Forwarding l3f(m_l3fVector[i]);
                    l3f.setDefaultConnectionService(cmd[2]);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setdcs command requires an integer indicating the layer 3 resource." << endl;
            }
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
            cout << "unknown command." << endl;
            print();
        }
    }
}

bool MenuLayer3Forwarding::quit()
{
    return m_quit;
}

void MenuLayer3Forwarding::onFindResource(shared_ptr< OCResource > resource)
{
    lock_guard<mutex> lock(m_mutex);
    cout << "Found resource" << endl;

    try
    {
        if (resource)
        {
            for (auto &resourceType : resource->getResourceTypes())
            {
                if (resourceType == UPNP_OIC_TYPE_LAYER3_FORWARDING)
                {
                    if (m_l3fSet.find(resource) == m_l3fSet.end())
                    {
                        m_l3fSet.insert(resource);
                        m_l3fVector.push_back(resource);
                        if (m_l3fSet.size() != m_l3fVector.size())
                        {
                            cerr << "Mismatch in discovered Services. Reinitializing.";
                            init(m_l3fSet);
                        }
                    }
                }
            }
        }
    }
    catch (exception &e)
    {
        cerr << "Caught exception in foundResource: " << e.what() << endl;
    }
}
