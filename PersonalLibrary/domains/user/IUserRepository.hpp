#pragma once
#include "User.hpp"
#include <optional>
#include <vector>

class IUserRepository {
public:
    virtual void save(const User& user) = 0;
    virtual std::optional<User> findByEmail(const std::string& email) = 0;
    virtual void updateFirstName(const std::string& email, const std::string& newName) = 0;
    virtual void removeByEmail(const std::string& email) = 0;
    virtual int getIdByEmail(const std::string& email) = 0;
    virtual ~IUserRepository() = default;
};