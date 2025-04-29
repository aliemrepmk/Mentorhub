#pragma once

#include <string>

class UserManager {
public:
    static bool createUser(const std::string& first_name, const std::string& last_name, const std::string& email, const std::string& password);
    static bool readUser(const std::string& email);
    static bool updateUser(const std::string& email, const std::string& new_firstname);
    static bool deleteUser(const std::string& email);
    static bool verifyUser(const std::string& email, const std::string& password);
    static int getUserId(const std::string& email);
};