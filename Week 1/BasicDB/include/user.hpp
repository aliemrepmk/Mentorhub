#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <ctime>
#include <optional>
#include <pqxx/pqxx>

class User {
private:
    int id;
    std::string first_name;
    std::string last_name;
    std::string email;
    std::time_t created_at;
    std::time_t updated_at;

public:
    // Constructor
    User(int id, const std::string& first_name, const std::string& last_name,
         const std::string& email, std::time_t created_at, std::time_t updated_at);

    // Getters
    int getId() const;
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getEmail() const;
    std::time_t getCreatedAt() const;
    std::time_t getUpdatedAt() const;

    // Database operations
    static std::optional<User> getUserByEmail(pqxx::connection& conn, const std::string& email);
    
    // Utility
    std::string getFullName() const;
    void getUserInfo() const;
};

#endif // USER_HPP