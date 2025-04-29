#include "database/readingListsManager.hpp"
#include "database/dbManager.hpp"
#include <pqxx/pqxx>
#include <iostream>
#include <sw/redis++/redis++.h>
#include <unordered_map>
#include <unordered_set>

using namespace sw::redis;

std::vector<ReadingListsManager::ReadingListTuple> ReadingListsManager::getReadingListsByUser(int userId) {
    try {
        Redis redis("tcp://127.0.0.1:6379");
        std::string userKey = "user:" + std::to_string(userId) + ":reading_lists";

        std::vector<ReadingListTuple> readingLists;
        std::unordered_set<std::string> listIds;
        redis.smembers(userKey, std::inserter(listIds, listIds.begin()));

        if (!listIds.empty()) {
            std::cout << "[Cache hit] - Fetching reading list details from Redis hashes" << std::endl;
            for (const auto& listId : listIds) {
                auto hashKey = "reading_list:" + listId;
                std::unordered_map<std::string, std::string> data;
                redis.hgetall(hashKey, std::inserter(data, data.begin()));
                if (!data.empty()) {
                    readingLists.emplace_back(
                        std::stoi(data["id"]),
                        data["name"],
                        data["created_at"],
                        std::stoi(data["book_count"])
                    );
                }
            }
            return readingLists;
        }

        std::cout << "[Cache miss] - Loading from DB and populating Redis" << std::endl;
        auto& conn = DatabaseManager::getInstance().getConnection();
        pqxx::work w(conn);

        auto result = w.exec_params(
            "SELECT rl.id, rl.name, rl.created_at, COUNT(rlb.book_id) AS book_count "
            "FROM reading_list rl "
            "LEFT JOIN reading_list_books rlb ON rl.id = rlb.reading_list_id "
            "WHERE rl.user_id = $1 "
            "GROUP BY rl.id, rl.name, rl.created_at",
            userId
        );

        for (const auto& row : result) {
            int id = row["id"].as<int>();
            std::string name = row["name"].c_str();
            std::string createdAt = row["created_at"].c_str();
            int bookCount = row["book_count"].as<int>();

            readingLists.emplace_back(id, name, createdAt, bookCount);

            std::string hashKey = "reading_list:" + std::to_string(id);
            std::unordered_map<std::string, std::string> fields = {
                {"id", std::to_string(id)},
                {"name", name},
                {"created_at", createdAt},
                {"book_count", std::to_string(bookCount)}
            };
            redis.hmset(hashKey, fields.begin(), fields.end());
            redis.sadd(userKey, std::to_string(id));
        }

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

        Redis redis("tcp://127.0.0.1:6379");
        redis.del("user:" + std::to_string(userId) + ":reading_lists");

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
        w.exec_params("DELETE FROM reading_list WHERE id = $1", listId);
        w.commit();

        Redis redis("tcp://127.0.0.1:6379");
        redis.del("reading_list:" + std::to_string(listId));
        redis.srem("user:" + std::to_string(userId) + ":reading_lists", std::to_string(listId));

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

        auto result = w.exec_params("SELECT id FROM books WHERE isbn = $1", isbn);
        if (result.empty()) return false;

        int bookId = result[0]["id"].as<int>();
        w.exec_params(
            "INSERT INTO reading_list_books (reading_list_id, book_id) VALUES ($1, $2) "
            "ON CONFLICT DO NOTHING",
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

        auto res = w.exec_params("SELECT id FROM books WHERE isbn = $1", isbn);
        if (res.empty()) return false;

        int bookId = res[0]["id"].as<int>();
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