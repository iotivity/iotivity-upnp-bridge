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

#include <gtest/gtest.h>
#include <UpnpInternal.h>
#include <UpnpHelper.h>

TEST(UpnpHelper, findResourceTypeLight) {
    EXPECT_STREQ(UPNP_OIC_TYPE_DEVICE_LIGHT.c_str(), findResourceType("urn:schemas-upnp-org:device:Light").c_str());
    EXPECT_STREQ(UPNP_OIC_TYPE_DEVICE_LIGHT.c_str(), findResourceType("urn:schemas-upnp-org:device:light").c_str());
}

TEST(UpnpHelper, findResourceTypeSwitchPower) {
    EXPECT_STREQ(UPNP_OIC_TYPE_POWER_SWITCH.c_str(), findResourceType("urn:schemas-upnp-org:service:SwitchPower").c_str());
    EXPECT_STREQ(UPNP_OIC_TYPE_POWER_SWITCH.c_str(), findResourceType("urn:schemas-upnp-org:service:switchpower").c_str());
}

TEST(UpnpHelper, findResourceTypeDimming) {
    EXPECT_STREQ(UPNP_OIC_TYPE_BRIGHTNESS.c_str(), findResourceType("urn:schemas-upnp-org:service:Dimming").c_str());
    EXPECT_STREQ(UPNP_OIC_TYPE_BRIGHTNESS.c_str(), findResourceType("urn:schemas-upnp-org:service:dimming").c_str());
}

