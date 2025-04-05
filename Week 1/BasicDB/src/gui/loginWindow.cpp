#include "loginWindow.hpp"
#include "ui_LoginWindow.h"

#include "database/dbManager.hpp"

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

    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);
}

LoginWindow::~LoginWindow() {
    delete ui;
}
void LoginWindow::handleLogin() {
    QString email = ui->emailInput->text().trimmed();
    if (email.isEmpty()) {
        ui->statusLabel->setText("❗ Please enter your email.");
        return;
    }
}
