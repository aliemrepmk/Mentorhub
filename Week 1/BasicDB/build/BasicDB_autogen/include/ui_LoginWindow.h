/********************************************************************************
** Form generated from reading UI file 'LoginWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWINDOW_H
#define UI_LOGINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginWindow
{
public:
    QVBoxLayout *verticalLayout;
    QTableWidget *userTable;
    QLabel *label;
    QLineEdit *emailInput;
    QPushButton *loginButton;
    QLabel *statusLabel;

    void setupUi(QWidget *LoginWindow)
    {
        if (LoginWindow->objectName().isEmpty())
            LoginWindow->setObjectName("LoginWindow");
        LoginWindow->resize(800, 600);
        LoginWindow->setMinimumSize(QSize(800, 600));
        verticalLayout = new QVBoxLayout(LoginWindow);
        verticalLayout->setObjectName("verticalLayout");
        userTable = new QTableWidget(LoginWindow);
        userTable->setObjectName("userTable");

        verticalLayout->addWidget(userTable);

        label = new QLabel(LoginWindow);
        label->setObjectName("label");

        verticalLayout->addWidget(label);

        emailInput = new QLineEdit(LoginWindow);
        emailInput->setObjectName("emailInput");

        verticalLayout->addWidget(emailInput);

        loginButton = new QPushButton(LoginWindow);
        loginButton->setObjectName("loginButton");

        verticalLayout->addWidget(loginButton);

        statusLabel = new QLabel(LoginWindow);
        statusLabel->setObjectName("statusLabel");

        verticalLayout->addWidget(statusLabel);


        retranslateUi(LoginWindow);

        QMetaObject::connectSlotsByName(LoginWindow);
    } // setupUi

    void retranslateUi(QWidget *LoginWindow)
    {
        LoginWindow->setWindowTitle(QCoreApplication::translate("LoginWindow", "Login", nullptr));
        label->setText(QCoreApplication::translate("LoginWindow", "Enter your email to login:", nullptr));
        emailInput->setPlaceholderText(QCoreApplication::translate("LoginWindow", "Email", nullptr));
        loginButton->setText(QCoreApplication::translate("LoginWindow", "Login", nullptr));
        statusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class LoginWindow: public Ui_LoginWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWINDOW_H
