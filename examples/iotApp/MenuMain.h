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

#ifndef MENUMAIN_H_
#define MENUMAIN_H_

#include <memory>

#include "MenuBase.h"
#include "MenuDevices.h"
#include "MenuServices.h"
#include "MenuIotivity.h"

class MenuMain: public MenuBase
{
public:
    MenuMain();
    virtual ~MenuMain();
    virtual std::string getName();
    virtual void help();
    virtual void print();
    virtual void run(const std::vector<std::string>& cmd, std::stack<std::unique_ptr<MenuBase>>& menuStack);
    virtual bool quit();
private:
    bool m_quit;
};

#endif /* MENUMAIN_H_ */
