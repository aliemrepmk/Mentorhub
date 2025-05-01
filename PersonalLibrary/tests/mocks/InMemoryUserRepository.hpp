#pragma once
#include "domains/user/IUserRepository.hpp"
#include <unordered_map>
#include <optional>

class InMemoryUserRepository : public IUserRepository {
private:
    std::unordered_map<std::string, User> users;
    int idCounter = 1;

public:
    void save(const User& user) override {
        User saved(idCounter++, user.getFirstName(), user.getLastName(), user.getEmail(), user.getPasswordHash());
        users[user.getEmail()] = saved;
    }

    std::optional<User> findByEmail(const std::string& email) override {
        auto it = users.find(email);
        if (it != users.end()) return it->second;
        return std::nullopt;
    }

    void updateFirstName(const std::string& email, const std::string& newName) override {
        auto it = users.find(email);
        if (it != users.end()) {
            User u = it->second;
            users[email] = User(u.getId(), newName, u.getLastName(), u.getEmail(), u.getPasswordHash());
        }
    }

    void removeByEmail(const std::string& email) override {
        users.erase(email);
    }

    int getIdByEmail(const std::string& email) override {
        auto it = users.find(email);
        if (it == users.end()) {
            throw std::runtime_error("User not found: " + email);
        }
        return it->second.getId();
    }
};