#pragma once

#include <pqxx/pqxx>
#include <memory>
#include <string>

class PostgreDBManager {
public:
    static void initialize(const std::string& connStr);
    static PostgreDBManager& getInstance();

    bool initializeTables();

    pqxx::connection& getConnection();
    void closeConnection();

private:
    explicit PostgreDBManager(const std::string& connStr);
    ~PostgreDBManager();

    PostgreDBManager(const PostgreDBManager&) = delete;
    PostgreDBManager& operator=(const PostgreDBManager&) = delete;

    std::unique_ptr<pqxx::connection> conn;
    static inline std::string connectionString;  // modern static member
};