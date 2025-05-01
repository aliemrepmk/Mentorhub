#pragma once

#include <QWidget>
#include <QString>
#include "ui_LoginWindow.h"
#include "user/AuthenticateUser.hpp"

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(AuthenticateUser& authService, QWidget* parent = nullptr);
    ~LoginWindow();

private slots:
    void handleLogin();

private:
    Ui::LoginWindow* ui;
    AuthenticateUser& authenticateUser;
};