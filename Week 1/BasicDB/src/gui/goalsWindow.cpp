#include "goalsWindow.hpp"
#include "ui_GoalsWindow.h"
#include "database/goalsManager.hpp"
#include "addGoalDialog.hpp"

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
    auto goals = GoalsManager::getGoalsForList(m_readingListId);

    if (goals.empty()) {
        QMessageBox::information(this, "No Goals Found", "There are no goals in this reading list.");
        ui->goalTable->setRowCount(0);
        return;
    }

    ui->goalTable->setRowCount(static_cast<int>(goals.size()));
    int row = 0;
    for (const auto& g : goals) {
        QTableWidgetItem *startItem = new QTableWidgetItem(QString::fromStdString(g.startDate));
        startItem->setData(Qt::UserRole, g.id);

        ui->goalTable->setItem(row, 0, startItem);
        ui->goalTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(g.deadline)));
        ui->goalTable->setItem(row, 2, new QTableWidgetItem(QString::number(g.durationDays)));
        ++row;
    }
}

void GoalsWindow::onAddGoalClicked() {
    AddGoalDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        bool success = GoalsManager::addGoal(
            m_userId,
            m_readingListId,
            dialog.getStartDate(),
            dialog.getDeadline(),
            dialog.getDuration()
        );

        if (success)
            loadGoals();
        else
            QMessageBox::critical(this, "Add Goal Error", "Failed to add goal.");
    }
}

void GoalsWindow::onDeleteGoalClicked() {
    int currentRow = ui->goalTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a goal to delete.");
        return;
    }

    auto item = ui->goalTable->item(currentRow, 0);
    int goalId = item->data(Qt::UserRole).toInt();

    auto reply = QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this goal?");
    if (reply != QMessageBox::Yes)
        return;

    if (GoalsManager::deleteGoal(goalId)) {
        QMessageBox::information(this, "Deleted", "Goal deleted successfully.");
        loadGoals();
    } else {
        QMessageBox::critical(this, "Error Deleting Goal", "Failed to delete goal.");
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
        bool success = GoalsManager::updateGoal(
            goalId,
            dialog.getStartDate(),
            dialog.getDeadline(),
            dialog.getDuration()
        );

        if (success) {
            loadGoals();
            QMessageBox::information(this, "Updated", "Goal updated successfully.");
        } else {
            QMessageBox::critical(this, "Update Error", "Failed to update goal.");
        }
    }
}
