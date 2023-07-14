#ifndef TEXTFINDER_H
#define TEXTFINDER_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

class TextFinder : public QMainWindow
{
    Q_OBJECT

public:
    TextFinder(QWidget *parent = nullptr);
    ~TextFinder();

private slots:
    void onFindButtonClicked();

private:
    QLineEdit *leKeyword;
    QPushButton *pbFind;
    QTextEdit *teFile;

    void loadTextFile();
};
#endif // TEXTFINDER_H
