#include "mainwindow.h"

#include <QVBoxLayout>
#include <QWebEngineSettings>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    initWindow();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initWindow()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mWeView = new QWebEngineView(this);
    mWeView->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
    mainLayout->addWidget(mWeView);
    setLayout(mainLayout);
    mWeView->load(QUrl(QStringLiteral("qrc:/view/index.html")));
    resize(800, 768);
}

