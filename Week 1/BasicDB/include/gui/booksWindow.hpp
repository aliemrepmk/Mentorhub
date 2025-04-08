#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class BooksWindow; }
QT_END_NAMESPACE

class BooksWindow : public QWidget {
    Q_OBJECT

public:
    explicit BooksWindow(QWidget *parent = nullptr);
    ~BooksWindow();

private slots:
    void loadAllBooks();
    void searchBooks();
    void applyFilters();
    void updateYearLabels();
    void clearFilters();

private:
    Ui::BooksWindow *ui;
    void populateTable(const std::vector<std::tuple<std::string, std::string, std::string, std::string>>& books);
};
