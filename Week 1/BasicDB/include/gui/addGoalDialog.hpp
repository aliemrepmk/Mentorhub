#ifndef ADDGOALDIALOG_HPP
#define ADDGOALDIALOG_HPP

#include <QDialog>
#include <QDate>

namespace Ui {
class AddGoalDialog;
}

class AddGoalDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddGoalDialog(QWidget *parent = nullptr);
    ~AddGoalDialog();

    QDate getStartDate() const;
    QDate getDeadline() const;
    int getDuration() const;

    void setInitialValues(const QDate &startDate, int duration);

private:
    Ui::AddGoalDialog *ui;
};

#endif // ADDGOALDIALOG_HPP
