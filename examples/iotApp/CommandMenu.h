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

#ifndef COMMANDMENU_H_
#define COMMANDMENU_H_

#include <vector>
#include <string>
#include <stack>
#include <csignal>
#include <memory>

class CommandMenu
{
public:
    CommandMenu();
    virtual ~CommandMenu();
    virtual std::string getName() = 0;
    // The menu specific help message
    virtual void help() = 0;
    // print the actual commandline menu.
    virtual void print() = 0;
    // Method responsible for parsing the commands entered on the commandline.
    virtual void run(const std::vector<std::string>& cmd, std::stack<std::unique_ptr<CommandMenu>>& menuStack) = 0;
    //returns true if the user has asked to quite the program
    virtual bool quit() = 0;
//private:
    // can be used to exit program from any location
    static volatile std::sig_atomic_t interrupt;
};

CommandMenu::CommandMenu() {}
CommandMenu::~CommandMenu() {}

#endif /* COMMANDMENU_H_ */
