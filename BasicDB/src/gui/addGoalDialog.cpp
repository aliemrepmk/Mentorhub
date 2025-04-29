#include "addGoalDialog.hpp"
#include "ui_AddGoalDialog.h"
#include <QDate>

AddGoalDialog::AddGoalDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddGoalDialog) {
    ui->setupUi(this);

    // Set today as the minimum start date
    ui->startDateEdit->setMinimumDate(QDate::currentDate());
    ui->startDateEdit->setDate(QDate::currentDate());

    connect(ui->okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

AddGoalDialog::~AddGoalDialog() {
    delete ui;
}

QDate AddGoalDialog::getStartDate() const {
    return ui->startDateEdit->date();
}

int AddGoalDialog::getDuration() const {
    return ui->durationSpin->value();
}

QDate AddGoalDialog::getDeadline() const {
    return getStartDate().addDays(getDuration());
}

void AddGoalDialog::setInitialValues(const QDate &startDate, int duration) {
    ui->startDateEdit->setDate(startDate);
    ui->durationSpin->setValue(duration);
}