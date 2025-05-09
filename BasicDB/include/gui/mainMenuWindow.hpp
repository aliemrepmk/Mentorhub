#pragma once

#include <QWidget>

#include "booksWindow.hpp"
#include "readingListsWindow.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainMenuWindow; }
QT_END_NAMESPACE

class MainMenuWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainMenuWindow(QWidget *parent = nullptr);
    explicit MainMenuWindow(int userId, QWidget *parent = nullptr);
    ~MainMenuWindow();

private slots:
    void handleLogout();

private:
    Ui::MainMenuWindow *ui;
    int m_userId;
};