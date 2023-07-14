#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <opencv2/opencv.hpp>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_inputPushButton_pressed();

    void on_outputPushButton_pressed();

private:
    Ui::MainWindow *ui;

    void loadSettings();
    void saveSettings();
};
#endif // MAINWINDOW_H
