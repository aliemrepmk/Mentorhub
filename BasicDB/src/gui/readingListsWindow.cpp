#include "readingListsWindow.hpp"
#include "ui_ReadingListsWindow.h"
#include "readingListDetailsWindow.hpp"
#include "database/readingListsManager.hpp"
#include <iostream>
#include <QTableWidgetItem>
#include <QString>
#include <QInputDialog>
#include <QLineEdit>

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
        auto lists = ReadingListsManager::getReadingListsByUser(m_userId);
        
        // Populate the UI table
        ui->readingListTable->setRowCount(static_cast<int>(lists.size()));

        int row = 0;
        for (const auto& list : lists) {
            auto *titleItem = new QTableWidgetItem(QString::fromStdString(std::get<1>(list)));
            titleItem->setData(Qt::UserRole, std::get<0>(list));

            ui->readingListTable->setItem(row, 0, titleItem);
            ui->readingListTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(std::get<2>(list))));
            ui->readingListTable->setItem(row, 2, new QTableWidgetItem(QString::number(std::get<3>(list))));
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
            if (ReadingListsManager::addReadingList(m_userId, listName.toStdString())) {
                loadReadingLists();  // Refresh UI
            }
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