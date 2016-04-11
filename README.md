<!---
  ~ //******************************************************************
  ~ //
  ~ // Copyright 2106 Intel Corporation All Rights Reserved.
  ~ //
  ~ //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  ~ //
  ~ // Licensed under the Apache License, Version 2.0 (the "License");
  ~ // you may not use this file except in compliance with the License.
  ~ // You may obtain a copy of the License at
  ~ //
  ~ //      http://www.apache.org/licenses/LICENSE-2.0
  ~ //
  ~ // Unless required by applicable law or agreed to in writing, software
  ~ // distributed under the License is distributed on an "AS IS" BASIS,
  ~ // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  ~ // See the License for the specific language governing permissions and
  ~ // limitations under the License.
  ~ //
  ~ //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  --->

## Install required build libraries

### Ubuntu
    $ sudo apt-get install libgupnp-1.0-dev libgssdp-1.0-dev libboost-regex-dev
    $ sudo apt-get install gupnp-tools #optional but recommended

### Fedora
    $ sudo dnf install gssdp-devel gupnp-devel
    $ sudo dnf install gupnp-tools #optional but recommended

## Build
You will need to copy the setenv.sh.example to setenv.sh then setup the setenv.sh
file to reflect your environment. You can then source the setenv.sh to setup the
enviroment.

    $ cp setenv.sh.example setenv.sh 
    $ vi setenv.sh
    $ source setenv.sh

To build you will need to copy the BuildOptions.txt.example to BuildOptions.txt
Open the files and update the change the build variables to reflect your
environment.

    $ cp BuildOptions.txt.example BuildOptions.txt
    $ vi BuildOptions.txt

To build run 'scons'. Variables specified in the BuildOptions.txt can be
overwriten by adding them to the scons command at runtime.

    $ scons BUILD_TYPE=release

The Client toggles the state of any discovered light and quits after 10 seconds.
Alternatively, if given any runtime argument, the Client runs until Control-C
toggling the state of any discovered light every 5 seconds.

The ObserveClient runs until Control-C, it writes OBSERVE messages to the
console when the state of a light changes.

## Testing
Navigate to the `out/linux/x86_64/<build_type>/bin` directory.

Run the following:

    $ gupnp-network-light #requires gupnp-tools
    $ ./Driver
    $ ./ObserveClient
    $ ./Client

The `gupnp-network-light` is a virtual gupnp light that will only respond to
UPnP. The `Driver` is the code that bridges IoTivity and UPnP. The `ObserverClient`
finds the UPnP light using IoTivity and observes any changes in the current lights
attributes and reports the change.  The `Client` will find the light and will
toggle the light state.
