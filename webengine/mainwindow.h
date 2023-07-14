#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QWebEngineView>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QWebEngineView *mWeView;

    void initWindow();
};
#endif // MAINWINDOW_H
