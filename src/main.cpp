/**
 * @file main.cpp
 * @brief
 * @author 
 *
 * File created for the Digiplant project.
 */


#include	<iostream>
#include "gcryptWrapper/crypto.hh"

int main ( int /*argc*/, char** /* *argv[] */ )
{
	std::cout << "Hello World !! " << std::endl;


	std::cout << "Constructing" << std::endl;
    crypto::blockcipher* bc = new crypto::blockcipher(crypto::CIPHER_AES128, crypto::CIPHER_MODE_CTR);

	std::cout << "Set key" << std::endl;
    bc->secretkey("plop");

	std::cout << "Encrypting" << std::endl;
    bc->encrypt("Hello World !!");
    
    std::cout << bc->iv() << std::endl;

	std::cout << "Decrypting" << std::endl;
    bc->decrypt(bc->iv());


    std::cout << bc->iv() << std::endl;

    delete bc;


	return 0;
}				// ----------  end of function main  ----------
