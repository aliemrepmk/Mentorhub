#include "goalsWindow.hpp"
#include "ui_GoalsWindow.h"
#include "database/dbManager.hpp"
#include "addGoalDialog.hpp"

#include <pqxx/pqxx>
#include <QTableWidgetItem>
#include <QMessageBox>

GoalsWindow::GoalsWindow(int userId, int readingListId, QWidget *parent)
    : QWidget(parent), ui(new Ui::GoalsWindow),
      m_userId(userId), m_readingListId(readingListId) {

    ui->setupUi(this);
    setWindowTitle("📈 Reading Goals");

    ui->goalTable->setColumnCount(3);
    ui->goalTable->setHorizontalHeaderLabels(QStringList() << "Start Date" << "Deadline" << "Duration (Days)");
    ui->goalTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->goalTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->closeButton, &QPushButton::clicked, this, &GoalsWindow::close);
    connect(ui->addGoalButton, &QPushButton::clicked, this, &GoalsWindow::onAddGoalClicked);
    connect(ui->deleteGoalButton, &QPushButton::clicked, this, &GoalsWindow::onDeleteGoalClicked);
    connect(ui->updateGoalButton, &QPushButton::clicked, this, &GoalsWindow::onUpdateGoalClicked);

    loadGoals();
}

GoalsWindow::~GoalsWindow() {
    delete ui;
}

void GoalsWindow::loadGoals() {
    try {
        auto &conn = DatabaseManager::getInstance().getConnection();
        pqxx::work w(conn);

        auto result = w.exec_params(
            "SELECT id, start_date, deadline, duration_days FROM reading_goals WHERE reading_list_id = $1",
            m_readingListId
        );

        if (result.empty()) {
            QMessageBox::information(this, "No Goals Found", "There are no goals in this reading list.");
            ui->goalTable->setRowCount(0);
            return;
        }

        ui->goalTable->setRowCount(result.size());

        int row = 0;
        for (const auto &r : result) {
            QTableWidgetItem *startItem = new QTableWidgetItem(QString::fromStdString(r["start_date"].c_str()));
            startItem->setData(Qt::UserRole, r["id"].as<int>()); // Store goal ID in hidden data

            ui->goalTable->setItem(row, 0, startItem);
            ui->goalTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r["deadline"].c_str())));
            ui->goalTable->setItem(row, 2, new QTableWidgetItem(QString::number(r["duration_days"].as<int>())));
            ++row;
        }

    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Error Loading Goals", e.what());
    }
}

void GoalsWindow::onAddGoalClicked() {
    AddGoalDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        try {
            auto &conn = DatabaseManager::getInstance().getConnection();
            pqxx::work w(conn);

            w.exec_params(
                "INSERT INTO reading_goals (user_id, reading_list_id, start_date, deadline, duration_days) "
                "VALUES ($1, $2, $3, $4, $5)",
                m_userId,
                m_readingListId,
                dialog.getStartDate().toString("yyyy-MM-dd").toStdString(),
                dialog.getDeadline().toString("yyyy-MM-dd").toStdString(),
                dialog.getDuration()
            );

            w.commit();
            loadGoals(); // Refresh table

        } catch (const std::exception &e) {
            QMessageBox::critical(this, "Add Goal Error", e.what());
        }
    }
}

void GoalsWindow::onDeleteGoalClicked() {
    int currentRow = ui->goalTable->currentRow();

    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a goal to delete.");
        return;
    }

    auto item = ui->goalTable->item(currentRow, 0); // Get item from first column
    int goalId = item->data(Qt::UserRole).toInt();

    auto reply = QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this goal?");
    if (reply != QMessageBox::Yes)
        return;

    try {
        auto &conn = DatabaseManager::getInstance().getConnection();
        pqxx::work w(conn);

        w.exec_params("DELETE FROM reading_goals WHERE id = $1", goalId);
        w.commit();

        QMessageBox::information(this, "Deleted", "Goal deleted successfully.");
        loadGoals();

    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Error Deleting Goal", e.what());
    }
}

void GoalsWindow::onUpdateGoalClicked() {
    int currentRow = ui->goalTable->currentRow();

    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a goal to update.");
        return;
    }

    QTableWidgetItem *item = ui->goalTable->item(currentRow, 0);
    int goalId = item->data(Qt::UserRole).toInt();

    QDate currentStart = QDate::fromString(ui->goalTable->item(currentRow, 0)->text(), "yyyy-MM-dd");
    int currentDuration = ui->goalTable->item(currentRow, 2)->text().toInt();

    AddGoalDialog dialog(this);
    dialog.setInitialValues(currentStart, currentDuration);

    if (dialog.exec() == QDialog::Accepted) {
        try {
            auto &conn = DatabaseManager::getInstance().getConnection();
            pqxx::work w(conn);

            w.exec_params(
                "UPDATE reading_goals SET start_date = $1, deadline = $2, duration_days = $3 WHERE id = $4",
                dialog.getStartDate().toString("yyyy-MM-dd").toStdString(),
                dialog.getDeadline().toString("yyyy-MM-dd").toStdString(),
                dialog.getDuration(),
                goalId
            );

            w.commit();
            loadGoals();
            QMessageBox::information(this, "Updated", "Goal updated successfully.");

        } catch (const std::exception &e) {
            QMessageBox::critical(this, "Update Error", e.what());
        }
    }
}