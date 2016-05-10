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
#include "MenuIotivity.h"
#include "MenuServices.h"

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

MenuMain::MenuMain()
{
    m_quit = false;
    print();
}

MenuMain::~MenuMain() {}

std::string MenuMain::getName()
{
    return "Main";
}

void MenuMain::print()
{
    std::cout << "------------------------------------" << std::endl;
    std::cout << getName() << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "1) Devices" << std::endl;
    std::cout << "2) Services" << std::endl;
    std::cout << "3) IoTivity" << std::endl;
    std::cout << "h) Help" << std::endl;
    std::cout << "q) Exit" << std::endl;
}

void MenuMain::help() {
    print();
}
void MenuMain::run(const std::vector<std::string>& cmd, std::stack<std::unique_ptr<MenuBase>>& menuStack)
{
    if(cmd.size() > 0) {
        if("1" == cmd[0]) {
            std::cout << "Devices Menu not yet Implmented" << std::endl;
           // menuStack.push(unique_ptr<CommandMenu>(new(DevicesMenu)));
        } else if("2" == cmd[0]) {
            menuStack.push(std::unique_ptr<MenuBase>(new(MenuServices)));
        } else if("3" == cmd[0]) {
            menuStack.push(std::unique_ptr<MenuBase>(new(MenuIotivity)));
        } else if("q" == cmd[0]) {
            m_quit = true;
        } else if("h" == cmd[0]) {
            help();
        }
    }
}

bool MenuMain::quit() {
    return m_quit;
}
#endif /* MENUMAIN_H_ */
