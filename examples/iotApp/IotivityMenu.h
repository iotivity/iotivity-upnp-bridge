/*
 * IotivityMenu.h
 *
 *  Created on: May 11, 2016
 *      Author: georgena
 */

#ifndef IOTIVITYMENU_H_
#define IOTIVITYMENU_H_

#include <OCApi.h>
#include <OCPlatform.h>

#include <functional>
#include <mutex>

#include "CommandMenu.h"
#include "IotivityUtility.h"

class IotivityMenu: public CommandMenu
{
public:
    IotivityMenu();
    virtual ~IotivityMenu();
    virtual std::string getName();
    virtual void help();
    virtual void print();
    virtual void run(const std::vector<std::string>& cmd, std::stack<std::unique_ptr<CommandMenu>>& menuStack);
    virtual bool quit();
private:
    void onFindResource(std::shared_ptr< OC::OCResource > resource);
    void onResponse(const OC::HeaderOptions&, const OC::OCRepresentation& rep, const int eCode);
    void onObserve(const OC::HeaderOptions&, const OC::OCRepresentation& rep, const int eCode, const int sequenceNumber);

    std::vector< std::shared_ptr< OC::OCResource > > g_resources;
    bool m_quit;
    std::mutex m_mutex;

    OC::FindCallback onFindResourceCb;
    OC::GetCallback onResponceCb;
    OC::ObserveCallback onObserveCb;
};

IotivityMenu::IotivityMenu():
        g_resources(), m_quit(false)
{
    onFindResourceCb = std::bind(&IotivityMenu::onFindResource, this, std::placeholders::_1);
    onResponceCb = std::bind(&IotivityMenu::onResponse, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    onObserveCb = std::bind(&IotivityMenu::onObserve, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    print();
}

IotivityMenu::~IotivityMenu() {}

std::string IotivityMenu::getName()
{
    return "Iotivity";
}

void IotivityMenu::print()
{
    std::cout << "------------------------------------" << std::endl;
    std::cout << getName() << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "find) find <requestURI>" << std::endl;
    std::cout << "list) list all found resources" << std::endl;
    std::cout << "get) get <#> call resource get on the specified resource" << std::endl;
    std::cout << "post) post <#> <name> <type> <value>" << std::endl;
    std::cout << "observe) observe <#>" << std::endl;
    std::cout << "clear) clear all found resources" << std::endl;
    std::cout << "b) Back" << std::endl;
    std::cout << "h) Help" << std::endl;
    std::cout << "q) Exit" << std::endl;
}

void IotivityMenu::help() {
    print();
}
void IotivityMenu::run(const std::vector<std::string>& cmd, std::stack<std::unique_ptr<CommandMenu>>& menuStack)
{
    if(cmd.size() > 0) {
        if (cmd[0] == "find") {
            if(cmd.size() > 1) {
                std::string requestURI = cmd[1];
                OCStackResult result = OC::OCPlatform::findResource("", requestURI, CT_DEFAULT,
                        onFindResourceCb);
                std::cout << "findResource(" + requestURI + ") - " << result << std::endl;
            }
        } else if (cmd[0] == "list") {
            for (size_t i = 0; i < g_resources.size(); ++i) {
                std::cout << "[" << i << "]" << std::endl;
                printResourceCompact(g_resources[i]);
            }
        } else if (cmd[0] == "get") {
            size_t i = std::stoi(cmd[1]);
            OC::QueryParamsMap queryParams;
            OCStackResult result = g_resources[i]->get(queryParams, onResponceCb);
            std::cout << "get " << g_resources[i]->host() << g_resources[i]->uri() << " - " << result << std::endl;
        } else if (cmd[0] == "put") {
            size_t i = std::stoi(cmd[1]);
            OC::OCRepresentation rep;
            // TODO parse rep, as below
            OC::QueryParamsMap queryParams;
            OCStackResult result = g_resources[i]->put(rep, queryParams, onResponceCb);
            std::cout << "put " << g_resources[i]->host() << g_resources[i]->uri() << " - " << result << std::endl;
        } else if (cmd[0] == "post") {
            size_t i = std::stoi(cmd[1]);
            OC::OCRepresentation rep;
            size_t k = 2;
            while (k < cmd.size()) {
                std::string name = cmd[k++];
                std::string type = cmd[k++];
                if (type == "int") {
                    int value = std::stoi(cmd[k++]);
                    rep.setValue(name, value);
                } else if (type == "double") {
                    double value = std::stod(cmd[k++]);
                    rep.setValue(name, value);
                } else if (type == "bool") {
                    bool value = (cmd[k++] == "false") ? false : true;
                    rep.setValue(name, value);
                } else if (type == "string") {
                    std::string value = cmd[k++];
                    rep.setValue(name, value);
                }
            }
            OC::QueryParamsMap queryParams;
            OCStackResult result = g_resources[i]->post(rep, queryParams, onResponceCb);
            std::cout << "post " << g_resources[i]->host() << g_resources[i]->uri() << " - " << result << std::endl;
        } else if (cmd[0] == "observe") {
            size_t i = std::stoi(cmd[1]);
            OC::QueryParamsMap queryParams;
            OCStackResult result = g_resources[i]->observe(OC::ObserveType::Observe, queryParams, onObserveCb);
            std::cout << "observe " << g_resources[i]->host() << g_resources[i]->uri() << " - " << result << std::endl;
//        } else if (cmd == "delete") {
//            // TODO
        } else if ("clear" == cmd[0]) {
            g_resources.clear();
        } else if("q" == cmd[0]) {
            m_quit = true;
        } else if("b" == cmd[0]) {
            if(menuStack.size() <= 1)
            {
                print();
                return; //do nothing
            } else {
                menuStack.pop();
                menuStack.top()->print();
            }
        } else if("h" == cmd[0]) {
            help();
        }
    }
}

bool IotivityMenu::quit() {
    return m_quit;
}

void IotivityMenu::onFindResource(std::shared_ptr< OC::OCResource > resource)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "Found resource" << std::endl;
    g_resources.push_back(resource);
    printResourceCompact(resource);
}

void IotivityMenu::onResponse(const OC::HeaderOptions&, const OC::OCRepresentation& rep, const int eCode)
{
    std::cout << "\teCode " << eCode << std::endl;
    if (eCode == OC_STACK_OK) {
        std::cout << "\t" << rep.getHost() << rep.getUri() << std::endl;
        // TODO getPayload()
        // TODO getChildren()
        if (!rep.getResourceTypes().empty()) {
            std::cout << "\tresourceTypes ";
            for(auto rt : rep.getResourceTypes()) {
                std::cout << rt << " ";
            }
            std::cout << std::endl;
        }
        if (!rep.getResourceInterfaces().empty()) {
            std::cout << "\tresourceInterfaces ";
            for (auto ri : rep.getResourceInterfaces()) {
                std::cout << ri << " ";
            }
            std::cout << std::endl;
        }
        for (OC::OCRepresentation::const_iterator it = rep.begin(); it != rep.end(); ++it) {
            std::cout << "\t" << it->attrname() << " " << it->getValueToString() << std::endl;
            // TODO type, base_type, depth
        }
    }
}

void IotivityMenu::onObserve(const OC::HeaderOptions&, const OC::OCRepresentation& rep, const int eCode, const int sequenceNumber)
{
    std::cout << "observe" << std::endl;
    std::cout << "\teCode=" << eCode << std::endl;
    if (eCode == OC_STACK_OK) {
        std::cout << "\tsequencenumber " << sequenceNumber << std::endl
                << "\t" << rep.getHost() << rep.getUri() << std::endl;
        // TODO getPayload()
        // TODO getChildren()
        if (!rep.getResourceTypes().empty()) {
            std::cout << "\tresourceTypes ";
            for(auto rt : rep.getResourceTypes()) {
                std::cout << rt << " ";
            }
            std::cout << std::endl;
        }
        if (!rep.getResourceInterfaces().empty()) {
            std::cout << "\tresourceInterfaces ";
            for (auto ri : rep.getResourceInterfaces()) {
                std::cout << ri << " ";
            }
            std::cout << std::endl;
        }
        for (OC::OCRepresentation::const_iterator it = rep.begin(); it != rep.end(); ++it) {
            std::cout << "\t" << it->attrname() << " " << it->getValueToString() << std::endl;
            // TODO type, base_type, depth
        }
    }
}
#endif /* IOTIVITYMENU_H_ */
