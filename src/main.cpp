#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/static_assert.hpp>

#include "DBManager.h"
#include "PasswordManager.h"

namespace po = boost::program_options;

int main(int argc, char **argv) {
	// Declare a group of options that will be allowed only on command line
	po::options_description optionsGeneric("Generic options");
	optionsGeneric.add_options() //
	("version,v", "print version string") //
	("help,h", "produce help message");

	po::options_description optionsConfigure("Configuration options");
	optionsConfigure.add_options()//
	("file,f", po::value<std::string>()->default_value("./keysdb.aes"), "Specify a file containing the encrypted password.")//
	("hidden,d", po::value<bool>()->default_value(true), "Hide the password when typing them.")//
	("passwd,p", po::value<std::string>(), "Create a new file with a different password (use the --newpw to specify the new password).")//
	("newpw,P", po::value<std::string>(), "Choose the new password (use the --passwd switch to specify the output file).");


	po::options_description optionsGet("Get Keys");
	optionsGet.add_options()//
	("list,l", "List all the services on which password has been saved.")//
	("get,g", po::value<std::string>(), "Get a password for an existing service.");

	po::options_description optionsModify("Modify keys");
	optionsModify.add_options()//
	("add,a", po::value<std::string>(), "Add a service. (use the --key switch to input the key, or a random one will be generated).")//
	("key,k", po::value<std::string>()->implicit_value(""), "Add a key on a specified service (use the --add switch to specify the service).")//
	("remove,D", po::value<std::string>(), "Remove a service.")//
	("rename,m", po::value<std::string>(), "Rename a service. (use the --name choose the new name).")//
	("name,n", po::value<std::string>()->implicit_value(""), "New name for a specified service (use the --rename switch to specify the service).");

    po::positional_options_description pd;
    pd.add("get", 1);
	po::options_description cmdline_options;
	cmdline_options.add(optionsGeneric).add(optionsConfigure).add(optionsGet).add(optionsModify);
	po::options_description visible("Allowed options");
	visible.add(optionsGeneric).add(optionsConfigure).add(optionsGet).add(optionsModify);

	po::variables_map options;
	try {
		// Parse command line
		po::parsed_options parsed = po::command_line_parser(argc, argv).options(visible).positional(pd).run();
		po::store(parsed, options);
		po::notify(options);
	} catch (std::logic_error & error) {
		std::cerr << error.what() << std::endl << visible << std::endl;
		return -1;
	}

	if (options.count("help")) {
		std::cout << visible << std::endl;
		return 0;
	}
	if (options.count("version")) {
		std::cout << "Version 0.9" << std::endl;
		return 0;
	}
	if (!(options.count("list") || options.count("get") || options.count("add") || options.count("key") || options.count("remove") || options.count("rename") || options.count("passwd"))) {
		std::cerr << "I am waiting for at least one option ..." << std::endl;
		std::cerr << visible << std::endl;
		return -1;
	}

	std::string filename = options["file"].as<std::string> ();

	DBManager manager(filename);
	std::string password;
	if (options["hidden"].as<bool> ()) {
		password = PasswordManager::askPasswordHidden("Encryption password: ");
	} else {
		password = PasswordManager::askPassword();
		password.size();
	}
	DBManager::ERROR error = manager.initialize(password);
	if (error != DBManager::OK) {
		std::cerr << "Error. exiting." << std::endl;
		return error;
	}

	if (options.count("list")) {
		std::vector<std::string> services = manager.getServiceNames();
		if (services.size() == 0) {
			std::cerr << "No service ..." << std::endl;
		}
		for (unsigned int i = 0; i < services.size(); ++i) {
			std::cout << services[i] << std::endl;
		}
		return 0;
	}
	if (options.count("get")) {
		std::cout << manager.getKey(options["get"].as<std::string> ()) << std::endl;
		return 0;
	}
	if (options.count("add") && options.count("key")) {
		std::string key = options["key"].as<std::string> ();
		if (key.empty()) {
			if (options["hidden"].as<bool> ()) {
				key = PasswordManager::askPasswordHidden("Password to store ? ");
			} else {
				key = PasswordManager::askPassword();
			}
		}
		if (!manager.addKey(options["add"].as<std::string> (), key)) {
			return -1;
		}
		return manager.applyChanges();
	}
	if (options.count("add")) {
		if (!manager.addKey(options["add"].as<std::string> (), PasswordManager::generatePassword("&@!", 12))) {
			return -1;
		}
		return manager.applyChanges();
	}
	if (options.count("remove")) {
		if (!manager.removeKey(options["remove"].as<std::string> ())) {
			return -1;
		}
		return manager.applyChanges();
	}
	if (options.count("rename") && options.count("name")) {
	    
        std::string key = manager.getKey(options["rename"].as<std::string> ());
	
		if (manager.addKey(options["name"].as<std::string> (), key) && manager.removeKey(options["rename"].as<std::string> ())) {
    		return manager.applyChanges();			
		}
	}
	
	if (options.count("passwd")) {
	    std::string newFilename = options["passwd"].as<std::string> ();
	

	    
        std::string newPassword = options["newpw"].as<std::string> ();

	    DBManager newManager(newFilename);	    
	    DBManager::ERROR error = newManager.initialize(newPassword);
	    if (error != DBManager::OK) {
		    std::cerr << "Error. exiting." << std::endl;
		    return error;
	    }
	    
	    std::vector<std::string> services = manager.getServiceNames();

		for (unsigned int i = 0; i < services.size(); ++i) {
			newManager.addKey(services[i], manager.getKey(services[i]));
		}
	    
	    return newManager.applyChanges();
	
	}

	std::cerr << visible << std::endl;
	return 1;
}
