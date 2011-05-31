/**
 * @file ./DBManager.cpp
 * @author 
 */

// Includes /////////////////////////////////////////////////////// Includes //
#include "DBManager.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include "PasswordManager.h"

//using namespace std;
DBManager::DBManager(const std::string& dbfile) :
	dbfile(dbfile) {
	encryptor = NULL;
	parseError = false;
}

DBManager::ERROR DBManager::initialize(const std::string& password) {
	ERROR rtn = UNKNOWN_ERROR;
	try {
		std::ifstream file(dbfile.c_str(), std::ios::in);
		if (!file) {
			rtn = createDBFile(password, dbfile);
		} else {
			rtn = readDBFile(password, file);
			file.close();
		}
	} catch (CryptoPP::Exception& e) {
		rtn = WRONG_PASSWORD;
		std::cerr << "Encryption error !" << std::endl;
		std::cerr << e.GetWhat() << std::endl;
	}
	return rtn;
}
DBManager::ERROR DBManager::createDBFile(const std::string& password, const std::string& dbfile) {
	std::cerr << "File not found! Creating a new one." << std::endl;
	std::ofstream file(dbfile.c_str(), std::ios::out);
	if (!file) {
		std::cerr << "Cannot open the current dbfile for writing. Modifications are discarded." << std::endl;
		return WRONG_DB_FILE;
	}
	salt = PasswordManager::generatePassword("", 50);
	encryptor = new Encryptor(password, salt);
	challenge = encryptor->encrypt(PasswordManager::generatePassword("", 50));
	file << salt << "\n";
	file << challenge << "\n";
	file.close();
	return OK;
}

DBManager::ERROR DBManager::readDBFile(const std::string& password, std::ifstream& file) {
	// Get the salt
	getline(file, salt);
	if (salt.empty()) {
		parseError = true;
		std::cerr << "Wrong db file. Salt not found." << std::endl;
		return WRONG_DB_FILE; // Salt not found is fatal.
	}

	// We have the salt we can create an ecryptor
	encryptor = new Encryptor(password, salt);

	// Try to decrypt the simple challenge string
	getline(file, challenge);
	if (challenge.empty()) {
		parseError = true;
		std::cerr << "Wrong db file. Challenge string not found." << std::endl;
		return WRONG_DB_FILE; // Challenge not found is fatal.
	}
	try {
		encryptor->decrypt(challenge);
	} catch (CryptoPP::Exception& e) {
		parseError = true;
		std::cerr << "Error: Are you sure the password is right? " << std::endl;
		return WRONG_PASSWORD; // Challenge failed is fatal.
	}

	// Perform the read...
	std::string line;
	while (getline(file, line)) {
		std::size_t firstSpace = line.find_first_of(" ");
		if (firstSpace <= 1 || firstSpace >= line.length() - 1) {
			parseError = true;
			std::cerr << "Wrong entry. Space not found or misplaced." << std::endl;
		}
		std::string service = line.substr(0, firstSpace);
		std::string key = line.substr(firstSpace + 1, line.length());
		values.insert(std::make_pair(encryptor->decrypt(service), key));
	}
	return OK;
}

DBManager::~DBManager() {
	if (encryptor != NULL) {
		delete encryptor;
	}
}

std::string DBManager::getKey(const std::string& serviceName) {
	if (encryptor == NULL) {
		std::cerr << "Get key ignored." << serviceName << std::endl;
		return "";
	}
	try {
		DB::const_iterator findIt = values.find(serviceName);
		if (findIt != values.end()) {
			return encryptor->decrypt(findIt->second);
		}
	} catch (CryptoPP::Exception& e) {
		std::cerr << e.what() << std::endl;
	}
	std::cerr << "Service not found: " << serviceName << std::endl;
	return "";
}

std::vector<std::string> DBManager::getServiceNames() {
	std::vector<std::string> vals;
	for (DB::const_iterator it = values.begin(); it != values.end(); ++it) {
		vals.push_back(it->first);
	}
	return vals;
}

bool DBManager::addKey(const std::string& serviceName, const std::string& key) {
	if (encryptor == NULL) {
		std::cerr << "Add key ignored: " << serviceName << std::endl;
		return false;
	}
	if (parseError) {
		std::cerr << "Due to parse error, we are read only. Nothing to do..." << std::endl;
		return false;
	}
	try {
		DB::iterator findIt = values.find(serviceName);
		if (findIt != values.end()) {
			values.erase(findIt);
		}
		values.insert(std::make_pair(serviceName, encryptor->encrypt(key)));
	} catch (CryptoPP::Exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}


bool DBManager::removeKey(const std::string& serviceName) {
	if (encryptor == NULL) {
		std::cerr << "Remove key ignored: " << serviceName << std::endl;
		return false;
	}
	if (parseError) {
		std::cerr << "Due to parse error, we are read only. Nothing to do..." << std::endl;
		return false;
	}
	try {
		DB::iterator findIt = values.find(serviceName);
		if (findIt != values.end()) {
			values.erase(findIt);
			return true;
		}
	} catch (CryptoPP::Exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return false;
}

bool DBManager::applyChanges() {
	if (parseError) {
		std::cerr << "Due to parse error, we are read only. Nothing to do..." << std::endl;
		return false;
	}
	if (std::remove(dbfile.c_str()) != 0) {
		std::cerr << "Cannot remove the current dbfile to create a new one. Modifications are discarded." << std::endl;
		return false;
	}
	std::ofstream file(dbfile.c_str(), std::ios::out);
	if (!file) {
		std::cerr << "Cannot open the current dbfile for writing. Modifications are discarded." << std::endl;
		return false;
	}

	file << salt << "\n";
	file << challenge << "\n";
	for (DB::const_iterator it = values.begin(); it != values.end(); ++it) {
		try {
			file << encryptor->encrypt(it->first) << " " << it->second << std::endl;
		} catch (CryptoPP::Exception& e) {
			std::cerr << "Error when saving: " << it->first << std::endl;
			std::cerr << e.what() << std::endl;
		}
	}
	file.close();
	return true;
}
//END Of file : ./DBManager.cpp
