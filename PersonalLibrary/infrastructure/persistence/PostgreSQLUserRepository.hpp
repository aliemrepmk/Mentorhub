#pragma once
#include "user/IUserRepository.hpp"

class PostgreSQLUserRepository : public IUserRepository {
public:
    void save(const User& user) override;
    std::optional<User> findByEmail(const std::string& email) override;
    void updateFirstName(const std::string& email, const std::string& newName) override;
    void removeByEmail(const std::string& email) override;
    int getIdByEmail(const std::string& email) override;
};