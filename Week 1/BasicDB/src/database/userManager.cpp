#include "database/userManager.hpp"
#include "database/dbManager.hpp"

#include <sodium.h>

#include <iostream>
#include <stdexcept>
#include <pqxx/pqxx>

bool UserManager::createUser(const std::string& first_name, const std::string& last_name,
                             const std::string& email, const std::string& password) {
    auto& conn = DatabaseManager::getInstance().getConnection();

    try {
        pqxx::work w(conn);

        // Check if email already exists
        auto result = w.exec_params("SELECT id FROM users WHERE email = $1", email);
        if (!result.empty()) return false;

        // Hash password with Argon2id
        char hashed_password[crypto_pwhash_STRBYTES];
        if (crypto_pwhash_str(
                hashed_password,
                password.c_str(),
                password.length(),
                crypto_pwhash_OPSLIMIT_MODERATE,
                crypto_pwhash_MEMLIMIT_MODERATE) != 0) {
            throw std::runtime_error("Out of memory while hashing password");
        }

        // Insert user
        w.exec_params("INSERT INTO users (first_name, last_name, email, password_hash) VALUES ($1, $2, $3, $4)",
                      first_name, last_name, email, std::string(hashed_password));
        w.commit();

        std::cout << "User added to database\n";
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Can't add user to database: " + std::string(e.what()));
    }
}

bool UserManager::readUser(const std::string& email) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    try {
        pqxx::work w(conn);
        auto result = w.exec_params("SELECT id, first_name, last_name, email FROM users WHERE email = $1", email);
        if (result.empty()) {
            std::cout << "User does not exist\n";
            return false;
        }

        std::cout << "\nUser found!\n";
        for (const auto& row : result) {
            std::cout << "ID: " << row["id"].as<int>() << "\n"
                      << "First Name: " << row["first_name"].as<std::string>() << "\n"
                      << "Last Name: " << row["last_name"].as<std::string>() << "\n"
                      << "Email: " << row["email"].as<std::string>() << "\n\n";
        }

        w.commit();
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error: " + std::string(e.what()));
    }
}

bool UserManager::updateUser(const std::string& email, const std::string& new_firstname) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    try {
        pqxx::work w(conn);
        auto result = w.exec_params("SELECT id FROM users WHERE email = $1", email);
        if (result.empty()) {
            std::cout << "User does not exist\n";
            return false;
        }

        w.exec_params("UPDATE users SET first_name = $1 WHERE email = $2", new_firstname, email);
        w.commit();
        std::cout << "User is updated\n";
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error: " + std::string(e.what()));
    }
}

bool UserManager::deleteUser(const std::string& email) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    try {
        pqxx::work w(conn);
        auto result = w.exec_params("SELECT id FROM users WHERE email = $1", email);
        if (result.empty()) {
            std::cout << "User does not exist\n";
            return false;
        }

        w.exec_params("DELETE FROM users WHERE email = $1", email);
        w.commit();
        std::cout << "User is deleted\n";
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error: " + std::string(e.what()));
    }
}

bool UserManager::verifyUser(const std::string& email, const std::string& password) {
    auto& conn = DatabaseManager::getInstance().getConnection();

    try {
        pqxx::work w(conn);
        auto result = w.exec_params("SELECT password_hash FROM users WHERE email = $1", email);
        if (result.empty()) return false;

        std::string hash = result[0]["password_hash"].as<std::string>();
        return crypto_pwhash_str_verify(hash.c_str(), password.c_str(), password.length()) == 0;
    } catch (...) {
        return false;
    }
}

int UserManager::getUserId(const std::string& email) {
    auto& conn = DatabaseManager::getInstance().getConnection();

    try {
        pqxx::work w(conn);
        auto result = w.exec_params("SELECT id FROM users WHERE email = $1", email);
        if (result.empty()) {
            std::cout << "User does not exist\n";
            return false;
        }

        int id = result[0]["id"].as<int>();
        return id;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error: " + std::string(e.what()));
    }
}