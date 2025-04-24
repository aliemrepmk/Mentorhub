#include "readingListDetailsWindow.hpp"
#include "ui_ReadingListDetailsWindow.h"

#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>

#include "addBookDialog.hpp"
#include "database/readingListsManager.hpp"
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
        auto books = ReadingListsManager::getBooksInList(m_listId);
        
        ui->bookTable->setRowCount(static_cast<int>(books.size()));

        int row = 0;
        for (const auto& book : books) {
            QString title = QString::fromStdString(std::get<0>(book));
            QString author = QString::fromStdString(std::get<1>(book));
            QString isbn = QString::fromStdString(std::get<2>(book));

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
            if (!ReadingListsManager::addBookToList(m_listId, isbn.toStdString())) {
                QMessageBox::warning(this, "Not Found", "No book found with that ISBN.");
                return;
            }
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
        if (!ReadingListsManager::removeBookFromList(m_listId, isbn.toStdString())) {
            QMessageBox::warning(this, "Error", "Book not found in database.");
            return;
        }
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
        if (ReadingListsManager::deleteReadingList(m_listId, m_userId)) {
            QMessageBox::information(this, "Deleted", "The reading list has been deleted.");
            emit readingListDeleted();
            close();
        } else {
            QMessageBox::critical(this, "Error Deleting List", "Failed to delete the reading list.");
        }
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Error Deleting List", e.what());
    }
}