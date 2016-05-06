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

#include "UpnpContentDirectoryService.h"

using namespace OIC::Service;

static const string MODULE = "UpnpContentDirectoryService";

// Content Directory Service

// Organization:
// Attribute Name,
// State Variable Name (can be empty string), State variable type, "evented" flag
// Actions:
//    0: "GET" action name, action type, optional out parameters: var_name,var_type
//    1: "SET" action name, action type, optional in parameters: var_name,var_type
// Vector of embedded attributes (if present)
vector <UpnpAttributeInfo> UpnpContentDirectory::Attributes = {
    {"searchCaps",
     "SearchCapabilities", G_TYPE_STRING, false,
     {{"GetSearchCapabilities", UPNP_ACTION_GET, "SearchCaps", G_TYPE_STRING},
      },
     {}
    },
    {"sortCaps",
     "SortCapabilities", G_TYPE_STRING, false,
     {{"GetSortCapabilities", UPNP_ACTION_GET, "SortCaps", G_TYPE_STRING},
      },
     {}
    },
    {"systemUpdateId",
     "SystemUpdateID", G_TYPE_UINT, true,
     {{"GetSystemUpdateID", UPNP_ACTION_GET, "Id", G_TYPE_UINT},
      },
     {}
    },
    {"resetToken",
     "ServiceResetToken", G_TYPE_STRING, false,
     {{"GetServiceResetToken", UPNP_ACTION_GET, "ResetToken", G_TYPE_STRING},
      },
     {}
    },
    {"featureList",
     "FeatureList", G_TYPE_STRING, false,
     {{"GetFeatureList", UPNP_ACTION_GET, "FeatureList", G_TYPE_STRING},
      },
     {}
    }
};

// TODO Implement Browse and optional actions as necessary (eg Search)

bool UpnpContentDirectory::getAttributesRequest(UpnpRequest *request)
{
    bool status = false;

    for (auto it = m_attributeMap.begin(); it != m_attributeMap.end(); ++it)
    {
        DEBUG_PRINT(" \"" << it->first << "\"");
        // Check the request
        if (!UpnpAttribute::isValidRequest(&m_attributeMap, it->first, UPNP_ACTION_GET))
        {
            request->done++;
            continue;
        }

        UpnpAttributeInfo *attrInfo = it->second.first;
        bool result = UpnpAttribute::get(m_proxy, request, attrInfo);

        status |= result;
        if (!result)
        {
            request->done++;
        }
    }
    return status;
}

bool UpnpContentDirectory::setAttributesRequest(const RCSResourceAttributes &value,
        UpnpRequest *request)
{
    bool status = false;

    for (auto it = value.begin(); it != value.end(); ++it)
    {
        const std::string attrName = it->key();

        DEBUG_PRINT(" \"" << attrName << "\"");

        // Check the request
        if (!UpnpAttribute::isValidRequest(&m_attributeMap, attrName, UPNP_ACTION_POST))
        {
            request->done++;
            continue;
        }
        RCSResourceAttributes::Value attrValue = it->value();

        UpnpAttributeInfo *attrInfo = m_attributeMap[attrName].first;
        bool result = UpnpAttribute::set(m_proxy, request, attrInfo, &attrValue);

        status |= result;
        if (!result)
        {
            request->done++;
        }
    }

    return status;
}
