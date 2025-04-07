#include "database/dbManager.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <limits>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

std::string DatabaseManager::connectionString;

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance(connectionString);
    return instance;
}

void DatabaseManager::initialize(const std::string& connString) {
    connectionString = connString;
}

DatabaseManager::DatabaseManager(const std::string& connString) {
    try {
        conn = std::make_unique<pqxx::connection>(connString);
        if(!conn->is_open()) {
            throw std::runtime_error("dbManager: Failed to connect to database");
        } else {
            std::cout << "dbManager: Successfully connected to database\n";
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("dbManager: Database connection failed: " + std::string(e.what()));
    }
}

DatabaseManager::~DatabaseManager() {
    if(conn && conn->is_open()) {
        conn->close();
    }
}

pqxx::connection& DatabaseManager::getConnection() {
    return *conn;
}

void DatabaseManager::closeConnection() {
    if(conn && conn->is_open()) {
        conn->close();
        std::cout << "dbManager: Connection to database is closed\n";
    }
}

void DatabaseManager::initializeTables() {
    try {
        pqxx::work w(*conn);
        
        // Drop tables if already exists
        w.exec("DROP TABLE IF EXISTS users CASCADE");
        w.exec("DROP TABLE IF EXISTS books CASCADE");
        w.exec("DROP TABLE IF EXISTS reading_list CASCADE");
        w.exec("DROP TABLE IF EXISTS reading_list_books CASCADE");
        w.exec("DROP TABLE IF EXISTS reading_goals CASCADE");
        w.exec("DROP TABLE IF EXISTS goal_progress CASCADE");
        
        // Create tables with new schema
        w.exec(R"(
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
        
        w.commit();
        std::cout << "dbManager: Successfully created the tables\n";
    } catch (const std::exception& e) {
        throw std::runtime_error("dbManager: Failed to initialize tables: " + std::string(e.what()));
    }
}

void DatabaseManager::populateBooks(const std::string& json_file_path) {
    std::ifstream file(json_file_path);
    if (!file.is_open()) {
        std::cerr << "dbManager: Could not open JSON file: " << json_file_path << std::endl;
        return;
    }

    json books_json;
    try {
        file >> books_json;
    } catch (const json::parse_error& e) {
        std::cerr << "dbManager: JSON parse error: " << e.what() << std::endl;
        return;
    }

    try {
        pqxx::work w(*conn);

        for (const auto& book : books_json) {
            std::string book_name = book.at("book_name").get<std::string>();
            std::string author_name = book.at("author_name").get<std::string>();
            std::string publish_date = book.value("publish_date", "NULL");
            std::string isbn = book.value("isbn", "");

            std::string query = "INSERT INTO books (book_name, author_name, publish_date, isbn) VALUES (" +
                w.quote(book_name) + ", " +
                w.quote(author_name) + ", " +
                (publish_date == "NULL" ? "NULL" : w.quote(publish_date)) + ", " +
                w.quote(isbn) + 
                ") ON CONFLICT (isbn) DO NOTHING;";

            w.exec(query);
        }

        w.commit();
        std::cout << "dbManager: Books inserted successfully" << std::endl;
        
    } catch (const std::exception& e) {
        throw std::runtime_error("dbManager: Failed to insert books: " + std::string(e.what()));
    }
}

void DatabaseManager::populateReadingLists(const std::string& json_file_path) {
    std::ifstream file(json_file_path);
    if (!file.is_open()) {
        std::cerr << "dbManager: Could not open JSON file: " << json_file_path << std::endl;
        return;
    }

    json lists_json;
    try {
        file >> lists_json;
    } catch (const json::parse_error& e) {
        std::cerr << "dbManager: JSON parse error: " << e.what() << std::endl;
        return;
    }

    try {
        pqxx::work w(*conn);

        for (const auto& list : lists_json) {
            int user_id = list.at("user_id").get<int>();
            std::string list_name = list.at("name").get<std::string>();
            const auto& books = list.at("books");

            // Validate user exists
            auto user_check = w.exec_params("SELECT id FROM users WHERE id = $1", user_id);
            if (user_check.empty()) {
                std::cerr << "dbManager::populateReadingLists: Skipping list \"" << list_name << "\" — user_id " << user_id << " does not exist.\n";
                continue;
            }

            // Insert reading list
            auto result = w.exec_params("INSERT INTO reading_list (user_id, name) VALUES ($1, $2) RETURNING id", user_id, list_name);
            int list_id = result[0]["id"].as<int>();

            int added_count = 0;
            for (const auto& book : books) {
                std::string isbn = book.at("isbn").get<std::string>();
                std::string added_at = book.value("added_at", "");

                // Validate book exists
                auto book_res = w.exec_params("SELECT id FROM books WHERE isbn = $1", isbn);
                if (book_res.empty()) {
                    std::cerr << "dbManager::populateReadingLists: Skipping book with ISBN " << isbn << " — not found in books table.\n";
                    continue;
                }

                int book_id = book_res[0]["id"].as<int>();

                // Insert into reading_list_books
                w.exec_params(
                    "INSERT INTO reading_list_books (reading_list_id, book_id, added_at) "
                    "VALUES ($1, $2, $3) ON CONFLICT DO NOTHING",
                    list_id, book_id, added_at);
                ++added_count;
            }

            std::cout << "dbManager: Added reading list \"" << list_name
                      << "\" (ID " << list_id << ") for user_id " << user_id
                      << " with " << added_count << " book(s)"
                      << std::endl;
        }

        w.commit();
        std::cout << "dbManager: Reading lists populated successfully" << std::endl;

    } catch (const std::exception& e) {
        throw std::runtime_error("dbManager: Failed to populate reading lists: " + std::string(e.what()));
    }
}

void DatabaseManager::listUsers() {
    std::cout << "|   ID   |     First name     |     Last name      |            E-mail            |" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------" << std::endl;

    try {
        pqxx::work w(*conn);

        auto result = w.exec("SELECT id, first_name, last_name, email FROM users ORDER BY id;");

        if (result.empty()) {
            std::cout << "dbManager::listUsers: No users found." << std::endl;
            return;
        }

        for (const auto& user : result) {
            int id = user["id"].as<int>();
            std::string first_name = user["first_name"].as<std::string>();
            std::string last_name = user["last_name"].as<std::string>();
            std::string user_email = user["email"].as<std::string>();

            std::cout << "| "
                      << std::setw(6) << std::left << id << " | "
                      << std::setw(18) << std::left << first_name << " | "
                      << std::setw(18) << std::left << last_name << " | "
                      << std::setw(28) << std::left << user_email << " |"
                      << std::endl;
        }

    } catch (const std::exception& e) {
        throw std::runtime_error("dbManager: Failed to list users: " + std::string(e.what()));
    }
}

void DatabaseManager::listBooks() {
    std::cout << "|   ID   |     Book name      |     Author name     |   Publish date   |          ISBN         |" << std::endl;
    std::cout << "------------------------------------------------------------------------------------------------" << std::endl;

    try {
        pqxx::work w(*conn);

        auto result = w.exec("SELECT id, book_name, author_name, publish_date, isbn FROM books ORDER BY id;");

        if (result.empty()) {
            std::cout << "dbManager::listBooks: No books found." << std::endl;
            return;
        }

        for (const auto& book : result) {
            int id = book["id"].as<int>();
            std::string book_name = book["book_name"].as<std::string>();
            std::string author_name = book["author_name"].as<std::string>();

            // Handle nullable publish_date
            std::string publish_date = book["publish_date"].is_null() ? "N/A" : book["publish_date"].as<std::string>();
            std::string isbn = book["isbn"].is_null() ? "N/A" : book["isbn"].as<std::string>();

            std::cout << "| "
                      << std::setw(6) << std::left << id << " | "
                      << std::setw(20) << std::left << book_name << " | "
                      << std::setw(20) << std::left << author_name << " | "
                      << std::setw(17) << std::left << publish_date << " | "
                      << std::setw(20) << std::left << isbn << " |"
                      << std::endl;
        }

    } catch (const std::exception& e) {
        throw std::runtime_error("dbManager: Failed to list books: " + std::string(e.what()));
    }
}

void DatabaseManager::listUserReadingLists(int id) {
    try {
        pqxx::work w(*conn);

        auto lists = w.exec_params("SELECT id, name, created_at FROM reading_list WHERE user_id = $1 ORDER BY created_at DESC", id);
        if(lists.empty()) {
            std::cout << "dbManager: No reading lists found for user" << std::endl;
            return;
        }

        std::vector<int> valid_list_ids;
        for (const auto& row : lists) {
            valid_list_ids.push_back(row["id"].as<int>());
        }

        std::cout << "|   ID   |         Name         |      Created At      |" << std::endl;
        std::cout << "--------------------------------------------------------" << std::endl;
        for (const auto& row : lists) {
            std::cout << "| "
                      << std::setw(6) << std::left << row["id"].as<int>() << " | "
                      << std::setw(22) << std::left << row["name"].as<std::string>() << " | "
                      << std::setw(20) << std::left << row["created_at"].as<std::string>() << " |"
                      << std::endl;
        }

        while (true) {
            std::cout << "\nEnter the ID of the reading list to view its details (or 0 to cancel): ";
            int selected_list_id;
            std::cin >> selected_list_id;

            if (selected_list_id == 0) {
                std::cout << "dbManager: Exiting reading list viewer" << std::endl;
                break;
            }

            // Validate reading list ID
            if (std::find(valid_list_ids.begin(), valid_list_ids.end(), selected_list_id) == valid_list_ids.end()) {
                std::cout << "dbManager: Invalid reading list ID. Please try again" << std::endl;
                continue;
            }

            // Fetch books in the selected list
            auto books = w.exec_params(R"(
                SELECT b.book_name, b.author_name, b.isbn, rlb.added_at
                FROM reading_list_books rlb
                JOIN books b ON rlb.book_id = b.id
                WHERE rlb.reading_list_id = $1
                ORDER BY rlb.added_at
            )", selected_list_id);

            std::cout << "\n Books in Reading List ID " << selected_list_id << ":" << std::endl;
            if (books.empty()) {
                std::cout << "dbManager: This reading list is empty" << std::endl;
                continue;
            }

            std::cout << "|     Book Name     |     Author     |       ISBN        |     Added At     |" << std::endl;
            std::cout << "-----------------------------------------------------------------------------" << std::endl;

            for (const auto& book : books) {
                std::string book_name = book["book_name"].as<std::string>();
                std::string author_name = book["author_name"].as<std::string>();
                std::string isbn = book["isbn"].is_null() ? "N/A" : book["isbn"].as<std::string>();
                std::string added_at = book["added_at"].as<std::string>();

                std::cout << "| "
                          << std::setw(18) << std::left << book_name << " | "
                          << std::setw(14) << std::left << author_name << " | "
                          << std::setw(17) << std::left << isbn << " | "
                          << std::setw(15) << std::left << added_at << " |"
                          << std::endl;
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "dbManager: Failed to show reading lists: " << e.what() << std::endl;
    }
}

void DatabaseManager::createReadingGoal(int user_id, int reading_list_id, int duration_days) {
    try {
        pqxx::work w(*conn);

        auto start_time = std::chrono::system_clock::now();
        std::time_t raw_start = std::chrono::system_clock::to_time_t(start_time);
        std::tm* start_tm = std::localtime(&raw_start);
        char start_buf[20];
        std::strftime(start_buf, sizeof(start_buf), "%Y-%m-%d %H:%M:%S", start_tm);

        auto deadline_time = start_time + std::chrono::hours(24 * duration_days);
        std::time_t raw_deadline = std::chrono::system_clock::to_time_t(deadline_time);
        std::tm* deadline_tm = std::localtime(&raw_deadline);
        char deadline_buf[20];
        std::strftime(deadline_buf, sizeof(deadline_buf), "%Y-%m-%d %H:%M:%S", deadline_tm);

        auto res = w.exec_params(
            "INSERT INTO reading_goals (user_id, reading_list_id, duration_days, start_date, deadline) VALUES ($1, $2, $3, $4, $5) RETURNING id",
            user_id, reading_list_id, duration_days, std::string(start_buf), std::string(deadline_buf)
        );
        int goal_id = res[0]["id"].as<int>();

        auto books = w.exec_params(
            "SELECT book_id FROM reading_list_books WHERE reading_list_id = $1",
            reading_list_id
        );

        for (const auto& row : books) {
            int book_id = row["book_id"].as<int>();
            w.exec_params("INSERT INTO goal_progress (goal_id, book_id) VALUES ($1, $2)", goal_id, book_id);
        }

        w.commit();
        std::cout << "Goal created successfully with ID: " << goal_id << std::endl;
    } catch (const std::exception& e) {
        throw std::runtime_error("dbManager: Failed to create reading goal: " + std::string(e.what()));
    }
}

void DatabaseManager::updateBookProgress(int goal_id) {
    try {
        pqxx::work w(*conn);

        auto books = w.exec_params(
            "SELECT gp.book_id, b.book_name FROM goal_progress gp JOIN books b ON gp.book_id = b.id WHERE gp.goal_id = $1",
            goal_id
        );

        // Clear input buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        for (const auto& row : books) {
            int book_id = row["book_id"].as<int>();
            std::string book_name = row["book_name"].as<std::string>();

            std::cout << "Have you completed the book: '" << book_name << "'? (y/n): ";
            std::string input;
            std::getline(std::cin, input);
            bool is_completed = (input == "y" || input == "Y");

            w.exec_params(
                "UPDATE goal_progress SET is_completed = $1, marked_at = CURRENT_TIMESTAMP WHERE goal_id = $2 AND book_id = $3",
                is_completed, goal_id, book_id
            );
        }

        w.commit();
        std::cout << "All book progress statuses updated." << std::endl;
    } catch (const std::exception& e) {
        throw std::runtime_error("dbManager: Failed to update book progress: " + std::string(e.what()));
    }
}

void DatabaseManager::showGoalProgress(int goal_id) {
    try {
        pqxx::work w(*conn);

        auto goal_info = w.exec_params(
            "SELECT g.user_id, g.reading_list_id, g.duration_days, g.start_date, g.deadline, rl.name FROM reading_goals g JOIN reading_list rl ON g.reading_list_id = rl.id WHERE g.id = $1",
            goal_id
        );

        if (goal_info.empty()) {
            std::cout << "No such goal found." << std::endl;
            return;
        }

        auto row = goal_info[0];
        std::string list_name = row["name"].as<std::string>();
        std::string deadline = row["deadline"].as<std::string>();

        std::cout << "\nGoal for reading list: " << list_name << "\nDeadline: " << deadline << std::endl;

        auto progress = w.exec_params(
            "SELECT COUNT(*) FILTER (WHERE is_completed) AS done, COUNT(*) AS total FROM goal_progress WHERE goal_id = $1",
            goal_id
        );

        int done = progress[0]["done"].as<int>();
        int total = progress[0]["total"].as<int>();
        double percent = total == 0 ? 0.0 : (100.0 * done / total);

        std::cout << "Progress: " << done << " / " << total << " books completed (" << std::fixed << std::setprecision(1) << percent << "%)" << std::endl;

        auto remaining_days_query = w.exec_params(
            "SELECT EXTRACT(DAY FROM deadline - CURRENT_TIMESTAMP) AS days_remaining FROM reading_goals WHERE id = $1",
            goal_id
        );

        int days_left = remaining_days_query[0]["days_remaining"].as<int>();
        int books_left = total - done;

        if (books_left > days_left) {
            std::cout << "Alert: You're falling behind! " << books_left << " books left and only " << days_left << " days remaining." << std::endl;
        }

    } catch (const std::exception& e) {
        throw std::runtime_error("dbManager: Failed to show goal progress: " + std::string(e.what()));
    }
}

std::string DatabaseManager::loadConnectionString(const std::string& path) {
    std::ifstream file(path);
    std::unordered_map<std::string, std::string> env;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t equals = line.find('=');
        if (equals != std::string::npos) {
            std::string key = line.substr(0, equals);
            std::string value = line.substr(equals + 1);
            env[key] = value;
        }
    }

    std::string connection_string =
        "dbname=" + env["DB_NAME"] +
        " user=" + env["DB_USER"] +
        " password=" + env["DB_PASS"] +
        " hostaddr=" + env["DB_HOST"] +
        " port=" + env["DB_PORT"];

    return connection_string;
}