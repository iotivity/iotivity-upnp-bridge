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

#ifndef COMMANDLINEMODE_H_
#define COMMANDLINEMODE_H_

#include <OCApi.h>
#include <OCPlatform.h>
#include <unistd.h>

#include <functional>
#include <memory>
#include <string>

#include "IotivityUtility.h"

class CommandlineMode {
public:
    CommandlineMode();
    void run(int argc, char *argv[]);
private:
    void printHelp();
    void foundResource(std::shared_ptr< OC::OCResource > resource);
    void testBrightness(std::shared_ptr< OC::OCResource > resource);
    void testSwitch(std::shared_ptr< OC::OCResource > resource);
    bool listAllDevices;
    bool listAllServices;
    bool runTest;
    bool runInLoop;
    int  loopCount;
    int  loopDelayTime;
    int waitTime;

    std::shared_ptr<OC::OCResource> m_devices;
    std::shared_ptr<OC::OCResource> m_services;
    std::shared_ptr<OC::OCResource> m_other;

    OC::FindCallback onFoundCB;
    OC::FindCallback testBrightnessCB;
    OC::FindCallback testSwitchCB;
};

CommandlineMode::CommandlineMode() :
    listAllDevices(false),
    listAllServices(false),
    runTest(false),
    runInLoop(false),
    loopCount(5),
    loopDelayTime(10),
    waitTime(-1){

    onFoundCB        = std::bind(&CommandlineMode::foundResource, this, std::placeholders::_1);
    testBrightnessCB = std::bind(&CommandlineMode::testBrightness, this, std::placeholders::_1);
    testSwitchCB     = std::bind(&CommandlineMode::testSwitch, this, std::placeholders::_1);

}

void CommandlineMode::run(int argc, char *argv[])
{
    bool listAllDevices = false;
    bool listAllServices = false;
    /* Parse command line args */
    for (int i = 1; i < argc; ++i) {
        // currently all commands should be at least 2 characters
        if(strlen(argv[i]) < 2) {
            std::cerr << "Invalid input option" << std::endl;
            printHelp();
            exit(0);
        }
        // split the input so we can check for the "=" character for options
        // that take the "=" character.
        char* token = strtok(argv[i], "= ");
        std::cout << "token>> " << token << std::endl;
        if (0 == strcmp("--help", token) || 0 == strcmp("-h", token)) {
            printHelp();
            exit(0);
        } else if (0 == strcmp("--list", token)) {
            listAllDevices = true;
            listAllServices = true;
        } else if (0 == strncmp("--time", token, strlen("--time"))) {
            token = strtok(nullptr, "=");
            try {
                waitTime = std::stoi(token);
                std::cout << "Waiting " << waitTime << " seconds before exiting." << std::endl;
            } catch (const std::invalid_argument& ia) {
                std::cerr << "Invalid input option for --loop option" << std::endl;
                printHelp();
                exit(0);
            }
        } else if (0 == strcmp("--test", token)) {
            runTest = true;
        } else if (0 == strcmp("--loop", token)) {
            runInLoop = true;
            token = strtok(nullptr, "=");
            if (token != nullptr) {
                if (token[0] != '-') {
                    try {
                        loopCount = std::stoi(token);
                    } catch (const std::invalid_argument& ia) {
                        std::cerr << "Invalid input option for --loop option" << std::endl;
                        printHelp();
                        exit(0);
                    }
                } else {
                    loopCount = -1; //use -1 to indicate infinite loops
                }
            } else {
                loopCount = -1; //use -1 to indicate infinite loops
            }
        } else if (0 == strcmp("--delay", token)) {
            token = strtok(nullptr, "=");
            if (nullptr != token) {
                if (token[0] != '-') {
                    try {
                        loopDelayTime = std::stoi(token);
                    } catch (const std::invalid_argument& ia) {
                        std::cerr << "Invalid input option for --delay option" << std::endl;
                        printHelp();
                        exit(0);
                    }
                } else {
                    std::cerr << "--delay option can not have negative time" << std::endl;
                    printHelp();
                    exit(0);
                }
            } else {
                std::cerr << "Missing time for --delay option" << std::endl;
                printHelp();
                exit(0);
            }
        }

    }

    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << " Run test : " << ((runTest) ? "TRUE" : "FALSE") << std::endl;
    std::cout << " List Discovered resources " << ((listAllDevices || listAllServices) ? "TRUE" : "FALSE") << std::endl;
    std::cout << " Loop Mode: " << ((runInLoop) ? "TRUE" : "FALSE") <<
                 " LoopCount: ";
    ((loopCount == -1) ? std::cout << "ENDLESS" : std::cout << loopCount);
    std::cout << " LoopDelay: " << loopDelayTime << std::endl;
    if(loopCount != -1) { // The program is in endless mode SIGINT required to exit.
        if (waitTime == -1) {
            std::cout <<" Program will wait for 'q' to exit" << std::endl;
        } else {
            std::cout << " Wait " << waitTime << " seconds then exit" << std::endl;
        }
    }
    std::cout << "--------------------------------------------------" << std::endl;
    size_t loopIndicator = 1;
    do {
        if(runInLoop) {
            std::cout << "[LOOP " << loopIndicator++ << "]" << std::endl;
        }
        if(listAllDevices || listAllServices) {
            std::cout << "Finding all Resources."  << std::endl;
            OC::OCPlatform::findResource("", OC_RSRVD_WELL_KNOWN_URI, CT_DEFAULT, onFoundCB);
        }
        // Run pre-programmed test code that is used to verify functionality of found resource
        if(runTest) {
            OC::OCPlatform::findResource("", std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_BRIGHTNESS, CT_DEFAULT, testBrightnessCB);
            OC::OCPlatform::findResource("", std::string(OC_RSRVD_WELL_KNOWN_URI) +  "?rt=" + UPNP_OIC_TYPE_POWER_SWITCH, CT_DEFAULT, testSwitchCB);
        }
        if(runInLoop) {
            sleep(loopDelayTime);
        }
    } while (runInLoop && (loopCount == -1 || --loopCount > 0));
    if (-1 == waitTime) {
        std::cout << "Enter 'q' to exit program." << std::endl;
        while (getchar() != 'q');
    } else {
        std::cout << "Waiting " << waitTime << " seconds for all actions to complete before exiting." << std::endl;
        sleep(waitTime); //wait x seconds then exit
        std::cout << "EXITING" << std::endl;
    }
}

void CommandlineMode::printHelp() {
    std::cout <<
            "Usage: iotApp [OPTION]...\n"
            "\n"
            "The iotApp runs in two modes: interactive mode and commandline mode. To enter\n"
            "interactive mode run iotApp with no additional arguments. This will print text driven\n"
            "menus that can be used to find and interact with IoTivity devices and services.\n"
            "Commandline can run pre-programmed actions for testing and verification of IoTivity\n"
            "Services.\n"
            "\n"
            "Optional arguments for commandline mode are:\n"
            "---------------------------------------------------------------------------------------\n"
            "    --list          Discover and print all found Iotivity resources\n"
            "    --test          Run pre-programmed test code\n"
            "    --loop[=count]  Run the program in a loop if count is not given it will run in an\n"
            "                    endless loop. The program will not exit till an interupt signal is\n"
            "                    recieved.\n"
            "    --delay=time    The delay option is used to specify how long the program will wait\n"
            "                    in seconds between each loop when using the --loop option. If no\n"
            "                    delay option is specified the delay time will default to 10 seconds\n"
            "    --time=time     The number of seconds the program will wait for async actions to\n"
            "                    complete before exiting the program. If no time is given 'q' must\n"
            "                    be typed to quit the program upon completion.\n"
            "-h, --help          print this help text." << std::endl;
}

void CommandlineMode::foundResource(std::shared_ptr< OC::OCResource > resource) {
    //printResourceCompact(resource);
    printResourceInformation(resource);
}

void CommandlineMode::testBrightness(std::shared_ptr< OC::OCResource > resource) {
    Brightness b(resource);
    b.getBrightnessAsync([b](const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode)
            {
        if (eCode == OC_STACK_OK)
        {
            int brightness;
            std::string uri;
            rep.getValue("uri", uri);
            if(uri == b.getResource()->uri()) {
                rep.getValue("brightness", brightness);
            }
            brightness -= 20;
            brightness = ((brightness < 20) ? 100 : brightness); //don't drop below 20% so we can tell light is on
            b.setBrgithnessAsync(brightness, [](const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode){});
        }
            });
}

void CommandlineMode::testSwitch(std::shared_ptr< OC::OCResource > resource) {
    BinarySwitch s(resource);
    s.isOnAsync([s](const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode)
            {
        if (eCode == OC_STACK_OK)
        {
            bool powerState;
            std::string uri;
            rep.getValue("uri", uri);
            if(uri == s.getResource()->uri()) {
                rep.getValue("value", powerState);
            }
            s.turnOnAsync(!powerState, [](const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode){});
        }
            });
}

#endif
