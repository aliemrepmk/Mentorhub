#pragma once
#include "user/IUserRepository.hpp"
#include <memory>

class DeleteUser {
public:
    explicit DeleteUser(std::shared_ptr<IUserRepository> repo);
    bool execute(const std::string& email);

private:
    std::shared_ptr<IUserRepository> repo;
};