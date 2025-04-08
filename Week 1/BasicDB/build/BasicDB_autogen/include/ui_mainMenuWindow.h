/********************************************************************************
** Form generated from reading UI file 'mainMenuWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINMENUWINDOW_H
#define UI_MAINMENUWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainMenuWindow
{
public:
    QVBoxLayout *mainLayout;
    QLabel *headerLabel;
    QHBoxLayout *bodyLayout;
    QVBoxLayout *navLayout;
    QPushButton *booksButton;
    QPushButton *readingListsButton;
    QPushButton *goalsButton;
    QPushButton *accountButton;
    QSpacerItem *navSpacer;
    QPushButton *logoutButton;
    QStackedWidget *mainContent;

    void setupUi(QWidget *MainMenuWindow)
    {
        if (MainMenuWindow->objectName().isEmpty())
            MainMenuWindow->setObjectName("MainMenuWindow");
        MainMenuWindow->resize(800, 600);
        MainMenuWindow->setMinimumSize(QSize(800, 600));
        mainLayout = new QVBoxLayout(MainMenuWindow);
        mainLayout->setObjectName("mainLayout");
        headerLabel = new QLabel(MainMenuWindow);
        headerLabel->setObjectName("headerLabel");
        headerLabel->setAlignment(Qt::AlignCenter);
        headerLabel->setStyleSheet(QString::fromUtf8("font-size: 24px; font-weight: bold;"));

        mainLayout->addWidget(headerLabel);

        bodyLayout = new QHBoxLayout();
        bodyLayout->setObjectName("bodyLayout");
        navLayout = new QVBoxLayout();
        navLayout->setObjectName("navLayout");
        booksButton = new QPushButton(MainMenuWindow);
        booksButton->setObjectName("booksButton");
        booksButton->setStyleSheet(QString::fromUtf8("font-weight: bold;"));

        navLayout->addWidget(booksButton);

        readingListsButton = new QPushButton(MainMenuWindow);
        readingListsButton->setObjectName("readingListsButton");
        readingListsButton->setStyleSheet(QString::fromUtf8("font-weight: bold;"));

        navLayout->addWidget(readingListsButton);

        goalsButton = new QPushButton(MainMenuWindow);
        goalsButton->setObjectName("goalsButton");
        goalsButton->setStyleSheet(QString::fromUtf8("font-weight: bold;"));

        navLayout->addWidget(goalsButton);

        accountButton = new QPushButton(MainMenuWindow);
        accountButton->setObjectName("accountButton");
        accountButton->setStyleSheet(QString::fromUtf8("font-weight: bold;"));

        navLayout->addWidget(accountButton);

        navSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        navLayout->addItem(navSpacer);

        logoutButton = new QPushButton(MainMenuWindow);
        logoutButton->setObjectName("logoutButton");
        logoutButton->setStyleSheet(QString::fromUtf8("font-weight: bold; color: red;"));

        navLayout->addWidget(logoutButton);


        bodyLayout->addLayout(navLayout);

        mainContent = new QStackedWidget(MainMenuWindow);
        mainContent->setObjectName("mainContent");

        bodyLayout->addWidget(mainContent);


        mainLayout->addLayout(bodyLayout);


        retranslateUi(MainMenuWindow);

        QMetaObject::connectSlotsByName(MainMenuWindow);
    } // setupUi

    void retranslateUi(QWidget *MainMenuWindow)
    {
        MainMenuWindow->setWindowTitle(QCoreApplication::translate("MainMenuWindow", "Main Menu", nullptr));
        headerLabel->setText(QCoreApplication::translate("MainMenuWindow", "\360\237\223\232 PERSONAL LIBRARY", nullptr));
        booksButton->setText(QCoreApplication::translate("MainMenuWindow", "\360\237\223\226 Books", nullptr));
        readingListsButton->setText(QCoreApplication::translate("MainMenuWindow", "\360\237\223\213 Reading Lists", nullptr));
        goalsButton->setText(QCoreApplication::translate("MainMenuWindow", "\360\237\216\257 Goals", nullptr));
        accountButton->setText(QCoreApplication::translate("MainMenuWindow", "\360\237\221\244 Account", nullptr));
        logoutButton->setText(QCoreApplication::translate("MainMenuWindow", "\360\237\232\252 Logout", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainMenuWindow: public Ui_MainMenuWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINMENUWINDOW_H
