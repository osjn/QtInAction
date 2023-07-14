#include "textfinder.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QMessageBox>

TextFinder::TextFinder(QWidget *parent)
    : QMainWindow(parent)
{
    QHBoxLayout *hbLayout = new QHBoxLayout;
    QLabel *keyword = new QLabel("关键字: ");
    leKeyword = new QLineEdit;
    pbFind = new QPushButton("搜索");
    hbLayout->addWidget(keyword);
    hbLayout->addWidget(leKeyword);
    hbLayout->addWidget(pbFind);
    teFile = new QTextEdit;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(hbLayout);
    mainLayout->addWidget(teFile);
    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
    resize(800, 600);

    loadTextFile();
    connect(pbFind, SIGNAL(clicked()), this, SLOT(onFindButtonClicked()));
}

void TextFinder::loadTextFile()
{
    QFile inputFile("input.txt");
    inputFile.open(QIODevice::ReadOnly);

    QTextStream in(&inputFile);
    QString line = in.readAll();
    inputFile.close();

    teFile->setPlainText(line);
    QTextCursor cursor = teFile->textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
}

void TextFinder::onFindButtonClicked()
{
    QString searchString = leKeyword->text();
    if (!teFile->find(searchString))
    {
        QMessageBox::information(this, "温馨提示", "没有找到");
        teFile->moveCursor(QTextCursor::Start);
    }

    QPalette palette = teFile->palette();
    palette.setColor(QPalette::Highlight, palette.color(QPalette::Active, QPalette::Highlight));
    teFile->setPalette(palette);
}

TextFinder::~TextFinder()
{
}
