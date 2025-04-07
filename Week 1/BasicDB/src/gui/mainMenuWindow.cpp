#include "mainMenuWindow.hpp"
#include "ui_mainMenuWindow.h"

#include "loginWindow.hpp"

#include <QMessageBox>

MainMenuWindow::MainMenuWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainMenuWindow) {
    ui->setupUi(this);

    connect(ui->logoutButton, &QPushButton::clicked, this, &MainMenuWindow::handleLogout);
}

MainMenuWindow::~MainMenuWindow() {
    delete ui;
}

void MainMenuWindow::handleLogout() {
    this->close();
    auto* login = new LoginWindow();
    login->setAttribute(Qt::WA_DeleteOnClose);
    login->show();
}