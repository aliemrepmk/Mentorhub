#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class ReadingListsWindow; }
QT_END_NAMESPACE

class ReadingListsWindow : public QWidget {
    Q_OBJECT

public:
    explicit ReadingListsWindow(int userId, QWidget *parent = nullptr);
    ~ReadingListsWindow();

private:
    Ui::ReadingListsWindow *ui;
    int m_userId;

    void loadReadingLists();

private slots:
    void onAddReadingListClicked();
    void onReadingListClicked(int row, int column);
};