#include "readingListsWindow.hpp"
#include "ui_ReadingListsWindow.h"
#include "database/dbManager.hpp"

#include <pqxx/pqxx>
#include <QTableWidgetItem>
#include <QString>

ReadingListsWindow::ReadingListsWindow(int userId, QWidget *parent)
    : QWidget(parent), ui(new Ui::ReadingListsWindow), m_userId(userId) {
    ui->setupUi(this);

    ui->readingListTable->setColumnCount(3);
    QStringList headers = { "Title", "Created At", "# of Books" };
    ui->readingListTable->setHorizontalHeaderLabels(headers);
    ui->readingListTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    loadReadingLists();
}

ReadingListsWindow::~ReadingListsWindow() {
    delete ui;
}

void ReadingListsWindow::loadReadingLists() {
    auto& conn = DatabaseManager::getInstance().getConnection();

    try {
        pqxx::work w(conn);
        auto result = w.exec_params(
            "SELECT rl.name, rl.created_at, COUNT(rlb.book_id) AS book_count "
            "FROM reading_list rl "
            "LEFT JOIN reading_list_books rlb ON rl.id = rlb.reading_list_id "
            "WHERE rl.user_id = $1 "
            "GROUP BY rl.id, rl.name, rl.created_at",
            m_userId
        );

        ui->readingListTable->setRowCount(static_cast<int>(result.size()));

        int row = 0;
        for (const auto& r : result) {
            ui->readingListTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(r["name"].as<std::string>())));
            ui->readingListTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r["created_at"].as<std::string>())));
            ui->readingListTable->setItem(row, 2, new QTableWidgetItem(QString::number(r["book_count"].as<int>())));
            row++;
        }

    } catch (const std::exception& e) {
        qWarning("Failed to load reading lists: %s", e.what());
    }
}