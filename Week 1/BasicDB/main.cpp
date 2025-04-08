#include <QApplication>
#include "include/gui/loginWindow.hpp"

#include "database/dbManager.hpp"
#include "database/userManager.hpp"
#include <sodium.h>
#include <pqxx/pqxx>

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <stdexcept>

int main(int argc, char *argv[]) {
    if (sodium_init() < 0) {
        throw std::runtime_error("Failed to initialize libsodium.");
    }

    QApplication app(argc, argv);

    // Initialize database connection
    DatabaseManager::initialize(DatabaseManager::loadConnectionString("/Users/aliemrepamuk/Desktop/CENG/Mentorhub/Week 1/BasicDB/.env"));
    auto& dbManager = DatabaseManager::getInstance();
    auto& conn = dbManager.getConnection();

    // === DB Setup ===
    dbManager.initializeTables();
    dbManager.populateBooks("/Users/aliemrepamuk/Desktop/BasicDB/json/books.json");

    // Add initial users
    UserManager::createUser("Ali Emre",    "Pamuk", "aliemrepamuk0@gmail.com",   "SuperSecret123");
    UserManager::createUser("Ahmet Erdem", "Pamuk", "ahmeterdempamuk@gmail.com", "SuperSecret123");
    UserManager::createUser("Mehmet Emin", "Pamuk", "mehmeteminpamuk@gmail.com", "SuperSecret123");
    UserManager::createUser("Bekir",       "Pamuk", "bekirpamuk@gmail.com",      "SuperSecret123");
    UserManager::createUser("Özgür",       "Pamuk", "özgürpamuk@gmail.com",      "SuperSecret123");

    // Load example reading lists
    dbManager.populateReadingLists("/Users/aliemrepamuk/Desktop/BasicDB/json/reading_lists.json");

    // === Launch GUI Login Window ===
    LoginWindow loginWindow;
    loginWindow.show();

    int result = app.exec();

    // Close the database connection
    dbManager.closeConnection();

    return result;
}