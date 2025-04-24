#include "database/readingListsManager.hpp"
#include "database/dbManager.hpp"
#include <pqxx/pqxx>
#include <iostream>

std::vector<ReadingListsManager::ReadingListTuple> ReadingListsManager::getReadingListsByUser(int userId) {
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

    return readingLists;
}

bool ReadingListsManager::addReadingList(int userId, const std::string& name) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    try {
        pqxx::work w(conn);
        w.exec_params("INSERT INTO reading_list (user_id, name) VALUES ($1, $2)", userId, name);
        w.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to add reading list: " << e.what() << "\n";
        return false;
    }
}

bool ReadingListsManager::deleteReadingList(int listId) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    try {
        pqxx::work w(conn);
        w.exec_params("DELETE FROM reading_list WHERE id = $1", listId);
        w.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to delete reading list: " << e.what() << "\n";
        return false;
    }
}

std::vector<ReadingListsManager::BookTuple> ReadingListsManager::getBooksInList(int listId) {
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
}

bool ReadingListsManager::addBookToList(int listId, const std::string& isbn) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    try {
        pqxx::work w(conn);

        auto res = w.exec_params("SELECT id FROM books WHERE isbn = $1", isbn);
        if (res.empty()) return false;

        int bookId = res[0]["id"].as<int>();
        w.exec_params(
            "INSERT INTO reading_list_books (reading_list_id, book_id) VALUES ($1, $2) ON CONFLICT DO NOTHING",
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
        w.exec_params("DELETE FROM reading_list_books WHERE reading_list_id = $1 AND book_id = $2", listId, bookId);
        w.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error removing book from list: " << e.what() << std::endl;
        return false;
    }
}
