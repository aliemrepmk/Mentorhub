#pragma once

#include <string>
#include <memory>
#include <pqxx/pqxx>

class DatabaseManager {
public:
    // Get the instance of database
    static DatabaseManager& getInstance();

    // Initialize the singleton with connection string
    static void initialize(const std::string& connectionString);

    // Copy prevention
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Connect function
    pqxx::connection& getConnection();

    // Explicitly close the database connection
    void closeConnection();

    // Function to initalize tables & populate tables
    void initializeTables();
    void populateBooks(const std::string& json_file_path);
    void populateReadingLists(const std::string& json_file_path);

    // Functions for reading goals
    void createReadingGoal(int user_id, int reading_list_id, int duration_days);
    void updateBookProgress(int goal_id);
    void showGoalProgress(int goal_id);

    // Utility
    void listUsers();
    void listBooks();
    void listUserReadingLists(int id);
private:
    DatabaseManager(const std::string& connectionString);
    ~DatabaseManager();

    static std::string connectionString;
    std::unique_ptr<pqxx::connection> conn;
};