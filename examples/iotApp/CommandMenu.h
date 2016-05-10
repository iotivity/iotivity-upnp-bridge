/*
 * CommandMenu.h
 *
 *  Created on: Mar 9, 2016
 *      Author: georgena
 */

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
