#ifndef GOALSWINDOW_HPP
#define GOALSWINDOW_HPP

#include <QWidget>

namespace Ui {
class GoalsWindow;
}

class GoalsWindow : public QWidget {
    Q_OBJECT

public:
    explicit GoalsWindow(int readingListId, QWidget *parent = nullptr);
    ~GoalsWindow();

private:
    Ui::GoalsWindow *ui;
    int m_readingListId;

    void loadGoals();
};

#endif // GOALSWINDOW_HPP
