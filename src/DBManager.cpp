/**
 * @file ./DBManager.cpp
 * @author 
 */

// Includes /////////////////////////////////////////////////////// Includes //
#include "DBManager.h"
#include <fstream>
#include <iostream>
#include <stdio.h>

//using namespace std;
DBManager::DBManager(const std::string& password, const std::string& dbfile) :
	dbfile(dbfile) {
	encryptor = NULL;
	parseError = false;

	try {
		std::ifstream file(dbfile.c_str(), std::ios::in);
		if (file) {
			getline(file, salt);
			if (salt.empty()) {
				parseError = true;
				std::cerr << "Wrong db file. Salt not found." << std::endl;
			} else {
				encryptor = new Encryptor(password, salt);
				std::string line;
				while (getline(file, line)) {
					std::size_t firstSpace = line.find_first_of(" ");
					if (firstSpace <= 1 || firstSpace >= line.length() - 1) {
						parseError = true;
						std::cerr << "Wrong db file. Space not found or misplaced." << std::endl;
					}
					std::string service = line.substr(0, firstSpace);
					std::string key = line.substr(firstSpace + 1, line.length());
					values.insert(std::make_pair(encryptor->decrypt(service), key));
				}
				file.close();
			}
		} else {
			std::cout << "File not found! Creating a new one." << std::endl;
			std::ofstream file(dbfile.c_str(), std::ios::out);
			if (!file) {
				std::cerr << "Cannot open the current dbfile for writing. Modifications are discarded." << std::endl;
				return;
			}
			file << "plopsidbqisdcbhsdqchib" << "\n";
			file.close();
			encryptor = new Encryptor(password, salt);
		}
	} catch (CryptoPP::Exception& e) {
		parseError = true;
		std::cerr << "Cannot decrypt a service." << std::endl;
		std::cerr << e.GetWhat() << std::endl;
	}
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

void DBManager::addKey(const std::string& serviceName, const std::string& key) {
	if (encryptor == NULL) {
		std::cerr << "Add key ignored." << serviceName << std::endl;
		return;
	}
	if (parseError) {
		std::cerr << "Due to parse error, we are read only. Nothing to do..." << std::endl;
		return;
	}
	try {
		values.insert(std::make_pair(serviceName, encryptor->encrypt(key)));
	} catch (CryptoPP::Exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

void DBManager::applyChanges() {
	if (parseError) {
		std::cerr << "Due to parse error, we are read only. Nothing to do..." << std::endl;
		return;
	}
	if (std::remove(dbfile.c_str()) != 0) {
		std::cerr << "Cannot remove the current dbfile to create a new one. Modifications are discarded." << std::endl;
		return;
	}
	std::ofstream file(dbfile.c_str(), std::ios::out);
	if (!file) {
		std::cerr << "Cannot open the current dbfile for writing. Modifications are discarded." << std::endl;
		return;
	}

	file << salt << "\n";
	for (DB::const_iterator it = values.begin(); it != values.end(); ++it) {
		try {
			file << encryptor->encrypt(it->first) << " " << it->second << std::endl;
		} catch (CryptoPP::Exception& e) {
			std::cerr << "Error when saving: " << it->first << std::endl;
			std::cerr << e.what() << std::endl;
		}
	}
	file.close();
}

//END Of file : ./DBManager.cpp
