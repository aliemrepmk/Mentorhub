#ifndef GOALSMANAGER_HPP
#define GOALSMANAGER_HPP

#include <vector>
#include <string>
#include <tuple>
#include <QDate>

struct Goal {
    int id;
    std::string startDate;
    std::string deadline;
    int durationDays;
};

class GoalsManager {
public:
    static std::vector<Goal> getGoalsForList(int readingListId);
    static bool addGoal(int userId, int readingListId, const QDate& startDate, const QDate& deadline, int durationDays);
    static bool deleteGoal(int goalId);
    static bool updateGoal(int goalId, const QDate& startDate, const QDate& deadline, int durationDays);
};

#endif // GOALSMANAGER_HPP
