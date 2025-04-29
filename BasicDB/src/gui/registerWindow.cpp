// registerWindow.cpp
#include "gui/registerWindow.hpp"
#include "ui_RegisterWindow.h"
#include "database/userManager.hpp"

#include <QMessageBox>
#include <QRegularExpression>

RegisterWindow::RegisterWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::RegisterWindow) {
    ui->setupUi(this);

    // Connect register button
    connect(ui->registerButton, &QPushButton::clicked, this, &RegisterWindow::handleRegister);
}

RegisterWindow::~RegisterWindow() {
    delete ui;
}

void RegisterWindow::handleRegister() {
    QString firstName = ui->firstNameInput->text().trimmed();
    QString lastName = ui->lastNameInput->text().trimmed();
    QString email = ui->emailInput->text().trimmed();
    QString password = ui->passwordInput->text();

    if (firstName.isEmpty() || lastName.isEmpty() || email.isEmpty() || password.isEmpty()) {
        ui->statusLabel->setText("All fields must be filled.");
        return;
    }

    // Basic email validation
    QRegularExpression emailRegex("(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$)");
    if (!emailRegex.match(email).hasMatch()) {
        ui->statusLabel->setText("Please enter a valid email address.");
        return;
    }

    try {
        bool success = UserManager::createUser(firstName.toStdString(), lastName.toStdString(), email.toStdString(), password.toStdString());
        if (success) {
            QMessageBox::information(this, "Success", "Account created successfully!");
            this->close();
        } else {
            ui->statusLabel->setText("This email is already used.");
        }
    } catch (const std::exception &e) {
        ui->statusLabel->setText(QString("Error: ") + e.what());
    }
}