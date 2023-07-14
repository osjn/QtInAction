#include "mainwindow.h"
#include "qapplication.h"
#include "ui_mainwindow.h"
#include "cvplugininterface.h"

#define FILTERS_SUBFOLDER "/filter_plugins/"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    getPluginsList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getPluginsList()
{
    QDir filterDir(qApp->applicationDirPath() +
                   FILTERS_SUBFOLDER);
    QFileInfoList filters = filterDir.entryInfoList(
                QDir::NoDotAndDotDot | QDir::Files,
                QDir::Name);
    foreach (QFileInfo filter, filters)
    {
        if (QLibrary::isLibrary(filter.absoluteFilePath()))
        {
            QPluginLoader pluginLoader(
                        filter.absoluteFilePath(),
                        this);
            if (dynamic_cast<CvPluginInterface*>(pluginLoader.instance()))
            {
                ui->filtersList->addItem(filter.fileName());
                pluginLoader.unload();
            }
            else
            {
                QMessageBox::warning(
                            this, tr("Warning"),
                            QString(tr("Make sure %1 is a correct"
                                       " plugin for this application<br>"
                                       "and it's not in use by some other"
                                       " application!")).arg(filter.fileName()));
            }
        }
        else
        {
            QMessageBox::warning(this, tr("Warning"),
                                 QString(tr("Make sure only plugins"
                                            " exist in plugins folder.<br>"
                                            "%1 is not a plugin."))
                                 .arg(filter.fileName()));
        }
    }

    if (ui->filtersList->count() <= 0)
    {
        QMessageBox::critical(this, tr("No Plugins"),
                              tr("This application cannot work without plugins!"
                                 "<br>Make sure that filter_plugins folder exists "
                                 "in the same folder as the application<br>and that "
                                 "there are some filter plugins inside it"));
        this->setEnabled(false);
    }
}

