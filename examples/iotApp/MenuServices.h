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

#ifndef MENUSERVICES_H_
#define MENUSERVICES_H_

#include <OCApi.h>
#include <OCPlatform.h>

#include <functional>
#include <mutex>
#include <set>
#include <vector>

#include "IotivityUtility.h"
#include "MenuBase.h"

class MenuServices: public MenuBase
{
    public:
        MenuServices();
        virtual ~MenuServices();
        virtual std::string getName();
        virtual void help();
        virtual void print();
        virtual void run(const std::vector<std::string> &cmd,
                         std::stack<std::unique_ptr<MenuBase>> &menuStack);
        virtual bool quit();
    private:
        void onFindResource(OC::OCResource::Ptr resource);

        std::set<OC::OCResource::Ptr, OCResourceComp> m_avTransport;
        std::set<OC::OCResource::Ptr, OCResourceComp> m_binarySwitch;
        std::set<OC::OCResource::Ptr, OCResourceComp> m_brightness;
        std::set<OC::OCResource::Ptr, OCResourceComp> m_connectionManager;
        std::set<OC::OCResource::Ptr, OCResourceComp> m_contentDirectory;
        std::set<OC::OCResource::Ptr, OCResourceComp> m_layer3Forwarding;
        std::set<OC::OCResource::Ptr, OCResourceComp> m_renderingControl;
        std::set<OC::OCResource::Ptr, OCResourceComp> m_scheduledRecording;
        std::set<OC::OCResource::Ptr, OCResourceComp> m_wanIfConfig;
        std::set<OC::OCResource::Ptr, OCResourceComp> m_wanCableLinkConfig;
        std::set<OC::OCResource::Ptr, OCResourceComp> m_wanDSLLinkConfig;
        std::set<OC::OCResource::Ptr, OCResourceComp> m_wanEthernetConfig;
        std::set<OC::OCResource::Ptr, OCResourceComp> m_wanPotsLinkConfig;
        std::set<OC::OCResource::Ptr, OCResourceComp> m_wanIPConnection;
        std::set<OC::OCResource::Ptr, OCResourceComp> m_lanHostConfig;



        bool m_quit;
        std::mutex m_mutex;

        OC::FindCallback onFindResourceCb;
};

#endif /* MENUSERVICES_H_ */
