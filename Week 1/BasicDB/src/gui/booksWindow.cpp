#include "booksWindow.hpp"
#include "ui_booksWindow.h"
#include "database/bookManager.hpp"

#include <QTableWidgetItem>
#include <QString>

BooksWindow::BooksWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::BooksWindow) {
    ui->setupUi(this);

    connect(ui->searchButton, &QPushButton::clicked, this, &BooksWindow::searchBooks);
    connect(ui->applyFilterButton, &QPushButton::clicked, this, &BooksWindow::applyFilters);
    connect(ui->clearFilterButton, &QPushButton::clicked, this, &BooksWindow::clearFilters);
    connect(ui->minYearSlider, &QSlider::valueChanged, this, &BooksWindow::updateYearLabels);
    connect(ui->maxYearSlider, &QSlider::valueChanged, this, &BooksWindow::updateYearLabels);

    // Set defaults
    ui->minYearSlider->setValue(1800);
    ui->maxYearSlider->setValue(2025);
    updateYearLabels();

    loadAllBooks();
}

BooksWindow::~BooksWindow() {
    delete ui;
}

void BooksWindow::updateYearLabels() {
    ui->minYearLabel->setText(QString::number(ui->minYearSlider->value()));
    ui->maxYearLabel->setText(QString::number(ui->maxYearSlider->value()));
}

void BooksWindow::loadAllBooks() {
    auto books = BookManager::getAllBooks();
    populateTable(books);
}

void BooksWindow::searchBooks() {
    QString query = ui->searchInput->text().trimmed();
    if (!query.isEmpty()) {
        auto books = BookManager::searchBooksByName(query.toStdString());
        populateTable(books);
    } else {
        loadAllBooks();
    }
}

void BooksWindow::applyFilters() {
    QString author = ui->authorFilterInput->text().trimmed();
    QString isbn = ui->isbnFilterInput->text().trimmed();
    int minYear = ui->minYearSlider->value();
    int maxYear = ui->maxYearSlider->value();

    auto books = BookManager::filterBooks(
        author.toStdString(),
        isbn.toStdString(),
        minYear,
        maxYear
    );

    populateTable(books);
}

void BooksWindow::populateTable(const std::vector<std::tuple<std::string, std::string, std::string, std::string>>& books) {
    ui->bookTable->clearContents();
    ui->bookTable->setRowCount(static_cast<int>(books.size()));
    ui->bookTable->setColumnCount(4);
    ui->bookTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList headers = { "Book name", "Author", "Release year", "ISBN" };
    ui->bookTable->setHorizontalHeaderLabels(headers);
    ui->bookTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int row = 0;
    for (const auto& book : books) {
        ui->bookTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(std::get<0>(book))));
        ui->bookTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(std::get<1>(book))));
        ui->bookTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(std::get<2>(book))));
        ui->bookTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(std::get<3>(book))));
        ++row;
    }
}

void BooksWindow::clearFilters() {
    ui->authorFilterInput->clear();
    ui->isbnFilterInput->clear();
    ui->minYearSlider->setValue(1800);
    ui->maxYearSlider->setValue(2025);
    updateYearLabels();
    loadAllBooks();
}