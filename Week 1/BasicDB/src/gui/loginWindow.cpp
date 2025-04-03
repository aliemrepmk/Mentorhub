#include "loginWindow.hpp"
#include "ui_LoginWindow.h"

#include "database/dbManager.hpp"
#include "user.hpp"

#include <QScreen>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::LoginWindow) {
    ui->setupUi(this);

    // Place the GUI in the center of the screen
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    loadUsers();
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);
}

LoginWindow::~LoginWindow() {
    delete ui;
}

void LoginWindow::loadUsers() {
    try {
        pqxx::connection &conn = DatabaseManager::getInstance().getConnection();
        pqxx::work txn(conn);
        auto result = txn.exec("SELECT id, first_name, last_name, email FROM users");

        ui->userTable->setRowCount(result.size());
        ui->userTable->setColumnCount(3);
        ui->userTable->setHorizontalHeaderLabels({"ID", "Name", "Email"});

        int row = 0;
        for (const auto &user : result) {
            ui->userTable->setItem(row, 0, new QTableWidgetItem(QString::number(user["id"].as<int>())));
            QString name = QString::fromStdString(user["first_name"].as<std::string>() + " " + user["last_name"].as<std::string>());
            ui->userTable->setItem(row, 1, new QTableWidgetItem(name));
            ui->userTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(user["email"].as<std::string>())));
            ++row;
        }
        ui->userTable->resizeColumnsToContents();
    } catch (...) {
        ui->statusLabel->setText("⚠️ Failed to load users.");
    }
}

void LoginWindow::handleLogin() {
    QString email = ui->emailInput->text().trimmed();
    if (email.isEmpty()) {
        ui->statusLabel->setText("❗ Please enter your email.");
        return;
    }

    try {
        pqxx::connection &conn = DatabaseManager::getInstance().getConnection();
        auto userOpt = User::getUserByEmail(conn, email.toStdString());

        if (userOpt.has_value()) {
            const auto &user = userOpt.value();
            ui->statusLabel->setText(QString("✅ Welcome, %1!").arg(QString::fromStdString(user.getFirstName())));
            // Proceed to main app here
        } else {
            ui->statusLabel->setText("❌ No user found with that email.");
        }
    } catch (...) {
        ui->statusLabel->setText("⚠️ Login error.");
    }
}
