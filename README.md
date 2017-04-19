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

To build you copy the BuildOptions.txt.example to BuildOptions.txt. Open the
files and update the change the build variables to reflect your environment.

    $ cp BuildOptions.txt.example BuildOptions.txt
    $ vi BuildOptions.txt

To build run 'scons'. Variables specified in the BuildOptions.txt can be
overwritten by adding them to the scons command at run-time.

    $ scons BUILD_TYPE=release

The Client toggles the state of any discovered light and quits after 10 seconds.
Alternatively, if given any runtime argument, the Client runs until Control-C
toggling the state of any discovered light every 5 seconds.

The ObserveClient runs until Control-C, it writes OBSERVE messages to the
console when the state of a light changes.

## Testing
The iotivity-upnp-bridge is in the process of transitioning from the IoTivity
resource container for bridging to the new bridging code found in IoTivity. Also
known as the mini-plugin-manager or MPM. Eventualy the older version of the upnp
bridge that used the resource container will depricated. Until that time both version of the code live can be used.

### Testing MPM version of the bridge
For the time being the MPM version of the bridge relies on the mpm_sample_client
to launch run the bridging plugin. The mpm_sample_client can be found in the
out folder of the IoTivity build.

Run the following:

    $ gupnp-network-light #requires gupnp-tools be installed
    $ ./mpm_sample_client -a -n /<path_to>/libupnpplugin.so
    $ # mpm_sample_client does not automatically detect new devices
    $ # type 0 0 to scan for new upnp devices.
    $ ./Observer
    $ ./SimpleClient

The `gupnp-network-light` is a virtual gupnp light that will only respond to
UPnP. The `mpm_sample_client` is sample program designed to load a bridging
plugin. The `libupnpplugin.so` is the plugin code responsible for discovering
and controlling UPnP devices. The `Observer` finds the UPnP light using IoTivity
and observes any changes in the current lights attributes and reports the
change.  The `SimpleClient` will find the light and will toggle the light state.

### Testing resource container version of the bridge
Navigate to the `out/linux/x86_64/<build_type>/bin` directory.

Run the following:

    $ gupnp-network-light #requires gupnp-tools
    $ ./UpnpBridge
    $ ./Observer
    $ ./SimpleClient

The `gupnp-network-light` is a virtual gupnp light that will only respond to
UPnP. The `UpnpBridge` is the code that bridges IoTivity and UPnP. The `Observer`
finds the UPnP light using IoTivity and observes any changes in the current lights
attributes and reports the change.  The `SimpleClient` will find the light and
will toggle the light state.

## Android UPnP Client App
The Android UPnP client app is built along with all of the other IoTivity Android examples.
These instructions assume that the system has already been set up for Android development.
Follow IoTivity instructions for [Android development](https://www.iotivity.org/)

    $ cd <iotivity>

Copy the Android UPnP client apps into the IoTivity Android example code.

    $ cp -rv <iotivity-upnp-bridge>/android/examples/upnpclient/ java/examples-android
    $ cp -rv <iotivity-upnp-bridge>/android/examples/upnpavclient/ java/examples-android

Edit the settings.gradle file to include the upnpclients.

    $ sed -i "/^include / s/$/, ':upnpclient', ':upnpavclient'/" java/examples-android/settings.gradle

Build for Android.

    $ scons TARGET_OS=android TARGET_ARCH=<armeabi|x86|x86_64> TARGET_TRANSPORT=IP

For example, Nexus 7 Table which has arm chipset.

    $ scons TARGET_OS=android TARGET_ARCH=armeabi TARGET_TRANSPORT=IP

The UPnP Client Demo Android App, 'upnpclient-debug.apk' is in the 'java/examples-android/upnpclient/build/outputs/apk' directory.
The UPnP AV Client Demo Android App, 'upnpavclient-debug.apk' is in the 'java/examples-android/upnpavclient/build/outputs/apk' directory.

## Installing UPnP Client Demo Android App
Follow IoTivity instructions for installing Andriod example apps.

## Testing
Caveat: This currently requires the UpnpBridge running on a Linux system with
UPnP Client Demo Android App
