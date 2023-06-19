#include "mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initWindow();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initWindow()
{
    QWidget *centerWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *fileLayout = new QHBoxLayout;
    filePath = new QLineEdit;
    filePath->setReadOnly(true);
    browserButton = new QPushButton(tr("浏览..."));
    fileLayout->addWidget(filePath);
    fileLayout->addWidget(browserButton);
    mainLayout->addLayout(fileLayout);
    QHBoxLayout *convertLayout = new QHBoxLayout;
    QLabel *labelSelect = new QLabel(tr("请选择要转换的格式："));
    convertLayout->addWidget(labelSelect);
    createFilterList();
    convertLayout->addWidget(filterList);
    convertButton = new QPushButton(tr("转换"));
    convertLayout->addWidget(convertButton);
    convertLayout->addStretch();
    mainLayout->addLayout(convertLayout);
    outputDisplay = new QTextEdit;
    outputDisplay->setReadOnly(true);
    mainLayout->addWidget(outputDisplay);
    centerWidget->setLayout(mainLayout);
    setCentralWidget(centerWidget);
    resize(600, 680);
    setWindowTitle("视频格式转换");

    connect(browserButton, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(convertButton, SIGNAL(clicked()), this, SLOT(convertVideo()));

    process = new QProcess(this);
    connect(process, SIGNAL(started()), this, SLOT(processStarted()));
    connect(process, SIGNAL(readyReadStandardOutput()),
            this, SLOT(readyReadStandardOutput()));
    connect(process, SIGNAL(finished(int)), this, SLOT(processFinished()));
}

void MainWindow::createFilterList()
{
    filterList = new QComboBox;
    filterList->addItem("avi");
    filterList->addItem("mp4");
    filterList->addItem("mov");
    filterList->addItem("mkv");
}

void MainWindow::processStarted()
{
    browserButton->setEnabled(false);
    filterList->setEditable(false);
    convertButton->setEnabled(false);
}

void MainWindow::readyReadStandardOutput()
{
    outputText += process->readAllStandardOutput();
    outputDisplay->setText(outputText);
    outputDisplay->verticalScrollBar()->setSliderPosition(
        outputDisplay->verticalScrollBar()->maximum());
}

void MainWindow::processFinished()
{
    browserButton->setEnabled(true);
    filterList->setEditable(true);
    convertButton->setEnabled(true);
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open Video",
        "",
        "Video Files(*.avi *.mp4 *.mov *.mkv)"
        );
    filePath->setText(fileName);
}

void MainWindow::convertVideo()
{
    QString fileName = filePath->text();
    if (!fileName.isEmpty())
    {
        if (QFile::exists(fileName))
        {
            if (!outputDisplay->toPlainText().isEmpty())
                outputDisplay->setText("");

            QFileInfo fileInfo(fileName);
            QString outputFileName = fileInfo.path() + "/" + fileInfo.completeBaseName();
            int format = filterList->currentIndex();
            switch (format) {
            case 0:
                outputFileName += ".avi";
                break;
            case 1:
                outputFileName += ".mp4";
                break;
            case 2:
                outputFileName += ".mov";
                break;
            case 3:
                outputFileName += ".mkv";
                break;
            default:
                break;
            }

            QStringList arguments;
            arguments << "-i" << fileName << outputFileName;
            process->setProcessChannelMode(QProcess::MergedChannels);
            process->start("d:/app/ffmpeg/bin/ffmpeg", arguments);
        }
        else
        {
            QMessageBox::warning(this, tr("提示信息"), tr("视频文件不存在"));
        }
    }
    else
    {
        QMessageBox::warning(this, tr("提示信息"), tr("未选择任何文件"));
    }
}



