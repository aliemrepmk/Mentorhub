#ifndef ADDBOOKDIALOG_HPP
#define ADDBOOKDIALOG_HPP

#include <QDialog>

class QLineEdit;
class QPushButton;

class AddBookDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddBookDialog(QWidget *parent = nullptr);
    QString getISBN() const;

private:
    QLineEdit *isbnInput;
    QPushButton *addButton;

private slots:
    void onAddClicked();
};

#endif // ADDBOOKDIALOG_HPP