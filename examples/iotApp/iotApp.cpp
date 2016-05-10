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
#include <mutex>
#include <thread>
#include <set>
#include "UpnpConstants.h"
#include "BinarySwitch.h"
#include "Brightness.h"
#include "commandlineMode.h"

using namespace OC;
using namespace std;

int main(int argc, char *argv[])
{
    bool interactiveMode = false;
    if(argc == 1) {
        interactiveMode = true;
    }

    PlatformConfig cfg =
    { OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos,
    NULL };
    OCPlatform::Configure(cfg);

    if(interactiveMode) {
        std::cout << "running in interactive mode\n";
        // TODO
    } else {
        Commandline cl;
        cl.process(argc, argv);
    }
    return 0;
}
