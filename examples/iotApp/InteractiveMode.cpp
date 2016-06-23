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

#include "InteractiveMode.h"
#include "MenuMain.h"
#include <string>

using namespace std;

InteractiveMode::InteractiveMode()
{
    menuStack.push(unique_ptr<MenuBase>(new MenuMain));
}
void InteractiveMode::run()
{
    cout << "Enter command or type 'h' for help" << endl;
    string cmd;
    do
    {
        cout << menuStack.top()->getName() << "> ";

        getline(cin, cmd);
        vector<string> cmds = parseCmd(cmd);
        menuStack.top()->run(cmds, menuStack);
    }
    while (!menuStack.top()->quit());
    cout << "Exiting please wait..." << endl;
}

vector<string> InteractiveMode::parseCmd(const string &cmd)
{
    vector<string> token;
    size_t lpos = 0; //left position in the string
    size_t rpos = 0; //right position in the string
    rpos = cmd.find_first_of(" \"'");
    while (rpos != string::npos)
    {
        if (cmd[rpos] == '"')
        {
            //quote was found with no space before it i.e. token1"token2"
            if (rpos - lpos > 0)
            {
                token.push_back(cmd.substr(lpos, rpos - lpos));
            }
            lpos = rpos + 1;
            rpos = cmd.find_first_of("\"", lpos);
            if (rpos != string::npos)
            {
                token.push_back(cmd.substr(lpos, rpos - lpos));
            }
        }
        else if (cmd[rpos] == '\'')
        {
            //quote was found with no space before it i.e. token1'token2'
            if (rpos - lpos > 0)
            {
                token.push_back(cmd.substr(lpos, rpos - lpos));
            }
            lpos = rpos + 1;
            rpos = cmd.find_first_of("'", lpos);
            if (rpos != string::npos)
            {
                token.push_back(cmd.substr(lpos, rpos - lpos));
            }
        }
        else
        {
            //don't interpret multiple spaces as a tokens
            if (rpos - lpos > 0)
            {
                token.push_back(cmd.substr(lpos, rpos - lpos));
            }
        }
        //only true if mismatched quotes are given i.e. token1 "token2
        if (rpos != string::npos)
        {
            lpos = rpos + 1;
            rpos = cmd.find_first_of(" \"'", lpos);
        }
    }
    //don't interpret a closing quote as a token
    if (cmd.size() - lpos > 0)
    {
        token.push_back(cmd.substr(lpos, cmd.size() - lpos));
    }
    return token;
}
