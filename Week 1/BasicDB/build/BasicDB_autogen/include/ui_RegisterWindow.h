/********************************************************************************
** Form generated from reading UI file 'RegisterWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTERWINDOW_H
#define UI_REGISTERWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RegisterWindow
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QLineEdit *firstNameInput;
    QLineEdit *lastNameInput;
    QLineEdit *emailInput;
    QLineEdit *passwordInput;
    QPushButton *registerButton;
    QLabel *statusLabel;

    void setupUi(QWidget *RegisterWindow)
    {
        if (RegisterWindow->objectName().isEmpty())
            RegisterWindow->setObjectName("RegisterWindow");
        RegisterWindow->resize(600, 400);
        verticalLayout = new QVBoxLayout(RegisterWindow);
        verticalLayout->setObjectName("verticalLayout");
        titleLabel = new QLabel(RegisterWindow);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet(QString::fromUtf8("font-size: 18px; font-weight: bold;"));

        verticalLayout->addWidget(titleLabel);

        firstNameInput = new QLineEdit(RegisterWindow);
        firstNameInput->setObjectName("firstNameInput");

        verticalLayout->addWidget(firstNameInput);

        lastNameInput = new QLineEdit(RegisterWindow);
        lastNameInput->setObjectName("lastNameInput");

        verticalLayout->addWidget(lastNameInput);

        emailInput = new QLineEdit(RegisterWindow);
        emailInput->setObjectName("emailInput");

        verticalLayout->addWidget(emailInput);

        passwordInput = new QLineEdit(RegisterWindow);
        passwordInput->setObjectName("passwordInput");
        passwordInput->setEchoMode(QLineEdit::Password);

        verticalLayout->addWidget(passwordInput);

        registerButton = new QPushButton(RegisterWindow);
        registerButton->setObjectName("registerButton");
        registerButton->setStyleSheet(QString::fromUtf8("padding: 6px; font-weight: bold;"));

        verticalLayout->addWidget(registerButton);

        statusLabel = new QLabel(RegisterWindow);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setStyleSheet(QString::fromUtf8("color: red;"));

        verticalLayout->addWidget(statusLabel);


        retranslateUi(RegisterWindow);

        QMetaObject::connectSlotsByName(RegisterWindow);
    } // setupUi

    void retranslateUi(QWidget *RegisterWindow)
    {
        RegisterWindow->setWindowTitle(QCoreApplication::translate("RegisterWindow", "Create Account", nullptr));
        titleLabel->setText(QCoreApplication::translate("RegisterWindow", "Create Your Personal Library Account", nullptr));
        firstNameInput->setPlaceholderText(QCoreApplication::translate("RegisterWindow", "First Name", nullptr));
        lastNameInput->setPlaceholderText(QCoreApplication::translate("RegisterWindow", "Last Name", nullptr));
        emailInput->setPlaceholderText(QCoreApplication::translate("RegisterWindow", "E-mail", nullptr));
        passwordInput->setPlaceholderText(QCoreApplication::translate("RegisterWindow", "Password", nullptr));
        registerButton->setText(QCoreApplication::translate("RegisterWindow", "Register", nullptr));
        statusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class RegisterWindow: public Ui_RegisterWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTERWINDOW_H
