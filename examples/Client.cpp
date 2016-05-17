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

#include <OCApi.h>
#include <OCPlatform.h>

#include <thread>
#include <stack>
#include <signal.h>

#include "UpnpConstants.h"

using namespace OC;
using namespace std;

static volatile sig_atomic_t s_interrupt = false;

class PowerSwitch
{
    public:
        std::shared_ptr< OCResource > m_resource;
        std::string m_uri; // need to keep uri here, due to OCRepresentation bug
        bool m_state;
        PowerSwitch() :
            m_uri(""), m_state(false)
        {
        }
};

class Brightness
{
    public:
        std::shared_ptr< OCResource > m_resource;
        std::string m_uri; // need to keep uri here, due to OCRepresentation bug
        int m_loadlevel;
        Brightness() :
            m_uri(""), m_loadlevel(100)
        {
        }
};

class Light
{
    public:
        std::shared_ptr< OCResource > m_resource;
        std::string m_uri; // need to keep uri here, due to OCRepresentation bug
        PowerSwitch m_powerSwitch;
        Brightness m_brightness;

        Light() :
            m_resource(nullptr), m_uri("")
        {
        }
        Light(std::shared_ptr< OCResource > resource) :
            m_resource(resource), m_uri(resource->uri())
        {
        }
        Light(const Light &other)
        {
            m_resource = other.m_resource;
            m_uri = other.m_uri;
        }

        Light &operator= (const Light &other)
        {
            if (this != &other)
            {
                m_resource = other.m_resource;
                m_uri = other.m_uri;
            }
            return *this;
        }
        void sendGetRequest(OC::GetCallback cb)
        {
            m_resource->get(QueryParamsMap(), cb);
        }

        void onGetServices(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
        {
            AttributeValue links;
            if (rep.hasAttribute("links"))
            {
                if (rep.getAttributeValue("links", links))
                {
                    try
                    {
                        auto linkRepresentation = boost::get<std::vector<OCRepresentation> >(links);
                        for (auto l : linkRepresentation)
                        {
                            if (l.hasAttribute("rel"))
                            {
                                std::string rel;
                                if (l.getValue("rel", rel))
                                {
                                    //std::cout << "rel: " << rel << std::endl;
                                    if ("contains" == rel)
                                    {
                                        if (l.hasAttribute("rt"))
                                        {
                                            std::string rt;
                                            if (l.getValue("rt", rt))
                                            {
                                                //std::cout << "rt: " << rt << std::endl;
                                                if ("oic.r.switch.binary" == rt)
                                                {
                                                    if (l.hasAttribute("href"))
                                                    {
                                                        if (l.getValue("href", m_powerSwitch.m_uri))
                                                        {
                                                            //std::cout << "href: " << m_powerSwitch.m_uri << std::endl;
                                                        }
                                                        //cout << "calling find " << string(OC_RSRVD_WELL_KNOWN_URI) + "?rt=" + rt << endl;
                                                        // Using the filtered findResource is probably more efficent but is causes an error to be printed from iotivity
                                                        // till that error can be figured out I am not using the filtered fndResource call.
                                                        //OCPlatform::findResource("", string(OC_RSRVD_WELL_KNOWN_URI) + "?rt=" + rt, CT_DEFAULT, bind(&Light::foundServices, this, placeholders::_1));
                                                        OCPlatform::findResource("", string(OC_RSRVD_WELL_KNOWN_URI), CT_DEFAULT,
                                                                                 bind(&Light::foundServices, this, placeholders::_1));
                                                    }
                                                }
                                                else if ("oic.r.light.brightness" == rt)
                                                {
                                                    if (l.hasAttribute("href"))
                                                    {
                                                        if (l.getValue("href", m_brightness.m_uri))
                                                        {
                                                            //std::cout << "href: " << m_brightness.m_uri << std::endl;
                                                        }
                                                        //cout << "calling find " << string(OC_RSRVD_WELL_KNOWN_URI) + "?rt=" + rt << endl;
                                                        //OCPlatform::findResource("", string(OC_RSRVD_WELL_KNOWN_URI) + "?rt=" + rt, CT_DEFAULT, bind(&Light::foundServices, this, placeholders::_1));
                                                        OCPlatform::findResource("", string(OC_RSRVD_WELL_KNOWN_URI), CT_DEFAULT,
                                                                                 bind(&Light::foundServices, this, placeholders::_1));
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    catch (boost::bad_get &e)
                    {
                        std::cout << e.what() << std::endl;
                        std::cout << "falure to getValue for links" << std::endl;
                        std::cout << rep.getValueToString("links");
                    }
                }
                else
                {
                    std::cout << "falure to getValue for links" << std::endl;
                    std::cout << rep.getValueToString("links");
                }
            }
        }

        void foundServices(std::shared_ptr< OCResource > resource)
        {
            try
            {
                if (resource)
                {
                    for (auto &resourceType : resource->getResourceTypes())
                    {
                        if (resourceType == UPNP_OIC_TYPE_POWER_SWITCH)
                        {
                            if (resource->uri() == m_powerSwitch.m_uri)
                            {
                                m_powerSwitch.m_resource = resource;
                            }
                        }
                        else if (resourceType == UPNP_OIC_TYPE_BRIGHTNESS)
                        {
                            if (resource->uri() == m_brightness.m_uri)
                            {
                                m_brightness.m_resource = resource;
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
};

class InternetGatewayDevice
{
    public:
        std::shared_ptr< OCResource > m_resource;
        std::string m_uri;
        std::string m_name;
        std::string m_deviceType;
        std::string m_modelName;
        std::string m_modelNumber;

        InternetGatewayDevice() :
            m_resource(nullptr), m_uri(""), m_name("")
        {
        }
        InternetGatewayDevice(std::shared_ptr< OCResource > resource) :
            m_resource(resource), m_uri(""), m_name("")
        {
        }
};

class MediaRendererDevice
{
    public:
        std::shared_ptr< OCResource > m_resource;
        std::string m_uri;
        std::string m_name;
        std::string m_deviceType;
        std::string m_modelName;
        std::string m_modelNumber;

        MediaRendererDevice() :
            m_resource(nullptr), m_uri(""), m_name("")
        {
        }
        MediaRendererDevice(std::shared_ptr< OCResource > resource) :
            m_resource(resource), m_uri(""), m_name("")
        {
        }

        void sendGetRequest(OC::GetCallback cb)
        {
            m_resource->get(QueryParamsMap(), cb);
        }
};

class MediaServerDevice
{
    public:
        std::shared_ptr< OCResource > m_resource;
        std::string m_uri;
        std::string m_name;
        std::string m_deviceType;
        std::string m_modelName;
        std::string m_modelNumber;

        MediaServerDevice() :
            m_resource(nullptr), m_uri(""), m_name("")
        {
        }
        MediaServerDevice(std::shared_ptr< OCResource > resource) :
            m_resource(resource), m_uri(""), m_name("")
        {
        }

        void sendGetRequest(OC::GetCallback cb)
        {
            m_resource->get(QueryParamsMap(), cb);
        }
};

//static std::map< std::string, std::shared_ptr< OCResource > > s_resourceLookup;
static std::map< std::string, Light > s_lightLookup;
static std::map<string, InternetGatewayDevice> s_gatewayDeviceLookup;
static std::map<string, MediaRendererDevice> s_mediaRendererDeviceLookup;
static std::map<string, MediaServerDevice> s_mediaServerDeviceLookup;
static stack<string> s_menu;

void foundResource(std::shared_ptr< OCResource > resource)
{
    try
    {
        if (resource)
        {
            for (auto &resourceType : resource->getResourceTypes())
            {
                if (resourceType == UPNP_OIC_TYPE_DEVICE_LIGHT)
                {
                    if (s_lightLookup.find(resource->uri()) == s_lightLookup.end())
                    {
                        Light light = Light(resource);
                        s_lightLookup[resource->uri()] = light;
                        light.sendGetRequest(bind(&Light::onGetServices, &s_lightLookup[resource->uri()], placeholders::_1,
                                                  placeholders::_2, placeholders::_3));
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_DEVICE_INET_GATEWAY)
                {
                    if (s_gatewayDeviceLookup.find(resource->uri()) == s_gatewayDeviceLookup.end())
                    {
                        InternetGatewayDevice internetGatewayDevice = InternetGatewayDevice(resource);
                        s_gatewayDeviceLookup[resource->uri()] = internetGatewayDevice;

                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_DEVICE_MEDIA_RENDERER)
                {
                    if (s_mediaRendererDeviceLookup.find(resource->uri()) == s_mediaRendererDeviceLookup.end())
                    {
                        MediaRendererDevice mediaRendererDevice = MediaRendererDevice(resource);
                        s_mediaRendererDeviceLookup[resource->uri()] = mediaRendererDevice;

                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_DEVICE_MEDIA_SERVER)
                {
                    if (s_mediaServerDeviceLookup.find(resource->uri()) == s_mediaServerDeviceLookup.end())
                    {
                        MediaServerDevice mediaServerDevice = MediaServerDevice(resource);
                        s_mediaServerDeviceLookup[resource->uri()] = mediaServerDevice;

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

void descoverResource()
{
    PlatformConfig cfg =
    {
        OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos,
        NULL
    };
    OCPlatform::Configure(cfg);
    try
    {
        while (!s_interrupt)
        {
            OCPlatform::findResource("", OC_RSRVD_WELL_KNOWN_URI, CT_DEFAULT, &foundResource);
            usleep(5000000); // sleep for 5 seconds
        }
    }
    catch (OCException &e)
    {
        oclog() << "Exception in main: " << e.what();
    }
}

void listResources()
{
    cout << "-------Resources found-------" << endl;
    cout << "\t" << s_lightLookup.size() << " Light Devices" << endl;
    cout << "\t" << s_gatewayDeviceLookup.size() << " Internet Gateway Devices" <<
         endl; //TODO add gateway device
    cout << "\t" << s_mediaRendererDeviceLookup.size() << " Media Renderer Devices" <<
         endl; //TODO add gateway device
    cout << "\t" << s_mediaServerDeviceLookup.size() << " Media Server Devices" <<
         endl; //TODO add gateway device
    cout << "\n\tTotal resources " << s_lightLookup.size() + s_gatewayDeviceLookup.size() +
         s_mediaRendererDeviceLookup.size() + s_mediaServerDeviceLookup.size() << endl;
    cout << "-----------------------------" << endl;
}


//current list of menus
const static string MAIN_MENU = "main";
const static string LIGHT_MENU = "light";
const static string MEDIA_RENDERER_MENU = "renderer";
const static string MEDIA_SERVER_MENU = "server";

void mainCommands(const vector<string> &commandlist)
{
    if (commandlist.size() > 0)
    {
        if ("list" == commandlist[0])
        {
            listResources();
        }
        else if ("light" == commandlist[0] || "lights" == commandlist[0])
        {
            s_menu.push(LIGHT_MENU);
        }
        else if ("renderer" == commandlist[0] || "renderers" == commandlist[0])
        {
            s_menu.push(MEDIA_RENDERER_MENU);
        }
        else if ("server" == commandlist[0] || "servers" == commandlist[0])
        {
            s_menu.push(MEDIA_SERVER_MENU);
        }
        else if ("help" == commandlist[0] || "h" == commandlist[0])
        {
            cout << "--main menu commands --\n"
                 "list        print list of discovered resources\n"
                 "[resource]  enter resource specific menu.\n"
                 "            example `light`, `renderer`, `server`, `gateway`, or `fan`\n";
        }
    }
}

void listlights()
{
    if (0 == s_lightLookup.size())
    {
        cout << "Currently no light resources have been discovered." << endl;
    }
    int lightCnt = 0;
    for (auto const &r : s_lightLookup)
    {
        cout << lightCnt << ": ";
        cout << " Resource URI: " << r.first << endl;
        cout << "\tResource address: " << r.second.m_resource->host() << endl;
        for (auto &resourceType : r.second.m_resource->getResourceTypes())
        {
            cout << "\tResource type: " << resourceType << endl;
        }
        lightCnt++;
    }
}

void listMediaRenderers()
{
    if (0 == s_mediaRendererDeviceLookup.size())
    {
        cout << "Currently no media renderer resources have been discovered." << endl;
    }
    int mediaRendererCnt = 0;
    for (auto const &r : s_mediaRendererDeviceLookup)
    {
        cout << mediaRendererCnt << ": ";
        cout << " Resource URI: " << r.first << endl;
        cout << "\tResource address: " << r.second.m_resource->host() << endl;
        for (auto &resourceType : r.second.m_resource->getResourceTypes())
        {
            cout << "\tResource type: " << resourceType << endl;
        }
        mediaRendererCnt++;
    }
}

void listMediaServers()
{
    if (0 == s_mediaServerDeviceLookup.size())
    {
        cout << "Currently no media server resources have been discovered." << endl;
    }
    int mediaServerCnt = 0;
    for (auto const &r : s_mediaServerDeviceLookup)
    {
        cout << mediaServerCnt << ": ";
        cout << " Resource URI: " << r.first << endl;
        cout << "\tResource address: " << r.second.m_resource->host() << endl;
        for (auto &resourceType : r.second.m_resource->getResourceTypes())
        {
            cout << "\tResource type: " << resourceType << endl;
        }
        mediaServerCnt++;
    }
}

void onGetLight(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    try
    {
        if (eCode == OC_STACK_OK)
        {
            std::cout << "GET response" << std::endl;
            std::string repUri;
            rep.getValue("uri", repUri);
            if (s_lightLookup.find(repUri) != s_lightLookup.end())
            {
                auto light = s_lightLookup.find(repUri)->second;
                rep.getValue("uri", light.m_uri);
                std::cout << "\turi: " << light.m_uri << std::endl;
                s_lightLookup[repUri] = light;
            }
        }
        else
        {
            std::cout << "GET Response error: " << eCode << std::endl;
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Caught exception: " << e.what() << " in onGet" << std::endl;
    }
}

void onPostLight(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    (void) rep;
    try
    {
        if (eCode == OC_STACK_OK)
        {
            std::string repUri;
            rep.getValue("uri", repUri);
            std::cout << "POST request successful from uri: " << repUri << std::endl;
            for (auto &lp : s_lightLookup)
            {
                if (lp.second.m_powerSwitch.m_uri == repUri)
                {
                    rep.getValue("state", lp.second.m_powerSwitch.m_state);
                    cout << "\tPower Switch state: " << ((lp.second.m_powerSwitch.m_state) ? "ON" : "OFF") << endl;
                }
                if (lp.second.m_brightness.m_uri == repUri)
                {
                    rep.getValue("brightness", lp.second.m_brightness.m_loadlevel);
                    cout << "\tBrightness loadlevel: " << lp.second.m_brightness.m_loadlevel << "%" << endl;
                }
            }
        }
        else
        {
            std::cout << "POST response error: " << eCode << std::endl;
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Caught exception: " << e.what() << " in onPost" << std::endl;
    }
}

void getLight(string uri)
{
    auto lightItr = s_lightLookup.find(uri);
    if (lightItr != s_lightLookup.end())
    {
        std::cout << "Sending GET request to: " << uri << std::endl;
        lightItr->second.sendGetRequest(&onGetLight);
    }
    else
    {
        cout << "\"" << uri << "\" light resource not found.";
    }
}

void getMediaRenderer(string uri)
{
    auto mediaRendererItr = s_mediaRendererDeviceLookup.find(uri);
    if (mediaRendererItr != s_mediaRendererDeviceLookup.end())
    {
        std::cout << "Sending GET request to: " << uri << std::endl;
        mediaRendererItr->second.sendGetRequest(&onGetLight);
    }
    else
    {
        cout << "\"" << uri << "\" media renderer resource not found.";
    }
}

void getMediaServer(string uri)
{
    auto mediaServerItr = s_mediaServerDeviceLookup.find(uri);
    if (mediaServerItr != s_mediaServerDeviceLookup.end())
    {
        std::cout << "Sending GET request to: " << uri << std::endl;
        mediaServerItr->second.sendGetRequest(&onGetLight);
    }
    else
    {
        cout << "\"" << uri << "\" media server resource not found.";
    }
}

void onOffLight(string uri, bool state)
{
    auto lightItr = s_lightLookup.find(uri);
    if (lightItr != s_lightLookup.end())
    {
        std::cout << "Sending POST request to: " << lightItr->second.m_powerSwitch.m_uri << std::endl;
        OCRepresentation rep;
        rep.setValue("uri", lightItr->second.m_powerSwitch.m_uri);
        rep.setValue("value", state);
        cout << "Sending POST request to: " << lightItr->second.m_powerSwitch.m_uri << " value: " << ((
                    state) ? "ON" : "OFF") << endl;
        lightItr->second.m_powerSwitch.m_resource->post(rep, QueryParamsMap(), &onPostLight);
    }
    else
    {
        bool found = false;
        for (auto &l : s_lightLookup)
        {
            if (l.second.m_powerSwitch.m_uri == uri)
            {
                found = true;
                std::cout << "Sending POST request to: " << l.second.m_powerSwitch.m_uri << " value: " << ((
                              state) ? "ON" : "OFF") << endl;
                OCRepresentation rep;
                rep.setValue("uri", uri);
                rep.setValue("value", state);
                cout << "Posting to uri: " << uri << " value: " << ((state) ? "ON" : "OFF") << endl;
                l.second.m_powerSwitch.m_resource->post(rep, QueryParamsMap(), &onPostLight);
            }
        }
        if (!found)
        {
            cout << "\"" << uri << "\" light resource not found.";
        }
    }
}

void onGetToggleLight(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                      const int eCode)
{
    (void) headerOptions;
    try
    {
        if (eCode == OC_STACK_OK)
        {
            std::cout << "GET response" << std::endl;
            string uri;
            bool value;
            if (rep.getValue("uri", uri) && rep.getValue("value", value))
            {
                cout << "\t" << uri << " " << ((value) ? "ON" : "OFF") << endl;
                onOffLight(uri, !value);
            }
        }
        else
        {
            std::cout << "GET Response error: " << eCode << std::endl;
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Caught exception: " << e.what() << " in onGet" << std::endl;
    }
}

void toggleLight(string uri)
{
    auto lightItr = s_lightLookup.find(uri);
    if (lightItr != s_lightLookup.end())
    {
        lightItr->second.m_powerSwitch.m_resource->get(QueryParamsMap(), &onGetToggleLight);
    }
    else
    {
        cout << "\"" << uri << "\" light resource not found.";
    }
}

void dimLight(string uri, int value)
{
    auto lightItr = s_lightLookup.find(uri);
    if (lightItr != s_lightLookup.end())
    {
        std::cout << "Sending POST request to: " << lightItr->second.m_brightness.m_uri << "loadlevel: " <<
                  value << "%" << std::endl;
        OCRepresentation rep;
        rep.setValue("uri", lightItr->second.m_brightness.m_uri);
        rep.setValue("brightness", value);
        lightItr->second.m_brightness.m_resource->post(rep, QueryParamsMap(), &onPostLight);
    }
    else
    {
        cout << "\"" << uri << "\" light resource not found.";
    }
}
void lightCommands(const vector<string> &commandlist)
{
    const string DIM_COMMAND = "dim";
    const string ON_COMMAND = "on";
    const string OFF_COMMAND = "off";
    const string TOGGLE_COMMAND = "toggle";
    const string LIST_COMMAND = "list";
    const string GET_COMMAND = "get";
    const string HELP_COMMAND = "help";
    const string H_COMMAND = "h";
    const string ALL_KEYWORD = "all";
    if (commandlist.size() == 1)
    {
        if (LIST_COMMAND == commandlist[0])
        {
            listlights();
        }
        else if (ON_COMMAND == commandlist[0] ||
                 OFF_COMMAND == commandlist[0] ||
                 GET_COMMAND == commandlist[0] ||
                 LIST_COMMAND == commandlist[0] ||
                 TOGGLE_COMMAND == commandlist[0] ||
                 DIM_COMMAND == commandlist[0])
        {
            cout << "missing [uri] or keyword `all` for the command." << endl;
        }
        else if (HELP_COMMAND == commandlist[0] || H_COMMAND == commandlist[0])
        {
            cout << "--light menu commands --\n"
                 "For all commands the uri can be replaced with the keyword `all` to run the specified command\n"
                 "on all discovered lights. The command `list` or `get all` can be used to obtain the uri for\n"
                 "each individual light.\n"
                 "\n"
                 "list                 Print list of discovered lights\n"
                 "get [uri]            GET request for the light resource.\n"
                 "on [uri]             Turn on the light specified by the uri.\n"
                 "off [uri]            Turn off the light specified by the uri.\n"
                 "toggle [uri]         Toggle the power of the light off or on based on its current state.\n"
                 "dim [uri] [0-100]    Set the brightness of the light between 0* and 100%\n"
                 "\n";
        }
    }
    if (commandlist.size() == 2)
    {
        if (ON_COMMAND == commandlist[0])
        {
            if (ALL_KEYWORD == commandlist[1])
            {
                for (auto r : s_lightLookup)
                {
                    onOffLight(r.second.m_uri, true);
                }
            }
            else
            {
                onOffLight(commandlist[1], true);
            }
        }
        else if (OFF_COMMAND == commandlist[0])
        {
            if (ALL_KEYWORD == commandlist[1])
            {
                for (auto r : s_lightLookup)
                {
                    onOffLight(r.second.m_uri, false);
                }
            }
            else
            {
                onOffLight(commandlist[1], false);
            }
        }
        else if (TOGGLE_COMMAND == commandlist[0])
        {
            if (ALL_KEYWORD == commandlist[1])
            {
                for (auto r : s_lightLookup)
                {
                    toggleLight(r.second.m_uri);
                }
            }
            else
            {
                toggleLight(commandlist[1]);
            }
        }
        else if (GET_COMMAND == commandlist[0])
        {
            if (ALL_KEYWORD == commandlist[1])
            {
                for (auto r : s_lightLookup)
                {
                    getLight(r.second.m_uri);
                }
            }
            else
            {
                getLight(commandlist[1]);
            }
        }
    }
    if (commandlist.size() == 3)
    {
        if (DIM_COMMAND == commandlist[0])
        {
            int value = atoi(commandlist[2].c_str());
            if (value >= 0 && value <= 100)
            {
                if (ALL_KEYWORD == commandlist[1])
                {
                    for (auto r : s_lightLookup)
                    {
                        dimLight(r.second.m_uri, value);
                    }
                }
                else
                {
                    dimLight(commandlist[1], value);
                }
            }
            else
            {
                cerr << "the dim value must between 0 and 100" << endl;
            }
        }
    }
}

void mediaRendererCommands(const vector<string> &commandlist)
{
    const string LIST_COMMAND = "list";
    const string GET_COMMAND = "get";
    const string HELP_COMMAND = "help";
    const string H_COMMAND = "h";
    const string ALL_KEYWORD = "all";
    if (commandlist.size() == 1)
    {
        if (LIST_COMMAND == commandlist[0])
        {
            listMediaRenderers();
        }
        else if (GET_COMMAND == commandlist[0] ||
                 LIST_COMMAND == commandlist[0])
        {
            cout << "missing [uri] or keyword `all` for the command." << endl;
        }
        else if (HELP_COMMAND == commandlist[0] || H_COMMAND == commandlist[0])
        {
            cout << "-- renderer menu commands --\n"
                 "For all commands the uri can be replaced with the keyword `all` to run the specified command\n"
                 "on all discovered media renderers. The command `list` or `get all` can be used to obtain the uri for\n"
                 "each individual media renderer.\n"
                 "\n"
                 "list                 Print list of discovered media renderer\n"
                 "get [uri]            GET request for the media renderer resource.\n"
                 "\n";
        }
    }
    if (commandlist.size() == 2)
    {
        if (GET_COMMAND == commandlist[0])
        {
            if (ALL_KEYWORD == commandlist[1])
            {
                for (auto r : s_mediaRendererDeviceLookup)
                {
                    getMediaRenderer(r.second.m_uri);
                }
            }
            else
            {
                getMediaRenderer(commandlist[1]);
            }
        }
    }
}

void mediaServerCommands(const vector<string> &commandlist)
{
    const string LIST_COMMAND = "list";
    const string GET_COMMAND = "get";
    const string HELP_COMMAND = "help";
    const string H_COMMAND = "h";
    const string ALL_KEYWORD = "all";
    if (commandlist.size() == 1)
    {
        if (LIST_COMMAND == commandlist[0])
        {
            listMediaServers();
        }
        else if (GET_COMMAND == commandlist[0] ||
                 LIST_COMMAND == commandlist[0])
        {
            cout << "missing [uri] or keyword `all` for the command." << endl;
        }
        else if (HELP_COMMAND == commandlist[0] || H_COMMAND == commandlist[0])
        {
            cout << "-- server menu commands --\n"
                 "For all commands the uri can be replaced with the keyword `all` to run the specified command\n"
                 "on all discovered media servers. The command `list` or `get all` can be used to obtain the uri for\n"
                 "each individual media server.\n"
                 "\n"
                 "list                 Print list of discovered media server\n"
                 "get [uri]            GET request for the media server resource.\n"
                 "\n";
        }
    }
    if (commandlist.size() == 2)
    {
        if (GET_COMMAND == commandlist[0])
        {
            if (ALL_KEYWORD == commandlist[1])
            {
                for (auto r : s_mediaServerDeviceLookup)
                {
                    getMediaServer(r.second.m_uri);
                }
            }
            else
            {
                getMediaServer(commandlist[1]);
            }
        }
    }
}

void parseCmd(const string &cmd)
{
    vector<string> token;
    std::stringstream ss(cmd);
    std::string tok;
    while (getline(ss, tok, ' '))
    {
        if (!tok.empty())
        {
            token.push_back(tok);
        }
    }

    if (MAIN_MENU == s_menu.top())
    {
        mainCommands(token);
    }
    else if (LIGHT_MENU == s_menu.top())
    {
        lightCommands(token);
    }
    else if (MEDIA_RENDERER_MENU == s_menu.top())
    {
        mediaRendererCommands(token);
    }
    else if (MEDIA_SERVER_MENU == s_menu.top())
    {
        mediaServerCommands(token);
    }
    // commands available regardless of menu
    if (token.size() > 0)
    {
        if ("quit" == token[0] || "exit" == token[0])
        {
            s_interrupt = true;
        }
        else if ("up" == token[0] || "back" == token[0])
        {
            if (MAIN_MENU == s_menu.top())
            {
                return; //do nothing
            }
            else
            {
                s_menu.pop();
            }
        }
        else if ("help" == token[0] || "h" == token[0])
        {
            cout << "--commands available from every menu--\n"
                 "up, back    return to previous menu\n"
                 "help, h     print this help text.\n"
                 "quit, exit  exit program.\n";
        }
    }
}

int main(int argc, char *argv[])
{
    thread discover(descoverResource);

    cout << "Enter command or type help" << endl;
    s_menu.push(MAIN_MENU);
    string cmd;
    do
    {
        cout << s_menu.top() << "> ";

        getline(cin, cmd);
        parseCmd(cmd);
    }
    while (!s_interrupt);
    cout << "Exiting please wait..." << endl;
    discover.join();
    return 0;
}
