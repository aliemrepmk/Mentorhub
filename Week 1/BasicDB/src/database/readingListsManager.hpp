#pragma once

#include <string>
#include <vector>
#include <tuple>

class ReadingListsManager {
public:
    using ReadingListTuple = std::tuple<int, std::string, std::string, int>;
    using BookTuple = std::tuple<std::string, std::string, std::string>;

    static std::vector<ReadingListTuple> getReadingListsByUser(int userId);
    static bool addReadingList(int userId, const std::string& name);
    static bool deleteReadingList(int listId, int userId);
    static std::vector<BookTuple> getBooksInList(int listId);
    static bool addBookToList(int listId, const std::string& isbn);
    static bool removeBookFromList(int listId, const std::string& isbn);
}; 