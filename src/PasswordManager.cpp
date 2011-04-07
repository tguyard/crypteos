/**
 * @file ./PasswordManager.cpp
 * @author 
 */

// Includes /////////////////////////////////////////////////////// Includes //
#include "PasswordManager.h"

#include "crypto++/osrng.h"
using CryptoPP::AutoSeededRandomPool;

#ifdef HAVE_TERMIOS_H
#include <termios.h>
#elif HAVE_CONIO_H
#include "conio.h"
#endif
#include <iostream>
#include <stdexcept>

std::string PasswordManager::askPasswordHidden(const std::string& message) {
	std::string password;
#ifdef HAVE_TERMIOS_H

	struct termios oflags, nflags;
	char pass[128];

	/* disabling echo */
	tcgetattr(fileno(stdin), &oflags);
	nflags = oflags;
	nflags.c_lflag &= ~ECHO;
	nflags.c_lflag |= ECHONL;

	if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
		perror("tcsetattr");
		throw std::runtime_error("Unknown error...");
	}

	std::cout << message;
	fgets(pass, sizeof(pass), stdin);
	password.assign(pass, pass + strlen(pass) - 1);

	/* restore terminal */
	if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
		perror("tcsetattr");
		throw std::runtime_error("Unknown error...");
	}

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
		askPasswordHidden(message);
	}
	return password;
}
std::string PasswordManager::askPassword() {
	std::string password;
	std::cin >> password;
	std::string error = checkPasswordConstraints(password);
		if (!error.empty()) {
			std::cerr << error << std::endl;
			askPassword();
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
