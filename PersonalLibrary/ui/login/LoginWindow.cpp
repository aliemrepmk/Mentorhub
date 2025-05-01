#include "LoginWindow.hpp"
#include <QMessageBox>

LoginWindow::LoginWindow(AuthenticateUser& authService, QWidget* parent)
    : QWidget(parent),
      ui(new Ui::LoginWindow),
      authenticateUser(authService)
{
    ui->setupUi(this);

    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);
}

LoginWindow::~LoginWindow() {
    delete ui;
}

void LoginWindow::handleLogin() {
    QString email = ui->emailInput->text();
    QString password = ui->passwordInput->text();

    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter both email and password.");
        return;
    }

    bool success = authenticateUser.execute(email.toStdString(), password.toStdString());
    if (success) {
        QMessageBox::information(this, "Login Success", "Welcome!");
        // Emit signal or switch view here
    } else {
        QMessageBox::critical(this, "Login Failed", "Invalid credentials.");
    }
}