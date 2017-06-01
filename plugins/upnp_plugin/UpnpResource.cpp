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
#include <boost/regex.hpp>

#include "UpnpResource.h"
#include "UpnpConstants.h"
#include "UpnpInternal.h"

using namespace std;
using namespace boost;

static const string MODULE = "UpnpResource";

UpnpResource::UpnpResource()
{
}

UpnpResource::~UpnpResource()
{
}

void UpnpResource::setRegistered(bool registered)
{
    m_registered = registered;
}

bool UpnpResource::isRegistered()
{
    return m_registered;
}

void UpnpResource::addLink(UpnpResource::Ptr pResource)
{
    _link singleLink;

    singleLink.href = pResource->m_uri;
    singleLink.rel = LINK_REL_CONTAINS;
    singleLink.rt = pResource->getResourceType();
    // Embedded devices are all generic upnp devices for now
    if (singleLink.rt.find(UPNP_OIC_TYPE_DEVICE_PREFIX) == 0) {
        singleLink.rt = UPNP_DEVICE_RESOURCE;
    }
    // Non-light services are all generic upnp services for now
    else if (singleLink.rt.find(UPNP_OIC_TYPE_SERVICE_PREFIX) == 0) {
        if (!(singleLink.rt == UPNP_OIC_TYPE_BRIGHTNESS || singleLink.rt == UPNP_OIC_TYPE_POWER_SWITCH))
        {
            singleLink.rt = UPNP_SERVICE_RESOURCE;
        }
    }

    m_links.push_back(singleLink);
}

void UpnpResource::setLinkAttribute()
{
}

std::string UpnpResource::getResourceType()
{
    return m_resourceType;
}

std::string UpnpResource::getUdn()
{
    return m_udn;
}

void UpnpResource::setReady(bool isReady)
{
    m_ready = isReady;
}

bool UpnpResource::isReady()
{
    return m_ready;
}
