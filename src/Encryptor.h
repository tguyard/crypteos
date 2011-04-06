/**
 * @file ./Generator.h
 * @brief Declare the Generator class
 * @author 
 */

#ifndef GENERATOR_H_
#define GENERATOR_H_

// Local includes /////////////////////////////////////////// Local includes //

// System includes ///////////////////////////////////////// System includes //
#include <crypto++/cryptlib.h>
#include <boost/noncopyable.hpp>
#include <string>

/**
 * @brief 
 */
class Encryptor : public boost::noncopyable
{
// Constructor destructor ////////////////////////////////////////////////////
public:
    /** @brief */
    explicit Encryptor(const std::string& pass, const std::string& theSalt);
    /** @brief */
    /*final*/ ~Encryptor();

// Public Methods ////////////////////////////////////////// Public Methods //
public:
    std::string encrypt(const std::string& word)throw (CryptoPP::Exception);
    std::string decrypt(const std::string& word)throw (CryptoPP::Exception);

// Attributes /////////////////////////////////////////////////// Attributes //
private:
	typedef std::basic_string<byte> byteString;

    byte* key;
};

#endif /* GENERATOR_H_ */
