#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <QScrollBar>
#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void processStarted();
    void readyReadStandardOutput();
    void processFinished();
    void openFile();
    void convertVideo();

private:
    QProcess* process;
    QLineEdit *filePath;
    QPushButton *browserButton;
    QComboBox *filterList;
    QPushButton *convertButton;
    QTextEdit *outputDisplay;
    QString outputText;

    void initWindow();
    void createFilterList();
};
#endif // MAINWINDOW_H
