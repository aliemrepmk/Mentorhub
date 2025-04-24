#include "database/readingListsManager.hpp"
#include "database/dbManager.hpp"
#include <pqxx/pqxx>
#include <iostream>
#include <sw/redis++/redis++.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace sw::redis;

std::vector<ReadingListsManager::ReadingListTuple> ReadingListsManager::getReadingListsByUser(int userId) {
    try {
        Redis redis("tcp://127.0.0.1:6379");
        std::string cacheKey = "reading_lists:" + std::to_string(userId);
        auto cached = redis.get(cacheKey);

        if (cached) {
            // Cache hit
            std::cout << "[Cache hit] - Loading reading lists directly from the redis" << std::endl;
            json readingLists = json::parse(*cached);
            std::vector<ReadingListTuple> result;
            for (const auto& item : readingLists) {
                result.emplace_back(
                    item["id"].get<int>(),
                    item["name"].get<std::string>(),
                    item["created_at"].get<std::string>(),
                    item["book_count"].get<int>()
                );
            }
            return result;
        }

        // Cache miss: load from database
        std::cout << "[Cache miss] - Loading reading lists from database to the redis" << std::endl;
        auto& conn = DatabaseManager::getInstance().getConnection();
        pqxx::work w(conn);

        std::vector<ReadingListTuple> readingLists;
        auto result = w.exec_params(
            "SELECT rl.id, rl.name, rl.created_at, COUNT(rlb.book_id) AS book_count "
            "FROM reading_list rl "
            "LEFT JOIN reading_list_books rlb ON rl.id = rlb.reading_list_id "
            "WHERE rl.user_id = $1 "
            "GROUP BY rl.id, rl.name, rl.created_at",
            userId
        );

        for (const auto& row : result) {
            readingLists.emplace_back(
                row["id"].as<int>(),
                row["name"].c_str(),
                row["created_at"].c_str(),
                row["book_count"].as<int>()
            );
        }

        // Cache the result in Redis
        json cacheData;
        for (const auto& list : readingLists) {
            cacheData.push_back({
                {"id", std::get<0>(list)},
                {"name", std::get<1>(list)},
                {"created_at", std::get<2>(list)},
                {"book_count", std::get<3>(list)}
            });
        }
        redis.setex(cacheKey, 60, cacheData.dump());

        return readingLists;
    } catch (const std::exception& e) {
        std::cerr << "Error in getReadingListsByUser: " << e.what() << std::endl;
        throw;
    }
}

bool ReadingListsManager::addReadingList(int userId, const std::string& name) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    try {
        pqxx::work w(conn);
        w.exec_params("INSERT INTO reading_list (user_id, name) VALUES ($1, $2)", userId, name);
        w.commit();

        // Invalidate cache
        Redis redis("tcp://127.0.0.1:6379");
        redis.del("reading_lists:" + std::to_string(userId));
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to add reading list: " << e.what() << "\n";
        return false;
    }
}

bool ReadingListsManager::deleteReadingList(int listId, int userId) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    try {
        pqxx::work w(conn);

        // Delete the reading list
        w.exec_params("DELETE FROM reading_list WHERE id = $1", listId);
        w.commit();

        // Invalidate Redis cache
        Redis redis("tcp://127.0.0.1:6379");
        redis.del("reading_lists:" + std::to_string(userId));

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting reading list: " << e.what() << std::endl;
        return false;
    }
}

std::vector<ReadingListsManager::BookTuple> ReadingListsManager::getBooksInList(int listId) {
    try {
        auto& conn = DatabaseManager::getInstance().getConnection();
        pqxx::work w(conn);

        std::vector<BookTuple> books;
        auto result = w.exec_params(
            "SELECT b.book_name, b.author_name, b.isbn "
            "FROM reading_list_books rlb "
            "JOIN books b ON rlb.book_id = b.id "
            "WHERE rlb.reading_list_id = $1",
            listId
        );

        for (const auto& row : result) {
            books.emplace_back(
                row["book_name"].c_str(),
                row["author_name"].c_str(),
                row["isbn"].c_str()
            );
        }

        return books;
    } catch (const std::exception& e) {
        std::cerr << "Error in getBooksInList: " << e.what() << std::endl;
        throw;
    }
}

bool ReadingListsManager::addBookToList(int listId, const std::string& isbn) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    try {
        pqxx::work w(conn);

        // 1. Find book by ISBN
        auto result = w.exec_params("SELECT id FROM books WHERE isbn = $1", isbn);
        if (result.empty()) return false;

        int bookId = result[0]["id"].as<int>();

        // 2. Insert into reading_list_books
        w.exec_params(
            "INSERT INTO reading_list_books (reading_list_id, book_id) VALUES ($1, $2) "
            "ON CONFLICT DO NOTHING",  // prevent duplicates
            listId, bookId
        );

        w.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error adding book to list: " << e.what() << std::endl;
        return false;
    }
}

bool ReadingListsManager::removeBookFromList(int listId, const std::string& isbn) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    try {
        pqxx::work w(conn);

        // Get the book_id from ISBN
        auto res = w.exec_params("SELECT id FROM books WHERE isbn = $1", isbn);
        if (res.empty()) return false;

        int bookId = res[0]["id"].as<int>();

        // Delete from reading_list_books
        w.exec_params(
            "DELETE FROM reading_list_books WHERE reading_list_id = $1 AND book_id = $2",
            listId, bookId
        );

        w.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error removing book from list: " << e.what() << std::endl;
        return false;
    }
}
