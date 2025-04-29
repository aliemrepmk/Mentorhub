#include "database/goalsManager.hpp"
#include "database/dbManager.hpp"
#include <pqxx/pqxx>
#include <iostream>

std::vector<Goal> GoalsManager::getGoalsForList(int readingListId) {
    std::vector<Goal> goals;
    try {
        auto& conn = DatabaseManager::getInstance().getConnection();
        pqxx::work w(conn);

        auto result = w.exec_params(
            "SELECT id, start_date, deadline, duration_days FROM reading_goals WHERE reading_list_id = $1",
            readingListId
        );

        for (const auto& row : result) {
            goals.push_back({
                row["id"].as<int>(),
                row["start_date"].as<std::string>(),
                row["deadline"].as<std::string>(),
                row["duration_days"].as<int>()
            });
        }

    } catch (const std::exception& e) {
        std::cerr << "Error loading goals: " << e.what() << std::endl;
    }

    return goals;
}

bool GoalsManager::addGoal(int userId, int readingListId, const QDate& startDate, const QDate& deadline, int durationDays) {
    try {
        auto& conn = DatabaseManager::getInstance().getConnection();
        pqxx::work w(conn);

        w.exec_params(
            "INSERT INTO reading_goals (user_id, reading_list_id, start_date, deadline, duration_days) "
            "VALUES ($1, $2, $3, $4, $5)",
            userId,
            readingListId,
            startDate.toString("yyyy-MM-dd").toStdString(),
            deadline.toString("yyyy-MM-dd").toStdString(),
            durationDays
        );

        w.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error adding goal: " << e.what() << std::endl;
        return false;
    }
}

bool GoalsManager::deleteGoal(int goalId) {
    try {
        auto& conn = DatabaseManager::getInstance().getConnection();
        pqxx::work w(conn);

        w.exec_params("DELETE FROM reading_goals WHERE id = $1", goalId);
        w.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting goal: " << e.what() << std::endl;
        return false;
    }
}

bool GoalsManager::updateGoal(int goalId, const QDate& startDate, const QDate& deadline, int durationDays) {
    try {
        auto& conn = DatabaseManager::getInstance().getConnection();
        pqxx::work w(conn);

        w.exec_params(
            "UPDATE reading_goals SET start_date = $1, deadline = $2, duration_days = $3 WHERE id = $4",
            startDate.toString("yyyy-MM-dd").toStdString(),
            deadline.toString("yyyy-MM-dd").toStdString(),
            durationDays,
            goalId
        );

        w.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error updating goal: " << e.what() << std::endl;
        return false;
    }
}
