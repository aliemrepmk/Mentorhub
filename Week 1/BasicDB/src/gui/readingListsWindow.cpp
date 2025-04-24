#include "readingListsWindow.hpp"
#include "ui_ReadingListsWindow.h"
#include "readingListDetailsWindow.hpp"
#include "database/dbManager.hpp"
#include <iostream>
#include <pqxx/pqxx>
#include <QTableWidgetItem>
#include <QString>
#include <QInputDialog>
#include <QLineEdit>

#include <sw/redis++/redis++.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace sw::redis;

ReadingListsWindow::ReadingListsWindow(int userId, QWidget *parent)
    : QWidget(parent), ui(new Ui::ReadingListsWindow), m_userId(userId) {
    ui->setupUi(this);

    ui->readingListTable->setColumnCount(3);
    QStringList headers = { "Title", "Created At", "# of Books" };
    ui->readingListTable->setHorizontalHeaderLabels(headers);
    ui->readingListTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->readingListTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->addReadingListButton, &QPushButton::clicked, this, &ReadingListsWindow::onAddReadingListClicked);
    connect(ui->readingListTable, &QTableWidget::cellClicked, this, &ReadingListsWindow::onReadingListClicked);

    loadReadingLists();
}

ReadingListsWindow::~ReadingListsWindow() {
    delete ui;
}

void ReadingListsWindow::loadReadingLists() {
    try {
        Redis redis("tcp://127.0.0.1:6379");
        std::string cacheKey = "reading_lists:" + std::to_string(m_userId);
        auto cached = redis.get(cacheKey);

        json readingLists;

        if (cached) {
            // Cache hit
            readingLists = json::parse(*cached);
            std::cout << "[Cache hit] - Loading reading lists directly from the redis" << std::endl;
        } else {
            // Cache miss: load from PostgreSQL
            std::cout << "[Cache miss] - Loading reading lists from database to the redis" << std::endl;
            auto& conn = DatabaseManager::getInstance().getConnection();
            pqxx::work w(conn);

            auto result = w.exec(
                pqxx::zview(
                    "SELECT rl.id, rl.name, rl.created_at, COUNT(rlb.book_id) AS book_count "
                    "FROM reading_list rl "
                    "LEFT JOIN reading_list_books rlb ON rl.id = rlb.reading_list_id "
                    "WHERE rl.user_id = $1 "
                    "GROUP BY rl.id, rl.name, rl.created_at"
                ),
                pqxx::params{m_userId}
            );

            for (const auto& r : result) {
                readingLists.push_back({
                    {"id", r["id"].as<int>()},
                    {"name", r["name"].c_str()},
                    {"created_at", r["created_at"].c_str()},
                    {"book_count", r["book_count"].as<int>()}
                });
            }

            // Cache the result in Redis
            redis.setex(cacheKey, 60, readingLists.dump());
        }

        // Populate the UI table
        ui->readingListTable->setRowCount(static_cast<int>(readingLists.size()));

        int row = 0;
        for (const auto& item : readingLists) {
            auto *titleItem = new QTableWidgetItem(QString::fromStdString(item["name"]));
            titleItem->setData(Qt::UserRole, item["id"].get<int>());

            ui->readingListTable->setItem(row, 0, titleItem);
            ui->readingListTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(item["created_at"])));
            ui->readingListTable->setItem(row, 2, new QTableWidgetItem(QString::number(item["book_count"].get<int>())));
            ++row;
        }

    } catch (const std::exception& e) {
        qWarning("Failed to load reading lists: %s", e.what());
    }
}

void ReadingListsWindow::onAddReadingListClicked() {
    bool ok;
    QString listName = QInputDialog::getText(this, "New Reading List", "Enter reading list name:", QLineEdit::Normal, "", &ok);

    if (ok && !listName.trimmed().isEmpty()) {
        try {
            auto& conn = DatabaseManager::getInstance().getConnection();
            pqxx::work w(conn);

            w.exec(
                pqxx::zview("INSERT INTO reading_list (user_id, name) VALUES ($1, $2)"),
                pqxx::params{m_userId, listName.toStdString()}
            );

            w.commit();
            Redis redis("tcp://127.0.0.1:6379");
            redis.del("reading_lists:" + std::to_string(m_userId));
            loadReadingLists();  // Refresh UI

        } catch (const std::exception& e) {
            qWarning("Failed to add reading list: %s", e.what());
        }
    }
}

void ReadingListsWindow::onReadingListClicked(int row, int column) {
    if (column != 0) return;

    auto item = ui->readingListTable->item(row, column);
    if (!item) return;

    QString listName = item->text();
    int listId = item->data(Qt::UserRole).toInt();

    // Create and show the new details window
    auto *detailsWindow = new ReadingListDetailsWindow(m_userId, listId, listName);
    detailsWindow->setAttribute(Qt::WA_DeleteOnClose);
    detailsWindow->setWindowFlag(Qt::Window);
    connect(detailsWindow, &ReadingListDetailsWindow::readingListDeleted, this, &ReadingListsWindow::loadReadingLists);
    detailsWindow->show();
}