#ifndef READINGLISTDETAILSWINDOW_HPP
#define READINGLISTDETAILSWINDOW_HPP

#include <QWidget>

namespace Ui {
class ReadingListDetailsWindow;
}

class ReadingListDetailsWindow : public QWidget {
    Q_OBJECT

public:
    explicit ReadingListDetailsWindow(int userId, int listId, const QString &listName, QWidget *parent = nullptr);
    ~ReadingListDetailsWindow();

private slots:
    void onAddBookClicked();
    void onRemoveBookClicked();
    void onShowGoalsClicked();
    void onDeleteListClicked();

signals:
    void readingListDeleted();

private:
    Ui::ReadingListDetailsWindow *ui;
    int m_listId;
    int m_userId;
    QString m_listName;

    void loadBooks(); // Will query and show books
};

#endif // READINGLISTDETAILSWINDOW_HPP