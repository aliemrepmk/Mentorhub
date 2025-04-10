#include "mainMenuWindow.hpp"
#include "ui_mainMenuWindow.h"

#include "loginWindow.hpp"

#include <QMessageBox>

MainMenuWindow::MainMenuWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainMenuWindow) {
    ui->setupUi(this);

    connect(ui->logoutButton, &QPushButton::clicked, this, &MainMenuWindow::handleLogout);
    connect(ui->booksButton, &QPushButton::clicked, [this]() {
        auto* booksWin = new BooksWindow();
        booksWin->setAttribute(Qt::WA_DeleteOnClose);
        booksWin->show();
    });
}

MainMenuWindow::MainMenuWindow(int userId, QWidget *parent)
    : QWidget(parent), ui(new Ui::MainMenuWindow), m_userId(userId) {
    ui->setupUi(this);

    connect(ui->logoutButton, &QPushButton::clicked, this, &MainMenuWindow::handleLogout);

    connect(ui->booksButton, &QPushButton::clicked, [this]() {
        auto* booksWin = new BooksWindow();
        booksWin->setAttribute(Qt::WA_DeleteOnClose);
        booksWin->show();
    });

    // Connect reading list button
    connect(ui->readingListsButton, &QPushButton::clicked, [this]() {
        auto* rlWindow = new ReadingListsWindow(m_userId);
        rlWindow->setAttribute(Qt::WA_DeleteOnClose);
        rlWindow->show();
    });
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