/**
 * @file ./PasswordManager.cpp
 * @author 
 */

// Includes /////////////////////////////////////////////////////// Includes //
#include "PasswordManager.h"

#include "crypto++/osrng.h"
using CryptoPP::AutoSeededRandomPool;

#ifdef HAVE_UNISTD_H
#include "unistd.h"
#elif HAVE_CONIO_H
#include "conio.h"
#endif
#include <iostream>

std::string PasswordManager::askPassword(const std::string& message) {
	std::string password;
#ifdef HAVE_UNISTD_H
	const char* pass = getpass(message.c_str());
	password = pass;
#elif HAVE_CONIO_H
	char input = getch();
	while (input != '\r')
	{
		std::cout << "*";
		password += input;
		input = getch();
	}
	std::cout << std::endl;
#else
	// Cannot compile if there is no unistd.h nor conio.h
	BOOST_STATIC_ASSERT(false);
#endif
	std::string error = checkPasswordConstraints(password);
	if (!error.empty()) {
		std::cerr << error << std::endl;
		askPassword(message);
	}
	return password;
}
std::string PasswordManager::generatePassword(const std::string& specialCharAllowed, unsigned int length) {
	AutoSeededRandomPool prng;

	const unsigned int size = 30;
	byte randomBytes[size];

	std::string password;
	while (password.size() < length) {
		prng.GenerateBlock(randomBytes, size);
		for (unsigned int i = 0; i < size; ++i) {
			char current = randomBytes[i];
			if (('0' <= current && current <= '9') || //
					('A' <= current && current <= 'Z') || //
					('a' <= current && current <= 'z') || //
					(specialCharAllowed.find(current) != specialCharAllowed.npos)) {
				password.push_back(current);
			}
		}
	}
	return password;
}

std::string PasswordManager::checkPasswordConstraints(const std::string& password) {
	if (password.size() < PASSWORD_MIN_LENGTH) {
		return "Your password must be 8 char long minimum";
	}
	return "";
}

//END Of file : ./PasswordManager.cpp
