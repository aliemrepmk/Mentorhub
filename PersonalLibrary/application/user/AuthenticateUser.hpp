#pragma once
#include "user/IUserRepository.hpp"
#include "user/PasswordHasher.hpp"
#include <memory>

class AuthenticateUser {
public:
    AuthenticateUser(std::shared_ptr<IUserRepository> repo, std::shared_ptr<PasswordHasher> hasher);
    bool execute(const std::string& email, const std::string& password);

private:
    std::shared_ptr<IUserRepository> repo;
    std::shared_ptr<PasswordHasher> hasher;
};