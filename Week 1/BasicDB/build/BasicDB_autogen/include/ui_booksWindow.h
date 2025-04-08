/********************************************************************************
** Form generated from reading UI file 'booksWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BOOKSWINDOW_H
#define UI_BOOKSWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BooksWindow
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QHBoxLayout *filterLayout;
    QLineEdit *authorFilterInput;
    QLineEdit *isbnFilterInput;
    QVBoxLayout *yearSliderLayout;
    QHBoxLayout *yearLabelsLayout;
    QLabel *minYearLabel;
    QLabel *maxYearLabel;
    QHBoxLayout *yearSliders;
    QSlider *minYearSlider;
    QSlider *maxYearSlider;
    QPushButton *applyFilterButton;
    QPushButton *clearFilterButton;
    QHBoxLayout *searchLayout;
    QLineEdit *searchInput;
    QPushButton *searchButton;
    QTableWidget *bookTable;

    void setupUi(QWidget *BooksWindow)
    {
        if (BooksWindow->objectName().isEmpty())
            BooksWindow->setObjectName("BooksWindow");
        BooksWindow->setMinimumSize(QSize(800, 600));
        verticalLayout = new QVBoxLayout(BooksWindow);
        verticalLayout->setObjectName("verticalLayout");
        titleLabel = new QLabel(BooksWindow);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet(QString::fromUtf8("font-size: 24px; font-weight: bold;"));

        verticalLayout->addWidget(titleLabel);

        filterLayout = new QHBoxLayout();
        filterLayout->setObjectName("filterLayout");
        authorFilterInput = new QLineEdit(BooksWindow);
        authorFilterInput->setObjectName("authorFilterInput");

        filterLayout->addWidget(authorFilterInput);

        isbnFilterInput = new QLineEdit(BooksWindow);
        isbnFilterInput->setObjectName("isbnFilterInput");

        filterLayout->addWidget(isbnFilterInput);

        yearSliderLayout = new QVBoxLayout();
        yearSliderLayout->setObjectName("yearSliderLayout");
        yearLabelsLayout = new QHBoxLayout();
        yearLabelsLayout->setObjectName("yearLabelsLayout");
        minYearLabel = new QLabel(BooksWindow);
        minYearLabel->setObjectName("minYearLabel");

        yearLabelsLayout->addWidget(minYearLabel);

        maxYearLabel = new QLabel(BooksWindow);
        maxYearLabel->setObjectName("maxYearLabel");

        yearLabelsLayout->addWidget(maxYearLabel);


        yearSliderLayout->addLayout(yearLabelsLayout);

        yearSliders = new QHBoxLayout();
        yearSliders->setObjectName("yearSliders");
        minYearSlider = new QSlider(BooksWindow);
        minYearSlider->setObjectName("minYearSlider");
        minYearSlider->setOrientation(Qt::Horizontal);
        minYearSlider->setMinimum(1800);
        minYearSlider->setMaximum(2025);

        yearSliders->addWidget(minYearSlider);

        maxYearSlider = new QSlider(BooksWindow);
        maxYearSlider->setObjectName("maxYearSlider");
        maxYearSlider->setOrientation(Qt::Horizontal);
        maxYearSlider->setMinimum(1800);
        maxYearSlider->setMaximum(2025);

        yearSliders->addWidget(maxYearSlider);


        yearSliderLayout->addLayout(yearSliders);


        filterLayout->addLayout(yearSliderLayout);

        applyFilterButton = new QPushButton(BooksWindow);
        applyFilterButton->setObjectName("applyFilterButton");

        filterLayout->addWidget(applyFilterButton);

        clearFilterButton = new QPushButton(BooksWindow);
        clearFilterButton->setObjectName("clearFilterButton");

        filterLayout->addWidget(clearFilterButton);


        verticalLayout->addLayout(filterLayout);

        searchLayout = new QHBoxLayout();
        searchLayout->setObjectName("searchLayout");
        searchInput = new QLineEdit(BooksWindow);
        searchInput->setObjectName("searchInput");

        searchLayout->addWidget(searchInput);

        searchButton = new QPushButton(BooksWindow);
        searchButton->setObjectName("searchButton");

        searchLayout->addWidget(searchButton);


        verticalLayout->addLayout(searchLayout);

        bookTable = new QTableWidget(BooksWindow);
        bookTable->setObjectName("bookTable");

        verticalLayout->addWidget(bookTable);


        retranslateUi(BooksWindow);

        QMetaObject::connectSlotsByName(BooksWindow);
    } // setupUi

    void retranslateUi(QWidget *BooksWindow)
    {
        titleLabel->setText(QCoreApplication::translate("BooksWindow", "\360\237\223\232 PERSONAL LIBRARY", nullptr));
        authorFilterInput->setPlaceholderText(QCoreApplication::translate("BooksWindow", "Author's name", nullptr));
        isbnFilterInput->setPlaceholderText(QCoreApplication::translate("BooksWindow", "ISBN", nullptr));
        minYearLabel->setText(QCoreApplication::translate("BooksWindow", "1800", nullptr));
        maxYearLabel->setText(QCoreApplication::translate("BooksWindow", "2025", nullptr));
        applyFilterButton->setText(QCoreApplication::translate("BooksWindow", "Apply filter", nullptr));
        clearFilterButton->setText(QCoreApplication::translate("BooksWindow", "Clear filters", nullptr));
        searchInput->setPlaceholderText(QCoreApplication::translate("BooksWindow", "Enter the book name", nullptr));
        searchButton->setText(QCoreApplication::translate("BooksWindow", "Search", nullptr));
        (void)BooksWindow;
    } // retranslateUi

};

namespace Ui {
    class BooksWindow: public Ui_BooksWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BOOKSWINDOW_H
