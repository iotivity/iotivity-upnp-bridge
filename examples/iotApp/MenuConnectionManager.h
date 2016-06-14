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

#ifndef MENUCONNECTIONMANAGER_H_
#define MENUCONNECTIONMANAGER_H_

#include <OCApi.h>
#include <OCPlatform.h>

#include <set>
#include <vector>

#include "IotivityUtility.h"
#include "MenuBase.h"

class MenuConnectionManager: public MenuBase
{
public:
    MenuConnectionManager();
    virtual ~MenuConnectionManager();
     /*
      * Used to pass in an already discovered list of brightness resources before calling run.
      * this is not required to use this class but can give it an initial start state
      */
     void init(std::set<OC::OCResource::Ptr, OCResourceComp> brightnessSet);
     virtual std::string getName();
     virtual void help();
     virtual void print();
     virtual void run(const std::vector<std::string> &cmd,
                      std::stack<std::unique_ptr<MenuBase>> &menuStack);
     virtual bool quit();
 private:
     void onFindResource(OC::OCResource::Ptr);

     std::set<OC::OCResource::Ptr, OCResourceComp> m_connectionManagerSet;
     std::vector<OC::OCResource::Ptr> m_connectionManagerVector;
     bool m_quit;
     std::mutex m_mutex;

     OC::FindCallback onFindResourceCb;
};

#endif /* MENUCONNECTIONMANAGER_H_ */
