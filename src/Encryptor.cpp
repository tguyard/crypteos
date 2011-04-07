/**
 * @file ./Generator.cpp
 * @author 
 */

// Resolve a bug in crypto++ !!
#include "crypto++/integer.h"

// Includes /////////////////////////////////////////////////////// Includes //
#include "Encryptor.h"

#include "crypto++/osrng.h"
using CryptoPP::AutoSeededRandomPool;
#include "crypto++/cryptlib.h"
using CryptoPP::Exception;
#include "crypto++/hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;
#include "crypto++/filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;
#include "crypto++/aes.h"
using CryptoPP::AES;
#include "crypto++/modes.h"
using CryptoPP::ECB_Mode;
#include "crypto++/pwdbased.h"
using CryptoPP::PKCS5_PBKDF2_HMAC;

#include <string>
#include <iostream>

static const unsigned int ITERATIONS = 100000;
static const unsigned int AES_SIZE = AES::MAX_KEYLENGTH;

Encryptor::Encryptor(const std::string& pass, const std::string& theSalt) {
	// Initialize the random pool
	AutoSeededRandomPool prng;

	Encryptor::byteString passphrase;
	passphrase.assign(pass.begin(), pass.end());
	Encryptor::byteString salt;
	salt.assign(theSalt.begin(), theSalt.end());


	PKCS5_PBKDF2_HMAC<CryptoPP::SHA512> passToKey;
	key = new byte[AES_SIZE];
	passToKey.DeriveKey(key, AES_SIZE, '\0', passphrase.c_str(),
			passphrase.size(), salt.c_str(), salt.size(), ITERATIONS);

	// prng.GenerateBlock(key, sizeof(key));

}

Encryptor::~Encryptor() {
	delete key;
}

std::string Encryptor::encrypt(const std::string& word)
		throw (CryptoPP::Exception) {
	std::string cipher;

	ECB_Mode<AES>::Encryption e;
	e.SetKey(key, AES_SIZE);

	// The StreamTransformationFilter adds padding
	//  as required. ECB and CBC Mode must be padded
	//  to the block size of the cipher.
	StringSource(word, true,
			new StreamTransformationFilter(e, new StringSink(cipher)) // StreamTransformationFilter
	); // StringSource

	std::string encoded;
	StringSource(cipher, true, new HexEncoder(new StringSink(encoded))); // StringSource
	return encoded;

}
std::string Encryptor::decrypt(const std::string& word)
		throw (CryptoPP::Exception) {
	ECB_Mode<AES>::Decryption d;
	d.SetKey(key, AES_SIZE);

	std::string cipher;
	StringSource(word, true, new HexDecoder(new StringSink(cipher)));

	std::string recovered;
	StringSource s(cipher, true,
			new StreamTransformationFilter(d, new StringSink(recovered)) // StreamTransformationFilter
			); // StringSource

	return recovered;
}

//END Of file : ./Generator.cpp
