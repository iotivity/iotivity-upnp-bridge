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

#ifndef MENUCONTENTDIRECTORY_H_
#define MENUCONTENTDIRECTORY_H_

#include <OCApi.h>
#include <OCPlatform.h>

#include <set>
#include <vector>

#include "IotivityUtility.h"
#include "MenuBase.h"

class MenuContentDirectory: public MenuBase
{
public:
    MenuContentDirectory();
    virtual ~MenuContentDirectory();
    /*
     * Used to pass in an already discovered list of contentDirectory resources before calling run.
     * this is not required to use this class but can give it an initial start state
     */
    void init(std::set<OC::OCResource::Ptr, OCResourceComp> contentDirectorySet);
    virtual std::string getName();
    virtual void help();
    virtual void print();
    virtual void run(const std::vector<std::string> &cmd,
                     std::stack<std::unique_ptr<MenuBase>> &menuStack);
    virtual bool quit();
private:
    void onFindResource(OC::OCResource::Ptr);

    std::set<OC::OCResource::Ptr, OCResourceComp> m_contentDirectorySet;
    std::vector<OC::OCResource::Ptr> m_contentDirectoryVector;
    bool m_quit;
    std::mutex m_mutex;

    OC::FindCallback onFindResourceCb;
};

#endif /* MENUCONTENTDIRECTORY_H_ */
