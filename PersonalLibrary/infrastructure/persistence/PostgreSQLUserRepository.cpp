#include "PostgreSQLUserRepository.hpp"
#include "db/PostgreDBManager.hpp"
#include <pqxx/pqxx>
#include <stdexcept>

void PostgreSQLUserRepository::save(const User& user) {
    pqxx::work w(PostgreDBManager::getInstance().getConnection());
    w.exec_params(
        "INSERT INTO users (first_name, last_name, email, password_hash) VALUES ($1, $2, $3, $4)",
        user.getFirstName(), user.getLastName(), user.getEmail(), user.getPasswordHash()
    );
    w.commit();
}

std::optional<User> PostgreSQLUserRepository::findByEmail(const std::string& email) {
    pqxx::work w(PostgreDBManager::getInstance().getConnection());
    auto r = w.exec_params(
        "SELECT id, first_name, last_name, email, password_hash FROM users WHERE email = $1",
        email
    );

    if (r.empty()) return std::nullopt;

    const auto& row = r[0];
    return User(
        row["id"].as<int>(),
        row["first_name"].as<std::string>(),
        row["last_name"].as<std::string>(),
        row["email"].as<std::string>(),
        row["password_hash"].as<std::string>()
    );
}

void PostgreSQLUserRepository::updateFirstName(const std::string& email, const std::string& newName) {
    pqxx::work w(PostgreDBManager::getInstance().getConnection());
    w.exec_params(
        "UPDATE users SET first_name = $1 WHERE email = $2",
        newName, email
    );
    w.commit();
}

void PostgreSQLUserRepository::removeByEmail(const std::string& email) {
    pqxx::work w(PostgreDBManager::getInstance().getConnection());
    w.exec_params(
        "DELETE FROM users WHERE email = $1",
        email
    );
    w.commit();
}

int PostgreSQLUserRepository::getIdByEmail(const std::string& email) {
    pqxx::work w(PostgreDBManager::getInstance().getConnection());
    auto r = w.exec_params(
        "SELECT id FROM users WHERE email = $1",
        email
    );

    if (r.empty()) {
        throw std::runtime_error("getIdByEmail: user not found for email: " + email);
    }

    return r[0]["id"].as<int>();
}