#include "loginWindow.hpp"
#include "ui_LoginWindow.h"

#include "database/userManager.hpp"

#include "registerWindow.hpp"
#include "mainMenuWindow.hpp"

#include <QScreen>
#include <QGuiApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDebug>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::LoginWindow) {
    ui->setupUi(this);

    // Center the window
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    // Connect buttons
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    connect(ui->createAccountButton, &QPushButton::clicked, this, &LoginWindow::handleCreateAccount);
}

LoginWindow::~LoginWindow() {
    delete ui;
}

void LoginWindow::handleLogin() {
    QString email = ui->emailInput->text().trimmed();
    QString password = ui->passwordInput->text();

    if (email.isEmpty() || password.isEmpty()) {
        ui->statusLabel->setText("Please enter both email and password.");
        return;
    }

    if (UserManager::verifyUser(email.toStdString(), password.toStdString())) {
        // Login successful
        ui->statusLabel->setText("Login successful. Redirecting...");

        // Open main menu
        int userId = UserManager::getUserId(email.toStdString());
        auto* menu = new MainMenuWindow(userId);
        menu->setAttribute(Qt::WA_DeleteOnClose);
        menu->show();
        this->close();
    } else {
        ui->statusLabel->setText("Invalid email or password.");
    }
}

void LoginWindow::handleCreateAccount() {
    auto *registerWin = new RegisterWindow();
    registerWin->setAttribute(Qt::WA_DeleteOnClose); // clean up when closed
    registerWin->show();
}