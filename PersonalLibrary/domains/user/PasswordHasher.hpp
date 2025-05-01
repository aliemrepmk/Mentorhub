#pragma once
#include <string>

class PasswordHasher {
public:
    virtual std::string hash(const std::string& password) = 0;
    virtual bool verify(const std::string& hash, const std::string& password) = 0;
    virtual ~PasswordHasher() = default;
};