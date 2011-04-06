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

class DBManager : public boost::noncopyable
{
// Constructor destructor ////////////////////////////////////////////////////
public:
    /** @brief */
    DBManager(const std::string& password, const std::string& dbfile);
    /** @brief */
    /*final*/ ~DBManager();

// Public Methods ////////////////////////////////////////// Public Methods //
public:
    std::string getKey(const std::string& serviceName);
    std::vector<std::string> getServiceNames();

    void addKey(const std::string& serviceName, const std::string& key);

    void applyChanges();

private:
    void read();
    void write();

// Attributes /////////////////////////////////////////////////// Attributes //
private:
    typedef std::map<std::string/*names*/, std::string/*encrypted*/> DB;
    Encryptor* encryptor;
    DB values;
    std::string dbfile;
    std::string salt;
    bool parseError;

};

#endif /* DBMANAGER_H_ */
