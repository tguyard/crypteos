/**
 * @file ./DBManager.h
 * @brief Declare the DBManager class
 * @author 
 */

#ifndef DBMANAGER_H_
#define DBMANAGER_H_

// Local includes /////////////////////////////////////////// Local includes //
#include "Encryptor.h"

// System includes ///////////////////////////////////////// System includes //
#include <boost/noncopyable.hpp>
#include <vector>
#include <string>
#include <map>

/**
 * The DB file is of the following form:
 * First line: The salt string
 * Second line: An encrypted random string (to challenge the password)
 * then: each line is a pair of encrypted service/password separated by a space.
 */
class DBManager: public boost::noncopyable {
	// Constructor destructor ////////////////////////////////////////////////////
public:
	typedef enum {
		OK, UNKNOWN_ERROR, WRONG_PASSWORD, WRONG_DB_FILE
	} ERROR;

	/** @brief */
	DBManager(const std::string& dbfile);
	/** @brief */
	/*final*/
	~DBManager();

	// Public Methods ////////////////////////////////////////// Public Methods //
public:
	ERROR initialize(const std::string& password);
	std::string getKey(const std::string& serviceName);
	std::vector<std::string> getServiceNames();
	bool addKey(const std::string& serviceName, const std::string& key);
	bool applyChanges();

private:
	ERROR createDBFile(const std::string& password, const std::string& dbfile);
	/**
	 * Create the Encryptor.
	 */
	ERROR readDBFile(const std::string& password, std::ifstream& dbfile);

	// Attributes /////////////////////////////////////////////////// Attributes //
private:
	typedef std::map<std::string/*names*/, std::string/*encrypted*/> DB;
	Encryptor* encryptor;
	DB values;
	std::string dbfile;
	std::string salt;
	std::string challenge;
	bool parseError;

};

#endif /* DBMANAGER_H_ */
