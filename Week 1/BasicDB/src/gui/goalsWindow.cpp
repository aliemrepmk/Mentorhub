#include "goalsWindow.hpp"
#include "ui_GoalsWindow.h"
#include "database/dbManager.hpp"
#include <pqxx/pqxx>
#include <QTableWidgetItem>
#include <QMessageBox>

GoalsWindow::GoalsWindow(int readingListId, QWidget *parent)
    : QWidget(parent), ui(new Ui::GoalsWindow), m_readingListId(readingListId) {
    ui->setupUi(this);
    setWindowTitle("📈 Reading Goals");

    ui->goalTable->setColumnCount(3);
    ui->goalTable->setHorizontalHeaderLabels(QStringList() << "Start Date" << "Deadline" << "Duration (Days)");
    ui->goalTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->goalTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->closeButton, &QPushButton::clicked, this, &GoalsWindow::close);

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
            "SELECT start_date, deadline, duration_days FROM reading_goals WHERE reading_list_id = $1",
            m_readingListId
        );

        ui->goalTable->setRowCount(result.size());

        int row = 0;
        for (const auto &r : result) {
            ui->goalTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(r["start_date"].c_str())));
            ui->goalTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r["deadline"].c_str())));
            ui->goalTable->setItem(row, 2, new QTableWidgetItem(QString::number(r["duration_days"].as<int>())));
            ++row;
        }

    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Error Loading Goals", e.what());
    }
}