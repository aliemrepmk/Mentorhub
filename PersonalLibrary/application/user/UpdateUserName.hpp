#pragma once
#include "user/IUserRepository.hpp"
#include <memory>
#include <string>

class UpdateUserName {
public:
    explicit UpdateUserName(std::shared_ptr<IUserRepository> repo);
    bool execute(const std::string& email, const std::string& newFirstName);

private:
    std::shared_ptr<IUserRepository> repo;
};