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

#ifndef MAINMENU_H_
#define MAINMENU_H_

#include <memory>

#include "CommandMenu.h"
#include "IotivityMenu.h"

class MainMenu: public CommandMenu
{
public:
    MainMenu();
    virtual ~MainMenu();
    virtual std::string getName();
    virtual void help();
    virtual void print();
    virtual void run(const std::vector<std::string>& cmd, std::stack<std::unique_ptr<CommandMenu>>& menuStack);
    virtual bool quit();
private:
    bool m_quit;
};

MainMenu::MainMenu()
{
    m_quit = false;
    print();
}

MainMenu::~MainMenu() {}

std::string MainMenu::getName()
{
    return "Main";
}

void MainMenu::print()
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

void MainMenu::help() {
    print();
}
void MainMenu::run(const std::vector<std::string>& cmd, std::stack<std::unique_ptr<CommandMenu>>& menuStack)
{
    if(cmd.size() > 0) {
        if("1" == cmd[0]) {
            std::cout << "Devices Menu not yet Implmented" << std::endl;
           // menuStack.push(unique_ptr<CommandMenu>(new(DevicesMenu)));
        } else if("2" == cmd[0]) {
            std::cout << "Services Menu not yet Implmented" << std::endl;
            //menuStack.push(unique_ptr<CommandMenu>(new(ServicesMenu)));
        } else if("3" == cmd[0]) {
            menuStack.push(std::unique_ptr<CommandMenu>(new(IotivityMenu)));
        } else if("q" == cmd[0]) {
            m_quit = true;
        } else if("h" == cmd[0]) {
            help();
        }
    }
}

bool MainMenu::quit() {
    return m_quit;
}
#endif /* MAINMENU_H_ */
