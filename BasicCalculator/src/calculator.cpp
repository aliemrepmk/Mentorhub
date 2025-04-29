#include "calculator.h"

Calculator::Calculator(QWidget *parent) : QWidget(parent) {
    auto *layout = new QGridLayout(this);
    display = new QLineEdit(this);
    layout->addWidget(display, 0, 0, 1, 4);

    QStringList buttons = {"7", "8", "9", "/",
                           "4", "5", "6", "*",
                           "1", "2", "3", "-",
                           "0", "C", "=", "+"};

    int row = 1;
    int col = 0;
    for (const auto &text : buttons) {
        auto *btn = new QPushButton(text, this);
        layout->addWidget(btn, row, col);
        connect(btn, &QPushButton::clicked, this, &Calculator::onButtonClicked);
        col++;
        if (col == 4) {
            col = 0;
            row++;
        }
    }

    setLayout(layout);
    setWindowTitle("Simple Calculator");
    resize(300, 300);
}

void Calculator::onButtonClicked() {
    auto *btn = qobject_cast<QPushButton *>(sender());
    QString btnText = btn->text();

    if (btnText == "C") {
        display->clear();
    } else if (btnText == "=") {
        QString expr = display->text();
        QJSEngine engine;
        display->setText(engine.evaluate(expr).toString());
    } else {
        display->setText(display->text() + btnText);
    }
}