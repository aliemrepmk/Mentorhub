#include <QApplication>
#include "include/gui/loginWindow.hpp"

#include "database/dbManager.hpp"
#include "user.hpp"

#include <iostream>
#include <string>
#include <pqxx/pqxx>
#include <stdexcept>

bool createUser(pqxx::connection& conn, std::string first_name, std::string last_name, std::string email) {
    try {
        pqxx::work w(conn);

        // Check if user already exists
        auto result = w.exec_params("SELECT id FROM users WHERE email = $1", email);

        if (!result.empty()) {
            return false; // Email already exists
        } else {
            w.exec_params("INSERT INTO users (first_name, last_name, email)" 
            "VALUES ($1, $2, $3)",
            first_name, last_name, email);

            w.commit();
            std::cout << "User added to database\n";
            return true;
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Can't add  user to database: " + std::string(e.what()));
    }

    return false;
}

bool readUser(pqxx::connection& conn, std::string email) {
    try {
        pqxx::work w(conn);

        // Check if user already exists
        auto result = w.exec_params("SELECT id, first_name, last_name, email FROM users WHERE email = $1", email);

        if (!result.empty()) {
            std::cout << "\nUser found!\n";
            for (const auto& row : result) {
                std::cout << "ID: " << row["id"].as<int>() << "\n"
                          << "First Name: " << row["first_name"].as<std::string>() << "\n"
                          << "Last Name: " << row["last_name"].as<std::string>() << "\n"
                          << "Email: " << row["email"].as<std::string>() << "\n\n";
            }

            w.commit();
            return true; // Email exists
        } else {
            std::cout << "User does not exists\n";
            return false;
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Error: " + std::string(e.what()));
    }

    return false;
}

bool updateUser(pqxx::connection& conn, std::string email, std::string new_firstname) {
    try {
        pqxx::work w(conn);

        // Check if user already exists
        auto result = w.exec_params("SELECT id FROM users WHERE email = $1", email);

        if (!result.empty()) {
            w.exec_params("UPDATE users SET first_name = $1 WHERE email = $2", new_firstname, email);
            w.commit();

            std::cout << "User is updated\n";

            return true; // Email exists
        } else {
            std::cout << "User does not exists\n";
            return false;
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Error: " + std::string(e.what()));
    }

    return false;
}

bool deleteUser(pqxx::connection& conn, std::string email) {
    try {
        pqxx::work w(conn);

        // Check if user already exists
        auto result = w.exec_params("SELECT id FROM users WHERE email = $1", email);

        if (!result.empty()) {
            w.exec_params("DELETE FROM users WHERE email = $1", email);
            w.commit();

            std::cout << "User is deleted\n";

            return true; // Email exists
        } else {
            std::cout << "User does not exists\n";
            return false;
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Error: " + std::string(e.what()));
    }

    return false;
}

void explainDb(pqxx::connection& conn, std::string email) {
    try {
        pqxx::work w(conn);

        auto check = w.exec_params("SELECT id FROM users WHERE email = $1", email);
        if (check.empty()) {
            std::cout << "User does not exist\n";
            return;
        }

        std::cout << "\nEXPLAIN SELECT * FROM users WHERE email = '" << email << "'\n";
        auto explain_result = w.exec_params("EXPLAIN SELECT * FROM users WHERE email = $1", email);
        for (const auto& row : explain_result) {
            std::cout << row[0].c_str() << "\n";
        }

        std::cout << "\nEXPLAIN ANALYZE SELECT * FROM users WHERE email = '" << email << "'\n";
        auto analyze_result = w.exec_params("EXPLAIN ANALYZE SELECT * FROM users WHERE email = $1", email);
        for (const auto& row : analyze_result) {
            std::cout << row[0].c_str() << "\n";
        }

        w.commit();
    } catch (const std::exception& e) {
        throw std::runtime_error("Error: " + std::string(e.what()));
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Initialize database connection
    const std::string CONNECTION_STRING = "dbname=user_db user=postgres password=381755 hostaddr=127.0.0.1 port=5432";
    DatabaseManager::initialize(CONNECTION_STRING);
    auto& dbManager = DatabaseManager::getInstance();
    auto& conn = dbManager.getConnection();

    // === DB Setup ===
    dbManager.initializeTables();
    dbManager.populateBooks("/Users/aliemrepamuk/Desktop/BasicDB/json/books.json");

    // Add initial users
    createUser(conn, "Ali Emre",    "Pamuk", "aliemrepamuk0@gmail.com");
    createUser(conn, "Ahmet Erdem", "Pamuk", "ahmeterdempamuk@gmail.com");
    createUser(conn, "Mehmet Emin", "Pamuk", "mehmeteminpamuk@gmail.com");
    createUser(conn, "Bekir",       "Pamuk", "bekirpamuk@gmail.com");
    createUser(conn, "Özgür",       "Pamuk", "özgürpamuk@gmail.com");

    // Load example reading lists
    dbManager.populateReadingLists("/Users/aliemrepamuk/Desktop/BasicDB/json/reading_lists.json");


    std::cout << "\n\n" << std::endl;
    
    // Retrieve an user by email
    try {
        auto userOpt = User::getUserByEmail(conn, "aliemrepamuk0@gmail.com");
        if (userOpt.has_value()) {
            const auto& user = userOpt.value();
            user.getUserInfo();
        } else {
            std::cout << "User not found!\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    std::cout << "\n\n" << std::endl;

    dbManager.listUsers();
    std::cout << "\n\n" << std::endl;

    dbManager.listBooks();
    std::cout << "\n\n" << std::endl;

    dbManager.listUserReadingLists(5);

    // === Example Reading Goal Usage ===
    // User ID 5 wants to finish reading list ID 25 in 30 days
    dbManager.createReadingGoal(5, 25, 30);
    dbManager.updateBookProgress(1);
    dbManager.showGoalProgress(1);


    // === Launch GUI Login Window ===
    LoginWindow loginWindow;
    loginWindow.show();

    int result = app.exec();

    // Close the database connection
    dbManager.closeConnection();

    return result;
}
