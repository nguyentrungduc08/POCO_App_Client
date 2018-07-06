/* 
 * File:   main.cpp
 * Author: Nguyen Trung Duc
 *
 * Created on July 6, 2018, 10:01 AM
 */
#include <bits/stdc++.h>
#include <cstdlib>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TTransportUtils.h>

#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/AbstractConfiguration.h"

#include "gen-cpp/UserStorage.h"


using namespace std;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace Task1;
using boost::make_shared;

using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Util::AbstractConfiguration;
using Poco::Util::OptionCallback;
using Poco::AutoPtr;

class Client_App : public Application {
public:

    Client_App() : _helpRequested(false) {
        std::cout << "constructor" << std::endl;
    }

    void initialize(Application& self) {
        std::cout << "initialize" << std::endl;
        loadConfiguration(); // load default configuration files, if present
        Application::initialize(self);
        // add your own initialization code here
    }

    void uninitialize() {
        std::cout << "uninitialize" << std::endl;
        // add your own uninitialization code here
        Application::uninitialize();
    }

    void reinitialize(Application& self) {
        std::cout << "reinitialize" << std::endl;
        Application::reinitialize(self);
        // add your own reinitialization code here
    }

    void defineOptions(OptionSet& options) {
        std::cout << "defineOptions" << std::endl;
        Application::defineOptions(options);

        options.addOption(
                Option("help", "h", "display help information on command line arguments")
                .required(false)
                .repeatable(false)
                .callback(OptionCallback<Client_App>(this, &Client_App::handleHelp)));

        options.addOption(
                Option("define", "D", "define a configuration property")
                .required(false)
                .repeatable(true)
                .argument("name=value")
                .callback(OptionCallback<Client_App>(this, &Client_App::handleDefine)));

        options.addOption(
                Option("config-file", "f", "load configuration data from a file")
                .required(false)
                .repeatable(true)
                .argument("file")
                .callback(OptionCallback<Client_App>(this, &Client_App::handleConfig)));

        options.addOption(
                Option("bind", "b", "bind option value to test.property")
                .required(false)
                .repeatable(false)
                .argument("value")
                .binding("test.property"));
    }

    void handleHelp(const std::string& name, const std::string& value) {
        std::cout << "handleHelp" << std::endl;
        _helpRequested = true;

        stopOptionsProcessing();
    }

    void handleDefine(const std::string& name, const std::string& value) {
        std::cout << "handleDefine" << std::endl;
    }

    void handleConfig(const std::string& name, const std::string& value) {
        std::cout << "handleConfig" << std::endl;
        loadConfiguration(value);
    }

    int main(const ArgVec& args) {
        std::cout << "begin main" << std::endl;
        if (!_helpRequested) {
            std::cout << "HELLO WORLD" << std::endl;
        }
        boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
        boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        UserStorageClient client(protocol);

        try {
            transport->open();
            do {
                this->printMenuOption();
                int cmd = 0;
                bool loop = true;
                std::cin >> cmd;
                switch (cmd) {
                    case 0:
                        loop = false;
                        break;
                    case 1:
                    {
                        //std::cout << createProfileUser(client) << std::endl;		 
                        UserProfile profile;
                        profile.uid = -1;
                        std::cout << "name : ";
                        std::cin >> profile.name;
                        std::cout << "age: ";
                        std::cin >> profile.age;
                        std::cout << "gender: ";
                        std::cin >> profile.gender;
                        int32_t res = client.createUser(profile);
                        std::cout << "Success. Uid form server : " << res << std::endl;
                    }
                        break;
                    case 2:
                    {
                        UserProfile profile;
                        int32_t uid;
                        std::cout << "uid: ";
                        std::cin >> uid;
                        client.getUser(profile, uid);
                        if (profile.uid == -1) {
                            std::cout << "invalid uid " << uid << "!!!" << std::endl;
                        } else {
                            std::cout << "profile User with id: " << uid << std::endl;
                            std::cout << "name : " << profile.name << std::endl;
                            std::cout << "age: " << profile.age << std::endl;
                            std::cout << "gender: " << profile.gender << std::endl;
                        }

                    }
                        break;
                    case 3:
                    {
                        UserProfile profile;
                        int32_t uid;
                        std::cout << "uid: ";
                        std::cin >> uid;
                        std::cout << "name: ";
                        std::cin >> profile.name;
                        std::cout << "age: ";
                        std::cin >> profile.age;
                        std::cout << "gender: ";
                        std::cin >> profile.gender;

                        int32_t res = client.editUser(uid, profile);
                        if (res == -1) {
                            std::cout << "invalid uid " << uid << "!!!" << std::endl;
                        } else {
                            std::cout << "Update success profile id " << uid << std::endl;
                        }

                    }
                        break;

                    default:
                        std::cout << "try again" << std::endl;
                        break;
                }

                if (!loop)
                    break;

            } while (1);



            //int32_t uid = 8;
            //client.editUser(uid);

            std::cout << "DONE" << std::endl;
            transport->close();

        } catch (TException &tx) {
            std::cout << "Error: " << tx.what() << std::endl;
        }

        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;

    void
    printMenuOption() {
        std::cout << "........................" << std::endl;
        std::cout << "Choose option: " << std::endl;
        std::cout << "1. create new user " << std::endl;
        std::cout << "2. get user " << std::endl;
        std::cout << "3. edit user " << std::endl;
        std::cout << "0. exit! " << std::endl;
        return;
    }
};


POCO_APP_MAIN(Client_App)
