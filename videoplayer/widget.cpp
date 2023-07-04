#include "widget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    initWindow();
}

Widget::~Widget()
{
}

void Widget::onBtnOpenClick()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("选择视频"),
        "",
        tr("视频(*.mp4)")
        );
    videoWidget->setUrl(fileName);
    videoWidget->open();
}

void Widget::initWindow()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    videoWidget = new VideoWidget;
    mainLayout->addWidget(videoWidget);
    QHBoxLayout *controlLayout = new QHBoxLayout;
    QPushButton *btnOpen = new QPushButton(tr("打开"));
    controlLayout->addWidget(btnOpen);
    controlLayout->addStretch();
    mainLayout->addLayout(controlLayout);
    setLayout(mainLayout);
//    resize(800, 600);

    connect(btnOpen, SIGNAL(clicked()), this, SLOT(onBtnOpenClick()));
}

