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

using namespace std;
using namespace OC;

MenuRenderingControl::MenuRenderingControl() :
    m_quit(false), m_mutex()
{
}

MenuRenderingControl::~MenuRenderingControl()
{
}

void MenuRenderingControl::init(set<OCResource::Ptr, OCResourceComp> renderingControlSet)
{
    lock_guard<mutex> lock(m_mutex);
    m_renderingControlSet.clear();
    m_renderingControlVector.clear();

    m_renderingControlSet = renderingControlSet;
    for (auto r : m_renderingControlSet)
    {
        m_renderingControlVector.push_back(r);
    }
    print();
}

string MenuRenderingControl::getName()
{
    return "Rendering Control";
}

void MenuRenderingControl::print()
{
    cout << "------------------------------------" << endl;
    cout << getName() << ": " << m_renderingControlVector.size() << " found)" << endl;
    cout << "------------------------------------" << endl;
    cout << "find) Discover any rendering control services " << endl;
    cout << "list) list currently discovered content directory services" << endl;
    cout << "clear) clear all discovered content directory services" << endl;
    //TODO add last change
    cout << "listpresets) listpresets <#> <instanceId> - obtain the preset name list" << endl;
    cout << "selectpreset) selectpreset <#> <instanceId> <presetName> - select preset" << endl;
    cout <<
         "getmute) getmute <#> <instanceId> [channel] - get mute (Master channel is used if channel is unspecified)"
         << endl;
    cout <<
         "setmute) setmute <#> <instanceId> [channel] <desiredMute> - set mute (Master channel is used if channel is unspecified)"
         << endl;
    cout <<
         "getvol) getvol <#> <instanceId> [channel] - get volume (Master channel is used if channel is unspecified)"
         << endl;
    cout <<
         "setvol) setvol <#> <instanceId> [channel] <desiredVolume> - set volume (Master channel is used if channel is unspecified)"
         << endl;
    cout << "b) Back" << endl;
    cout << "h) Help" << endl;
    cout << "q) Quit" << endl;
}

void MenuRenderingControl::help()
{
    print();
}
void MenuRenderingControl::run(const vector<string> &cmd,
                               stack<unique_ptr<MenuBase>> &menuStack)
{
    if (cmd.size() > 0)
    {
        if ("find" == cmd[0])
        {
            onFindResourceCb = bind(&MenuRenderingControl::onFindResource, this, placeholders::_1);
            OCPlatform::findResource("",
                                     string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_RENDERING_CONTROL, CT_DEFAULT,
                                     onFindResourceCb);
        }
        else if (cmd[0] == "list")
        {
            for (size_t i = 0; i < m_renderingControlVector.size(); ++i)
            {
                cout << "[" << i << "]" << endl;
                printResourceCompact(m_renderingControlVector[i]);
            }
        }
        else if ("listpresets" == cmd[0])
        {
            if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    unsigned int instanceid = stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    cout << m_renderingControlVector[i]->uri() << endl;
                    cout << "Present Name List: " << rc.listPresets(instanceid) << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "listpresets command requires an integer or key word all to work." << endl;
            }
        }
        else if ("selectpreset" == cmd[0])
        {
            if (cmd.size() > 3)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    unsigned int instanceid = stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    cout << m_renderingControlVector[i]->uri() << endl;
                    // [2]instanceId, [3]presetName
                    rc.selectPreset(instanceid, cmd[3]);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "listpresets command missing required parameter." << endl;
            }
        }
        else if ("getmute" == cmd[0])
        {
            if (cmd.size() > 3)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    unsigned int instanceid = stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    cout << m_renderingControlVector[i]->uri() << endl;
                    // [2]instanceId, [3]channel
                    cout << "Current Mute: " << rc.getMute(instanceid, cmd[3]) << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    unsigned int instanceid = stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    cout << m_renderingControlVector[i]->uri() << endl;
                    // [2]instanceId, channel
                    cout << "Current Mute: " << rc.getMute(instanceid, string("Master")) << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getmute command missing required parameter." << endl;
            }
        }
        else if ("setmute" == cmd[0])
        {
            if (cmd.size() > 4)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    unsigned int instanceid = stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    cout << m_renderingControlVector[i]->uri() << endl;
                    // [2]instanceId, [3]channel, [4]desiredMute
                    bool desiredMute = ((cmd[4] == "true") ? true : false);
                    rc.setMute(instanceid, cmd[3], desiredMute);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else if (cmd.size() > 3)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    unsigned int instanceid = stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    cout << m_renderingControlVector[i]->uri() << endl;
                    // [2]instanceId, channel, [3]desiredMute
                    bool desiredMute = ((cmd[3] == "true") ? true : false);
                    rc.setMute(instanceid, string("Master"), desiredMute);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setmute command missing required parameter." << endl;
            }
        }
        else if ("getvol" == cmd[0])
        {
            if (cmd.size() > 3)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    unsigned int instanceid = stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    cout << m_renderingControlVector[i]->uri() << endl;
                    // [2]instanceId, [3]channel
                    cout << "Current Volume: " << rc.getVolume(instanceid, cmd[3]) << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else if (cmd.size() > 2)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    unsigned int instanceid = stoi(cmd[2]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    cout << m_renderingControlVector[i]->uri() << endl;
                    // [2]instanceId, channel
                    cout << "Current Volume: " << rc.getVolume(instanceid, string("Master")) << endl;
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "getvol command missing required parameter." << endl;
            }
        }
        else if ("setvol" == cmd[0])
        {
            if (cmd.size() > 4)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    unsigned int instanceid = stoi(cmd[2]);
                    int desiredVolume = stoi(cmd[4]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    cout << m_renderingControlVector[i]->uri() << endl;
                    // [2]instanceId, [3]channel, [4]desiredVolume
                    rc.setVolume(instanceid, cmd[3], desiredVolume);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            if (cmd.size() > 3)
            {
                try
                {
                    size_t i = stoi(cmd[1]);
                    if (i >= m_renderingControlVector.size())
                    {
                        cout << "Invalid resource specified." << endl;
                        return;
                    }
                    unsigned int instanceid = stoi(cmd[2]);
                    int desiredVolume = stoi(cmd[3]);
                    RenderingControl rc(m_renderingControlVector[i]);
                    cout << m_renderingControlVector[i]->uri() << endl;
                    // [2]instanceId, channel, [3]desiredVolume
                    rc.setVolume(instanceid, string("Master"), desiredVolume);
                }
                catch (const invalid_argument &ia)
                {
                    cout << "unable to convert string to int." << endl;
                }
            }
            else
            {
                cout << "setvol command missing required parameter." << endl;
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
            cout << "unknown command." << endl;
            print();
        }
    }
}

bool MenuRenderingControl::quit()
{
    return m_quit;
}

void MenuRenderingControl::onFindResource(shared_ptr< OCResource > resource)
{
    lock_guard<mutex> lock(m_mutex);
    cout << "Found resource" << endl;

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
                            cerr << "Mismatch in discovered Services. Reinitializing.";
                            init(m_renderingControlSet);
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
