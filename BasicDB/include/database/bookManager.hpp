#pragma once

#include <string>
#include <vector>
#include <tuple>

class BookManager {
public:
    static std::vector<std::tuple<std::string, std::string, std::string, std::string>> getAllBooks();
    static std::vector<std::tuple<std::string, std::string, std::string, std::string>> searchBooksByName(const std::string& name);
    static std::vector<std::tuple<std::string, std::string, std::string, std::string>> searchBooksByAuthor(const std::string& author);
    static std::vector<std::tuple<std::string, std::string, std::string, std::string>> filterBooksByYear(int year);
    static std::tuple<std::string, std::string, std::string, std::string> getBookByISBN(const std::string& isbn);
    static bool addBook(const std::string& name, const std::string& author, const std::string& publishDate, const std::string& isbn);
    static std::vector<std::tuple<std::string, std::string, std::string, std::string>> filterBooksBetweenYears(int startYear, int endYear);
    static std::vector<std::tuple<std::string, std::string, std::string, std::string>> filterBooks(const std::string& author, const std::string& isbn, int startYear, int endYear);
};