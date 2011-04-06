/**
 * @file main.cpp
 * @brief
 * @author 
 *
 * File created for the Digiplant project.
 */

#include	<iostream>
#include "gcryptWrapper/crypto.hh"

int main(int /*argc*/, char** /* *argv[] */) {
	crypto::blockcipher* bc = new crypto::blockcipher(
			crypto::CIPHER_AES128, crypto::CIPHER_MODE_CTR);
	std::cout << "Set key" << std::endl;
	bc->secretkey("a12ca12ca1a12ca12ca1a12ca12ca11");

	std::cout << "Encrypting" << std::endl;
	bc->encrypt("A");

	crypto::bytes value = bc->final();
	for (unsigned int i = 0; i < value.size(); ++i) {
		std::cout << value[i];
	}
	std::cout << std::endl;

	std::cout << "Decrypting" << std::endl;
	bc->decrypt(value);

	value = bc->final();
	for (unsigned int i = 0; i < value.size(); ++i) {
		std::cout << value[i];
	}
	std::cout << std::endl;

	delete bc;

	return 0;
} // ----------  end of function main  ----------
