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

#include "UpnpConstants.h"

using namespace OC;

class Light
{
    public:
        Light()
        {
        }
};

class PowerSwitch
{
    public:
        bool m_state;
        PowerSwitch() :
            m_state(false)
        {
        }
};

class Brightness
{
    public:
        int m_loadlevel;
        Brightness() :
            m_loadlevel(100)
        {
        }
};

static std::map< std::string, std::shared_ptr< OCResource > > s_resourceLookup;
static std::map< std::string, Light > s_lightLookup;
static std::map< std::string, PowerSwitch > s_powerSwitchLookup;
static std::map<std::string, Brightness> s_brightnessLookup;

void GetLightRepresentation(std::shared_ptr< OCResource > resource);
void PostLightRepresentation(std::shared_ptr< OCResource > resource);

void GetPowerSwitchRepresentation(std::shared_ptr< OCResource > resource);
void PostPowerSwitchRepresentation(std::shared_ptr< OCResource > resource);

void GetBrightnessRepresentation(std::shared_ptr< OCResource > resource);
void PostBrightnessRepresentation(std::shared_ptr< OCResource > resource);

void OnPostPowerSwitch(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                       const int eCode)
{
    (void) headerOptions;
    (void) rep;
    try
    {
        if (eCode == OC_STACK_OK)
        {
            std::cout << "POST request successful" << std::endl;
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Caught exception: " << e.what() << " in onPostPowerSwitch" << std::endl;
    }

}

void OnPostBrightness(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                      const int eCode)
{
    (void) headerOptions;
    try
    {
        if (eCode == OC_STACK_OK)
        {
            std::cout << "POST Brightness request successful" << std::endl;
            std::string repUri = rep.getUri();

            Brightness brightness = s_brightnessLookup[repUri];

            rep.getValue("brightness", brightness.m_loadlevel);
            std::cout << "\turi: " << repUri << std::endl;
            std::cout << "\tbrightness: " << brightness.m_loadlevel << std::endl;
            s_brightnessLookup[repUri] = brightness; // save new copy
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Caught exception: " << e.what() << " in onPostBrightness" << std::endl;
    }
}

void OnPostLight(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    try
    {
        if (eCode == OC_STACK_OK)
        {
            std::cout << "POST request successful" << std::endl;
            std::string repUri = rep.getUri();

            Light light = s_lightLookup[repUri];

            std::cout << "\turi: " << repUri << std::endl;
            s_lightLookup[repUri] = light; // save new copy
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

void OnGetLight(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    try
    {
        if (eCode == OC_STACK_OK)
        {
            std::cout << "GET response" << std::endl;
            std::string repUri = rep.getUri();

            std::shared_ptr< OCResource > lightResource = s_resourceLookup[repUri];
            Light light = s_lightLookup[repUri];

            std::cout << "\turi: " << repUri << std::endl;
            s_lightLookup[repUri] = light; // save new copy

            PostLightRepresentation(lightResource);
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

void OnGetPowerSwitch(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                      const int eCode)
{
    (void) headerOptions;
    try
    {
        if (eCode == OC_STACK_OK)
        {
            std::cout << "GET response" << std::endl;
            std::string repUri = rep.getUri();

            std::shared_ptr< OCResource > powerSwitchResource = s_resourceLookup[repUri];
            PowerSwitch powerSwitch = s_powerSwitchLookup[repUri];

            rep.getValue("value", powerSwitch.m_state);
            std::cout << "\turi: " << repUri << std::endl;
            std::cout << "\tstate: " << powerSwitch.m_state << std::endl;

            s_powerSwitchLookup[repUri] = powerSwitch; // save new copy

            PostPowerSwitchRepresentation(powerSwitchResource);
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

void OnGetBrightness(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                     const int eCode)
{
    (void) headerOptions;
    try
    {
        if (eCode == OC_STACK_OK)
        {
            std::cout << "GET Brightness response" << std::endl;
            std::string repUri = rep.getUri();

            std::shared_ptr< OCResource > brightnessResource = s_resourceLookup[repUri];
            Brightness brightness = s_brightnessLookup[repUri];

            rep.getValue("brightness", brightness.m_loadlevel);
            std::cout << "\turi: " << repUri << std::endl;
            std::cout << "\tbrightness: " << brightness.m_loadlevel << std::endl;

            s_brightnessLookup[repUri] = brightness; // save new copy

            PostBrightnessRepresentation(brightnessResource);
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

void PostLightRepresentation(std::shared_ptr< OCResource > resource)
{
    OCRepresentation rep;
    std::cout << "Sending POST request to: " << resource->uri() << std::endl;

    rep.setUri(resource->uri()); // must be set to be available in OnLight()
    resource->post(rep, QueryParamsMap(), &OnPostLight);
}

void PostPowerSwitchRepresentation(std::shared_ptr< OCResource > resource)
{
    OCRepresentation rep;
    std::cout << "Sending POST request to: " << resource->uri() << std::endl;

    PowerSwitch powerSwitch = s_powerSwitchLookup[resource->uri()];

    rep.setUri(resource->uri()); // must be set to be available in OnPowerSwitch()

    rep.setValue("value", !powerSwitch.m_state);

    resource->post(rep, QueryParamsMap(), &OnPostPowerSwitch);
}

void PostBrightnessRepresentation(std::shared_ptr< OCResource > resource)
{
    OCRepresentation rep;
    std::cout << "Sending POST Brightness request to: " << resource->uri() << std::endl;

    Brightness brightness = s_brightnessLookup[resource->uri()];

    rep.setUri(resource->uri()); // must be set to be available in OnBrightness()

    std::cout << "Current brightness level " << brightness.m_loadlevel << std::endl;
    int loadlevel = brightness.m_loadlevel - 20;
    loadlevel = ((loadlevel < 20) ? 100 : loadlevel); //don't drop below 20% so we can tell light is on

    std::cout << "Setting brightness loadlevel to " << loadlevel << std::endl;
    rep.setValue("brightness", loadlevel);

    resource->post(rep, QueryParamsMap(), &OnPostBrightness);
}

void GetLightRepresentation(std::shared_ptr< OCResource > resource)
{
    std::cout << "Sending GET request to: " << resource->uri() << std::endl;
    resource->get(QueryParamsMap(), &OnGetLight);
}

void GetPowerSwitchRepresentation(std::shared_ptr< OCResource > resource)
{
    std::cout << "Sending GET request to: " << resource->uri() << std::endl;
    resource->get(QueryParamsMap(), &OnGetPowerSwitch);
}

void GetBrightnessRepresentation(std::shared_ptr< OCResource > resource)
{
    std::cout << "Sending GET request to: " << resource->uri() << std::endl;
    resource->get(QueryParamsMap(), &OnGetBrightness);
}

void foundResource(std::shared_ptr< OCResource > resource)
{
    std::string resourceUri;
    std::string hostAddress;

    try
    {
        if (resource)
        {
            std::cout << "DISCOVERED Resource:" << std::endl;
            resourceUri = resource->uri();
            std::cout << "\tResource uri: " << resourceUri << std::endl;
            hostAddress = resource->host();
            std::cout << "\tResource address: " << hostAddress << std::endl;

            std::cout << "\tList of resource interfaces: " << std::endl;
            for (auto &resourceInterface : resource->getResourceInterfaces())
            {
                std::cout << "\t\t" << resourceInterface << std::endl;
            }

            std::cout << "\tList of resource types: " << std::endl;

            for (auto &resourceType : resource->getResourceTypes())
            {
                std::cout << "\t\t" << resourceType << std::endl;
                if (resourceType == UPNP_OIC_TYPE_DEVICE_LIGHT)
                {
                    if (s_lightLookup.find(resourceUri) == s_lightLookup.end())
                    {
                        Light light = Light();
                        s_lightLookup[resourceUri] = light;

                        std::shared_ptr< OCResource > lightResource = resource;
                        s_resourceLookup[resourceUri] = lightResource;

                        GetLightRepresentation(lightResource);
                    }
                    else
                    {
                        //std::cout << "Already discovered uri: " << resourceUri << std::endl;
                        s_resourceLookup[resourceUri] = resource;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_POWER_SWITCH)
                {
                    if (s_powerSwitchLookup.find(resourceUri) == s_powerSwitchLookup.end())
                    {
                        PowerSwitch powerSwitch = PowerSwitch();
                        s_powerSwitchLookup[resourceUri] = powerSwitch;

                        std::shared_ptr< OCResource > powerSwitchResource = resource;
                        s_resourceLookup[resourceUri] = powerSwitchResource;

                        GetPowerSwitchRepresentation(powerSwitchResource);
                    }
                    else
                    {
                        //std::cout << "Already discovered uri: " << resourceUri << std::endl;
                        s_resourceLookup[resourceUri] = resource;
                    }
                }
                else if (resourceType == UPNP_OIC_TYPE_BRIGHTNESS)
                {
                    std::cout << "found Brightness resouce." << std::endl;
                    if (s_brightnessLookup.find(resourceUri) == s_brightnessLookup.end())
                    {
                        std::cout << "adding Brightness resouce." << std::endl;
                        Brightness brightness = Brightness();
                        s_brightnessLookup[resourceUri] = brightness;

                        std::shared_ptr< OCResource > brightnessResource = resource;
                        s_resourceLookup[resourceUri] = brightnessResource;
                        std::cout << "Calling GetBrightnessRepresentation Brightness resouce." << std::endl;
                        GetBrightnessRepresentation(brightnessResource);
                    }
                    else
                    {
                        //std::cout << "Already discovered uri: " << resourceUri << std::endl;
                        s_resourceLookup[resourceUri] = resource;
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

int main(int argc, char *argv[])
{
    PlatformConfig cfg =
    {
        OC::ServiceType::InProc, OC::ModeType::Both, OC_DEFAULT_ADAPTER, OC::QualityOfService::LowQos,
        NULL
    };
    OCPlatform::Configure(cfg);
    try
    {
        std::cout << "Finding Resource... " << std::endl;
        OCPlatform::findResource("", OC_RSRVD_WELL_KNOWN_URI, CT_DEFAULT, &foundResource);
        if (argc > 1)
        {
            while (true)
            {
                usleep(5000000); // sleep for 5 seconds
                OCPlatform::findResource("", OC_RSRVD_WELL_KNOWN_URI, CT_DEFAULT, &foundResource);
            }
        }
        else
        {
            usleep(10000000); // run for 10s.
        }

    }
    catch (OCException &e)
    {
        oclog() << "Exception in main: " << e.what();
    }
    return 0;
}
