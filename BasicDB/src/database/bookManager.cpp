#include "database/bookManager.hpp"
#include "database/dbManager.hpp"
#include <iostream>
#include <pqxx/pqxx>
#include <stdexcept>

using BookTuple = std::tuple<std::string, std::string, std::string, std::string>;

std::vector<BookTuple> BookManager::getAllBooks() {
    auto& conn = DatabaseManager::getInstance().getConnection();
    std::vector<BookTuple> books;

    pqxx::work w(conn);
    auto result = w.exec("SELECT book_name, author_name, publish_date, isbn FROM books");

    for (const auto& row : result) {
        books.emplace_back(
            row["book_name"].as<std::string>(),
            row["author_name"].as<std::string>(),
            row["publish_date"].as<std::string>(),
            row["isbn"].as<std::string>()
        );
    }

    return books;
}

std::vector<BookTuple> BookManager::searchBooksByName(const std::string& name) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    std::vector<BookTuple> books;

    pqxx::work w(conn);
    auto result = w.exec_params("SELECT book_name, author_name, publish_date, isbn FROM books WHERE book_name ILIKE $1", "%" + name + "%");

    for (const auto& row : result) {
        books.emplace_back(
            row["book_name"].as<std::string>(),
            row["author_name"].as<std::string>(),
            row["publish_date"].as<std::string>(),
            row["isbn"].as<std::string>()
        );
    }

    return books;
}

std::vector<BookTuple> BookManager::searchBooksByAuthor(const std::string& author) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    std::vector<BookTuple> books;

    pqxx::work w(conn);
    auto result = w.exec_params("SELECT book_name, author_name, publish_date, isbn FROM books WHERE author_name ILIKE $1", "%" + author + "%");

    for (const auto& row : result) {
        books.emplace_back(
            row["book_name"].as<std::string>(),
            row["author_name"].as<std::string>(),
            row["publish_date"].as<std::string>(),
            row["isbn"].as<std::string>()
        );
    }

    return books;
}

std::vector<BookTuple> BookManager::filterBooksByYear(int year) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    std::vector<BookTuple> books;

    pqxx::work w(conn);
    auto result = w.exec_params("SELECT book_name, author_name, publish_date, isbn FROM books WHERE EXTRACT(YEAR FROM publish_date)::int = $1", year);

    for (const auto& row : result) {
        books.emplace_back(
            row["book_name"].as<std::string>(),
            row["author_name"].as<std::string>(),
            row["publish_date"].as<std::string>(),
            row["isbn"].as<std::string>()
        );
    }

    return books;
}

BookTuple BookManager::getBookByISBN(const std::string& isbn) {
    auto& conn = DatabaseManager::getInstance().getConnection();

    pqxx::work w(conn);
    auto result = w.exec_params("SELECT book_name, author_name, publish_date, isbn FROM books WHERE isbn = $1", isbn);

    if (result.empty()) {
        throw std::runtime_error("Book not found.");
    }

    const auto& row = result[0];
    return {
        row["book_name"].as<std::string>(),
        row["author_name"].as<std::string>(),
        row["publish_date"].as<std::string>(),
        row["isbn"].as<std::string>()
    };
}

bool BookManager::addBook(const std::string& name, const std::string& author, const std::string& publishDate, const std::string& isbn) {
    auto& conn = DatabaseManager::getInstance().getConnection();

    try {
        pqxx::work w(conn);

        // Check if the book already exists by ISBN
        auto check = w.exec_params("SELECT isbn FROM books WHERE isbn = $1", isbn);
        if (!check.empty()) {
            std::cerr << "Book with ISBN already exists.\n";
            return false;
        }

        w.exec_params(
            "INSERT INTO books (book_name, author_name, publish_date, isbn) VALUES ($1, $2, $3, $4)",
            name, author, publishDate, isbn
        );

        w.commit();
        std::cout << "Book added successfully.\n";
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Failed to add book: " << e.what() << "\n";
        return false;
    }
}

std::vector<std::tuple<std::string, std::string, std::string, std::string>> BookManager::filterBooksBetweenYears(int startYear, int endYear) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> books;

    try {
        pqxx::work w(conn);
        auto result = w.exec_params(
            "SELECT book_name, author_name, publish_date, isbn "
            "FROM books "
            "WHERE EXTRACT(YEAR FROM publish_date)::int BETWEEN $1 AND $2",
            startYear, endYear
        );

        for (const auto& row : result) {
            books.emplace_back(
                row["book_name"].as<std::string>(),
                row["author_name"].as<std::string>(),
                row["publish_date"].as<std::string>(),
                row["isbn"].as<std::string>()
            );
        }

    } catch (const std::exception& e) {
        std::cerr << "Error filtering books by year range: " << e.what() << std::endl;
    }

    return books;
}

std::vector<std::tuple<std::string, std::string, std::string, std::string>>
BookManager::filterBooks(const std::string& author, const std::string& isbn, int startYear, int endYear) {
    auto& conn = DatabaseManager::getInstance().getConnection();
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> books;

    try {
        pqxx::work w(conn);
        pqxx::result result;

        if (!author.empty() && !isbn.empty()) {
            std::string query =
                "SELECT book_name, author_name, publish_date, isbn "
                "FROM books "
                "WHERE author_name ILIKE $1 AND isbn = $2 "
                "AND EXTRACT(YEAR FROM publish_date)::int BETWEEN $3 AND $4";

            result = w.exec_params(query, "%" + author + "%", isbn, startYear, endYear);
        }
        else if (!author.empty()) {
            std::string query =
                "SELECT book_name, author_name, publish_date, isbn "
                "FROM books "
                "WHERE author_name ILIKE $1 "
                "AND EXTRACT(YEAR FROM publish_date)::int BETWEEN $2 AND $3";

            result = w.exec_params(query, "%" + author + "%", startYear, endYear);
        }
        else if (!isbn.empty()) {
            std::string query =
                "SELECT book_name, author_name, publish_date, isbn "
                "FROM books "
                "WHERE isbn = $1 "
                "AND EXTRACT(YEAR FROM publish_date)::int BETWEEN $2 AND $3";

            result = w.exec_params(query, isbn, startYear, endYear);
        }
        else {
            std::string query =
                "SELECT book_name, author_name, publish_date, isbn "
                "FROM books "
                "WHERE EXTRACT(YEAR FROM publish_date)::int BETWEEN $1 AND $2";

            result = w.exec_params(query, startYear, endYear);
        }

        for (const auto& row : result) {
            books.emplace_back(
                row["book_name"].as<std::string>(),
                row["author_name"].as<std::string>(),
                row["publish_date"].as<std::string>(),
                row["isbn"].as<std::string>()
            );
        }

    } catch (const std::exception& e) {
        std::cerr << "filterBooks error: " << e.what() << std::endl;
    }

    return books;
}