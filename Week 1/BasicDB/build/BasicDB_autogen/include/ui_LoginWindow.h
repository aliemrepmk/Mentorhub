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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginWindow
{
public:
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QHBoxLayout *formLayout;
    QVBoxLayout *inputLayout;
    QLineEdit *emailInput;
    QLineEdit *passwordInput;
    QPushButton *loginButton;
    QPushButton *createAccountButton;
    QLabel *statusLabel;

    void setupUi(QWidget *LoginWindow)
    {
        if (LoginWindow->objectName().isEmpty())
            LoginWindow->setObjectName("LoginWindow");
        LoginWindow->resize(800, 600);
        LoginWindow->setMinimumSize(QSize(800, 600));
        mainLayout = new QVBoxLayout(LoginWindow);
        mainLayout->setObjectName("mainLayout");
        titleLabel = new QLabel(LoginWindow);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet(QString::fromUtf8("font-size: 24px; font-weight: bold;"));

        mainLayout->addWidget(titleLabel);

        formLayout = new QHBoxLayout();
        formLayout->setObjectName("formLayout");
        inputLayout = new QVBoxLayout();
        inputLayout->setObjectName("inputLayout");
        emailInput = new QLineEdit(LoginWindow);
        emailInput->setObjectName("emailInput");

        inputLayout->addWidget(emailInput);

        passwordInput = new QLineEdit(LoginWindow);
        passwordInput->setObjectName("passwordInput");
        passwordInput->setEchoMode(QLineEdit::Password);

        inputLayout->addWidget(passwordInput);


        formLayout->addLayout(inputLayout);

        loginButton = new QPushButton(LoginWindow);
        loginButton->setObjectName("loginButton");
        loginButton->setMinimumSize(QSize(120, 80));
        loginButton->setStyleSheet(QString::fromUtf8("font-weight: bold;"));

        formLayout->addWidget(loginButton);


        mainLayout->addLayout(formLayout);

        createAccountButton = new QPushButton(LoginWindow);
        createAccountButton->setObjectName("createAccountButton");
        createAccountButton->setStyleSheet(QString::fromUtf8("font-weight: bold; padding: 8px;"));

        mainLayout->addWidget(createAccountButton);

        statusLabel = new QLabel(LoginWindow);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setAlignment(Qt::AlignCenter);

        mainLayout->addWidget(statusLabel);


        retranslateUi(LoginWindow);

        QMetaObject::connectSlotsByName(LoginWindow);
    } // setupUi

    void retranslateUi(QWidget *LoginWindow)
    {
        titleLabel->setText(QCoreApplication::translate("LoginWindow", "PERSONAL LIBRARY", nullptr));
        emailInput->setPlaceholderText(QCoreApplication::translate("LoginWindow", "E-mail", nullptr));
        passwordInput->setPlaceholderText(QCoreApplication::translate("LoginWindow", "Password", nullptr));
        loginButton->setText(QCoreApplication::translate("LoginWindow", "Login", nullptr));
        createAccountButton->setText(QCoreApplication::translate("LoginWindow", "If you don't have an account click here to create your account", nullptr));
        statusLabel->setText(QString());
        (void)LoginWindow;
    } // retranslateUi

};

namespace Ui {
    class LoginWindow: public Ui_LoginWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWINDOW_H
