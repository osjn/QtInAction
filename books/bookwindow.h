#ifndef BOOKWINDOW_H
#define BOOKWINDOW_H

#include <QtWidgets>
#include <QtSql>

class BookWindow : public QMainWindow
{
    Q_OBJECT

public:
    BookWindow(QWidget *parent = nullptr);
    ~BookWindow();

private slots:
    void about();

private:
    void showError(const QSqlError &err);
    QSqlRelationalTableModel *model;
    int authorIdx, genreIdx;

    void createMenuBar();
};
#endif // BOOKWINDOW_H
