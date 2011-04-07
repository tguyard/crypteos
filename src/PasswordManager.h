/**
 * @file ./PasswordManager.h
 * @brief Declare the PasswordManager class
 * @author 
 */

#ifndef PASSWORDMANAGER_H_
#define PASSWORDMANAGER_H_

// Local includes /////////////////////////////////////////// Local includes //

// System includes ///////////////////////////////////////// System includes //
#include <boost/noncopyable.hpp>
#include <string>

/**
 * @brief Utility class. Contain static methods on password generation.
 */
class PasswordManager: public boost::noncopyable {
	// Constructor destructor ////////////////////////////////////////////////////
private:
	PasswordManager() {
	}
	virtual ~PasswordManager() {
	}

	// Public Methods ////////////////////////////////////////// Public Methods //
public:
	static std::string askPasswordHidden(const std::string& message);
	static std::string askPassword();
	static std::string generatePassword(const std::string& specialCharAllowed, unsigned int length);

private:
	static const unsigned int PASSWORD_MIN_LENGTH = 8;
	static std::string checkPasswordConstraints(const std::string& password);
};

#endif /* PASSWORDMANAGER_H_ */
