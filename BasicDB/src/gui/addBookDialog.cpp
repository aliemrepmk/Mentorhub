#include "addBookDialog.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

AddBookDialog::AddBookDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Add a Book by ISBN");

    auto *layout = new QVBoxLayout(this);

    auto *label = new QLabel("Enter ISBN:");
    isbnInput = new QLineEdit();
    layout->addWidget(label);
    layout->addWidget(isbnInput);

    addButton = new QPushButton("Add Book");
    layout->addWidget(addButton);

    connect(addButton, &QPushButton::clicked, this, &AddBookDialog::onAddClicked);
}

void AddBookDialog::onAddClicked() {
    if (!isbnInput->text().trimmed().isEmpty()) {
        accept();  // closes dialog with QDialog::Accepted
    }
}

QString AddBookDialog::getISBN() const {
    return isbnInput->text().trimmed();
}