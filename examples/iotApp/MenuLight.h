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

#ifndef MenuLight_H_
#define MenuLight_H_

#include <OCApi.h>
#include <OCPlatform.h>

#include <functional>
#include <mutex>
#include <set>
#include <vector>

#include <UpnpConstants.h>

#include "IotivityUtility.h"
#include "Light.h"
#include "MenuBase.h"

class MenuLight: public MenuBase
{
    public:
        MenuLight();
        virtual ~MenuLight();
        /*
         * The init member function is optional.
         * Used to pass in an already discovered light resources before calling run.
         */
        void init(std::set<std::shared_ptr<OC::OCResource>, OCResourceComp> lightSet);
        virtual std::string getName();
        virtual void help();
        virtual void print();
        virtual void run(const std::vector<std::string> &cmd,
                         std::stack<std::unique_ptr<MenuBase>> &menuStack);
        virtual bool quit();
    private:
        void onFindResource(std::shared_ptr< OC::OCResource > resource);

        std::set<std::shared_ptr<OC::OCResource>, OCResourceComp> m_lightSet;
        std::vector<std::shared_ptr<OC::OCResource> > m_lightVector;
        std::vector<Light> m_light_instance_vector;
        bool m_quit;
        std::mutex m_mutex;

        OC::FindCallback onFindResourceCb;
};

#endif /* MenuLight_H_ */
