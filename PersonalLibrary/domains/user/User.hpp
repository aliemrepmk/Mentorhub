#pragma once
#include <string>

class User {
    int id;
    std::string firstName, lastName, email, passwordHash;

public:
    User() = default;

    User(int id, std::string fn, std::string ln, std::string em, std::string hash)
        : id(id), firstName(std::move(fn)), lastName(std::move(ln)), email(std::move(em)), passwordHash(std::move(hash)) {}

    int getId() const { return id; }
    const std::string& getEmail() const { return email; }
    const std::string& getPasswordHash() const { return passwordHash; }
    const std::string& getFirstName() const { return firstName; }
    const std::string& getLastName() const { return lastName; }
};