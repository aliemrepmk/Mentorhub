#include "PostgreDBManager.hpp"
#include <iostream>
#include <stdexcept>

void PostgreDBManager::initialize(const std::string& connStr) {
    connectionString = connStr;
}

PostgreDBManager& PostgreDBManager::getInstance() {
    static PostgreDBManager instance(connectionString);
    return instance;
}

PostgreDBManager::PostgreDBManager(const std::string& connStr) {
    try {
        conn = std::make_unique<pqxx::connection>(connStr);
        if (!conn->is_open()) {
            throw std::runtime_error("Failed to connect to PostgreSQL");
        }
        std::cout << "[PostgreDBManager] Connected to PostgreSQL\n";
    } catch (const std::exception& e) {
        throw std::runtime_error("PostgreDBManager init failed: " + std::string(e.what()));
    }
}

pqxx::connection& PostgreDBManager::getConnection() {
    return *conn;
}

bool PostgreDBManager::initializeTables() {
    try {
        pqxx::work txn(getConnection());

        // Check if 'users' table already exists
        auto result = txn.exec(
            "SELECT EXISTS ("
            "SELECT FROM pg_tables WHERE schemaname = 'public' AND tablename = 'users'"
            ");"
        );

        if (result[0][0].as<bool>()) {
            std::cout << "[PostgreDBManager] Tables already exist. Skipping initialization.\n";
            return false;
        }

        // If not, create tables
        txn.exec(R"(
            CREATE TABLE users (
                id SERIAL PRIMARY KEY,
                first_name TEXT NOT NULL,
                last_name TEXT NOT NULL,
                email TEXT UNIQUE NOT NULL,
                password_hash TEXT NOT NULL,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            );

            CREATE TABLE books (
                id SERIAL PRIMARY KEY,
                book_name TEXT NOT NULL,
                author_name TEXT NOT NULL,
                publish_date DATE,
                isbn TEXT UNIQUE
            );

            CREATE TABLE reading_list (
                id SERIAL PRIMARY KEY,
                user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
                name TEXT NOT NULL,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            );

            CREATE TABLE reading_list_books (
                reading_list_id INTEGER NOT NULL REFERENCES reading_list(id) ON DELETE CASCADE,
                book_id INTEGER NOT NULL REFERENCES books(id) ON DELETE CASCADE,
                added_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                PRIMARY KEY (reading_list_id, book_id)
            );

            CREATE TABLE reading_goals (
                id SERIAL PRIMARY KEY,
                user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
                reading_list_id INTEGER NOT NULL REFERENCES reading_list(id) ON DELETE CASCADE,
                duration_days INTEGER NOT NULL CHECK (duration_days > 0),
                start_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                deadline TIMESTAMP NOT NULL,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            );

            CREATE TABLE goal_progress (
                goal_id INTEGER NOT NULL REFERENCES reading_goals(id) ON DELETE CASCADE,
                book_id INTEGER NOT NULL REFERENCES books(id) ON DELETE CASCADE,
                is_completed BOOLEAN DEFAULT FALSE,
                marked_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                PRIMARY KEY (goal_id, book_id)
            );
        )");

        txn.commit();
        std::cout << "[PostgreDBManager] Tables created successfully.\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[PostgreDBManager] Failed to initialize tables: " << e.what() << '\n';
        return false;
    }
}

void PostgreDBManager::closeConnection() {
    if (conn && conn->is_open()) {
        conn.reset();
        std::cout << "[PostgreDBManager] Connection closed\n";
    }
}

PostgreDBManager::~PostgreDBManager() {
    closeConnection();
}