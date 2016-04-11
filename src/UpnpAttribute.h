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

#ifndef UPNP_ATTRIBUTE_H_
#define UPNP_ATTRIBUTE_H_

#include <gupnp.h>

#include "UpnpInternal.h"

class UpnpAttribute
{
public:

    std::string name;
    std::string stateVar;
    GType type;
    int flags;
    std::vector < std::pair <std::string, UpnpActionType>> actions;

    UpnpAttribute(std::string name,
                  std::vector <std::pair <std::string,UpnpActionType>> actions,
                  std::string varName, GType type)
    {
        this->name = name;
        this->actions = actions;
        this->stateVar = varName;
        this->type = type;
        this->flags = 0;
    }

    UpnpAttribute(std::string name,
                  std::vector <std::pair <std::string, UpnpActionType> >actions)
    {
        this->name = name;
        this->actions = actions;
        this->stateVar = "";
        this->type = G_TYPE_NONE;
        this->flags = 0;
    }

    static std::vector <UpnpAttribute> * getServiceAttributeInfo(std::string resourceType);

    static UpnpAttribute * getAttributeInfo(std::vector <UpnpAttribute> *serviceAttributes,
                                            std::string attrName);

    static bool isValidRequest(std::map <std::string, UpnpAttribute*> *attrMap,
                               std::string,
                               UpnpActionType actionType);
};

#endif
