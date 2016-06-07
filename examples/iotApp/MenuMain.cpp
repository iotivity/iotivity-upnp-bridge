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

#include "MenuMain.h"

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
    std::cout << "d) Devices" << std::endl;
    std::cout << "s) Services" << std::endl;
    std::cout << "i) IoTivity" << std::endl;
    std::cout << "h) Help" << std::endl;
    std::cout << "q) Quit" << std::endl;
}

void MenuMain::help() {
    print();
}
void MenuMain::run(const std::vector<std::string>& cmd, std::stack<std::unique_ptr<MenuBase>>& menuStack)
{
    if(cmd.size() > 0) {
        if("d" == cmd[0]) {
            menuStack.push(std::unique_ptr<MenuBase>(new MenuDevices));
        } else if("s" == cmd[0]) {
            menuStack.push(std::unique_ptr<MenuBase>(new MenuServices));
        } else if("i" == cmd[0]) {
            menuStack.push(std::unique_ptr<MenuBase>(new MenuIotivity));
        } else if("q" == cmd[0]) {
            m_quit = true;
        } else if("h" == cmd[0]) {
            help();
        } else {
            std::cout << "unknown command." << std::endl;
            print();
        }
    }
}

bool MenuMain::quit() {
    return m_quit;
}
