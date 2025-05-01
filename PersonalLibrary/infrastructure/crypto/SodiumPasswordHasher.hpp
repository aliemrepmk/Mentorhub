#pragma once
#include "user/PasswordHasher.hpp"

class SodiumPasswordHasher : public PasswordHasher {
public:
    std::string hash(const std::string& password) override;
    bool verify(const std::string& hash, const std::string& password) override;
};