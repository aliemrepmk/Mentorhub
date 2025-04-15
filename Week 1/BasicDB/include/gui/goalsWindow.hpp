#ifndef GOALSWINDOW_HPP
#define GOALSWINDOW_HPP

#include <QWidget>

namespace Ui {
class GoalsWindow;
}

class GoalsWindow : public QWidget {
    Q_OBJECT

public:
    explicit GoalsWindow(int userId, int readingListId, QWidget *parent = nullptr);
    ~GoalsWindow();

private slots:
    void onAddGoalClicked();
    void onDeleteGoalClicked();
    void onUpdateGoalClicked();

private:
    void loadGoals();

    Ui::GoalsWindow *ui;
    int m_userId;
    int m_readingListId;
};

#endif // GOALSWINDOW_HPP