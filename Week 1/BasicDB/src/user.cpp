#include "user.hpp"
#include <iostream>
#include <stdexcept>
#include <ctime>

// Constructor
User::User(int id, const std::string& first_name, const std::string& last_name,
           const std::string& email, std::time_t created_at, std::time_t updated_at)
    : id(id), first_name(first_name), last_name(last_name),
      email(email), created_at(created_at), updated_at(updated_at) {}

// Getters
int User::getId() const { return id; }
std::string User::getFirstName() const { return first_name; }
std::string User::getLastName() const { return last_name; }
std::string User::getEmail() const { return email; }
std::time_t User::getCreatedAt() const { return created_at; }
std::time_t User::getUpdatedAt() const { return updated_at; }

// Utility
std::string User::getFullName() const {
    return first_name + " " + last_name;
}

void User::getUserInfo() const {
    std::cout << "-- User Information  --" << std::endl;
    std::cout << "ID: " << getId() << std::endl;
    std::cout << "Name: " << getFullName() << std::endl;
    std::cout << "E-mail: " << getEmail() << std::endl;

    time_t create = getCreatedAt();
    time_t update = getUpdatedAt();
    struct tm* createInfo = localtime(&create);
    struct tm* updateInfo = localtime(&update);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", createInfo);
    std::cout << "Created at: " << buffer << std::endl;

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", updateInfo);
    std::cout << "Updated at: " << updateInfo << std::endl;
} 

// Database operations
std::optional<User> User::getUserByEmail(pqxx::connection& conn, const std::string& email) {
    try {
        pqxx::work txn(conn);
        
        // Query the database for a user with the given email
        auto result = txn.exec_params(
            "SELECT id, first_name, last_name, email, "
            "EXTRACT(EPOCH FROM created_at)::integer as created_at_epoch, "
            "EXTRACT(EPOCH FROM updated_at)::integer as updated_at_epoch "
            "FROM users WHERE email = $1",
            email);
        
        // If no user found with this email, return empty optional
        if (result.empty()) {
            return std::nullopt;
        }
        
        // Extract data from the first row
        const auto& row = result[0];
        int id = row["id"].as<int>();
        std::string first_name = row["first_name"].as<std::string>();
        std::string last_name = row["last_name"].as<std::string>();
        std::string user_email = row["email"].as<std::string>();
        std::time_t created_at = row["created_at_epoch"].as<std::time_t>();
        std::time_t updated_at = row["updated_at_epoch"].as<std::time_t>();
        
        // Create and return a User object
        return User(id, first_name, last_name, user_email, created_at, updated_at);
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to get user by email: " + std::string(e.what()));
    }
}