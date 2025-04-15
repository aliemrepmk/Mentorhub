#include "readingListDetailsWindow.hpp"
#include "ui_ReadingListDetailsWindow.h"

#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>

#include "addBookDialog.hpp"
#include "database/dbManager.hpp"
#include <pqxx/pqxx>

#include "goalsWindow.hpp"

#include <sw/redis++/redis++.h>
using namespace sw::redis;

ReadingListDetailsWindow::ReadingListDetailsWindow(int userId, int listId, const QString &listName, QWidget *parent)
    : QWidget(parent), ui(new Ui::ReadingListDetailsWindow), m_userId(userId), m_listId(listId), m_listName(listName) {
    ui->setupUi(this);

    ui->titleLabel->setText("📚 Reading List: " + listName);
    ui->bookTable->setColumnCount(2);
    ui->bookTable->setHorizontalHeaderLabels(QStringList() << "Book Title" << "Author");
    ui->bookTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->bookTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->addBookButton, &QPushButton::clicked, this, &ReadingListDetailsWindow::onAddBookClicked);
    connect(ui->removeBookButton, &QPushButton::clicked, this, &ReadingListDetailsWindow::onRemoveBookClicked);
    connect(ui->showGoalsButton, &QPushButton::clicked, this, &ReadingListDetailsWindow::onShowGoalsClicked);
    connect(ui->deleteListButton, &QPushButton::clicked, this, &ReadingListDetailsWindow::onDeleteListClicked);

    loadBooks();
}

ReadingListDetailsWindow::~ReadingListDetailsWindow() {
    delete ui;
}

void ReadingListDetailsWindow::loadBooks() {
    try {
        auto &conn = DatabaseManager::getInstance().getConnection();
        pqxx::work w(conn);

        auto result = w.exec_params(
            "SELECT b.book_name, b.author_name, b.isbn "
            "FROM reading_list_books rlb "
            "JOIN books b ON rlb.book_id = b.id "
            "WHERE rlb.reading_list_id = $1",
            m_listId
        );


        ui->bookTable->setRowCount(static_cast<int>(result.size()));

        int row = 0;
        for (const auto &r : result) {
            QString title = QString::fromStdString(r["book_name"].c_str());
            QString author = QString::fromStdString(r["author_name"].c_str());
            QString isbn = QString::fromStdString(r["isbn"].c_str());

            auto *titleItem = new QTableWidgetItem(title);
            titleItem->setData(Qt::UserRole, isbn);  // Store ISBN for later use
            ui->bookTable->setItem(row, 0, titleItem);
            ui->bookTable->setItem(row, 1, new QTableWidgetItem(author));
            ++row;
        }

    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Error Loading Books", e.what());
    }
}

void ReadingListDetailsWindow::onAddBookClicked() {
    AddBookDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString isbn = dialog.getISBN();

        try {
            auto &conn = DatabaseManager::getInstance().getConnection();
            pqxx::work w(conn);

            // 1. Find book by ISBN
            auto result = w.exec_params(
                "SELECT id FROM books WHERE isbn = $1",
                isbn.toStdString()
            );

            if (result.empty()) {
                QMessageBox::warning(this, "Not Found", "No book found with that ISBN.");
                return;
            }

            int bookId = result[0]["id"].as<int>();

            // 2. Insert into reading_list_books
            w.exec_params(
                "INSERT INTO reading_list_books (reading_list_id, book_id) VALUES ($1, $2) "
                "ON CONFLICT DO NOTHING",  // prevent duplicates
                m_listId, bookId
            );

            w.commit();
            loadBooks();  // refresh table

        } catch (const std::exception &e) {
            QMessageBox::critical(this, "Error", e.what());
        }
    }
}

void ReadingListDetailsWindow::onRemoveBookClicked() {
    int row = ui->bookTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a book to remove.");
        return;
    }

    auto *item = ui->bookTable->item(row, 0);  // title column
    QString bookTitle = item->text();
    QString isbn = item->data(Qt::UserRole).toString();  // Retrieve hidden ISBN

    auto confirm = QMessageBox::question(
        this,
        "Confirm Removal",
        QString("Are you sure you want to remove \"%1\" from this reading list?").arg(bookTitle),
        QMessageBox::Yes | QMessageBox::No
    );

    if (confirm != QMessageBox::Yes)
        return;

    try {
        auto &conn = DatabaseManager::getInstance().getConnection();
        pqxx::work w(conn);

        // Get the book_id from ISBN
        auto res = w.exec_params("SELECT id FROM books WHERE isbn = $1", isbn.toStdString());
        if (res.empty()) {
            QMessageBox::warning(this, "Error", "Book not found in database.");
            return;
        }

        int bookId = res[0]["id"].as<int>();

        // Delete from reading_list_books
        w.exec_params(
            "DELETE FROM reading_list_books WHERE reading_list_id = $1 AND book_id = $2",
            m_listId, bookId
        );

        w.commit();
        loadBooks();  // Refresh the table

    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Error Removing Book", e.what());
    }
}

void ReadingListDetailsWindow::onShowGoalsClicked() {
    auto *goalsWin = new GoalsWindow(m_userId, m_listId);
    goalsWin->setAttribute(Qt::WA_DeleteOnClose);
    goalsWin->setWindowFlag(Qt::Window);
    goalsWin->show();
}

void ReadingListDetailsWindow::onDeleteListClicked() {
    auto confirm = QMessageBox::question(
        this,
        "Delete Reading List",
        QString("Are you sure you want to permanently delete the reading list \"%1\"?\n\nThis will also remove all related books and goals.").arg(m_listName),
        QMessageBox::Yes | QMessageBox::No
    );

    if (confirm != QMessageBox::Yes)
        return;

    try {
        auto &conn = DatabaseManager::getInstance().getConnection();
        pqxx::work w(conn);

        // Delete the reading list
        w.exec_params("DELETE FROM reading_list WHERE id = $1", m_listId);
        w.commit();

        // Invalidate Redis cache
        Redis redis("tcp://127.0.0.1:6379");
        redis.del("reading_lists:" + std::to_string(m_userId));

        // Close this window
        QMessageBox::information(this, "Deleted", "The reading list has been deleted.");
        emit readingListDeleted();
        close();

    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Error Deleting List", e.what());
    }
}