#include "httpwindow.h"
#include "ui_authenticationdialog.h"

#include <QtWidgets>
#include <QtNetwork>
#include <QUrl>

#include <algorithm>
#include <memory>

#if QT_CONFIG(ssl)
const char defaultUrl[] = "https://www.qt.io";
#else
const char defaultUrl[] = "http://www.qt.io";
#endif
const char defaultFileName[] = "index.html";

ProgressDialog::ProgressDialog(const QUrl &url, QWidget *parent)
    : QProgressDialog(parent)
{
    setWindowTitle(tr("Download Progress"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setLabelText(tr("Downloading %1.").arg(url.toDisplayString()));
    setMinimum(0);
    setValue(0);
    setMinimumDuration(0);
    setMinimumSize(400, 75);
}

void ProgressDialog::networkReplyProgress(qint64 bytesRead, qint64 totalBytes)
{
    setMaximum(totalBytes);
    setValue(bytesRead);
}

HttpWindow::HttpWindow(QWidget *parent)
    : QDialog(parent)
    , statusLabel(new QLabel(tr("Please enter the URL of a file you want to download.\n\n"), this))
    , urlLineEdit(new QLineEdit(defaultUrl))
    , downloadButton(new QPushButton(tr("Download")))
    , launchCheckBox(new QCheckBox("Launch file"))
    , defaultFileLineEdit(new QLineEdit(defaultFileName))
    , downloadDirectoryLineEdit(new QLineEdit)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("HTTP"));

    connect(&qnam, &QNetworkAccessManager::authenticationRequired,
            this, &HttpWindow::slotAuthenticationRequired);

    QFormLayout *formLayout = new QFormLayout;
    urlLineEdit->setClearButtonEnabled(true);
    connect(urlLineEdit, SIGNAL(textChanged()), this, SLOT(enableDownloadButton()));
    formLayout->addRow(tr("&URL:"), urlLineEdit);
    QString downloadDirectory = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (downloadDirectory.isEmpty() || !QFileInfo(downloadDirectory).isDir())
        downloadDirectory = QDir::currentPath();
    downloadDirectoryLineEdit->setText(QDir::toNativeSeparators(downloadDirectory));
    formLayout->addRow(tr("&Download directory:"), downloadDirectoryLineEdit);
    formLayout->addRow(tr("Default &file"), defaultFileLineEdit);
    launchCheckBox->setChecked(true);
    formLayout->addRow(launchCheckBox);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);

    mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));

    statusLabel->setWordWrap(true);
    mainLayout->addWidget(statusLabel);

    downloadButton->setDefault(true);
    connect(downloadButton, SIGNAL(clicked()), this, SLOT(downloadFile()));
    QPushButton *quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(downloadButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);
    mainLayout->addWidget(buttonBox);

    urlLineEdit->setFocus();
}

HttpWindow::~HttpWindow() = default;

void HttpWindow::startRequest(const QUrl &requestedUrl)
{
    url = requestedUrl;
    httpRequestAborted = false;

    reply.reset(qnam.get(QNetworkRequest(url)));
    connect(reply.get(), &QNetworkReply::finished, this, &HttpWindow::httpFinished);
    connect(reply.get(), &QIODevice::readyRead, this, &HttpWindow::httpReadyRead);
#if QT_CONFIG(ssl)
    connect(reply.get(), &QNetworkReply::sslErrors, this, &HttpWindow::sslErrors);
#endif

    ProgressDialog *progressDialog = new ProgressDialog(url, this);
    progressDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(progressDialog, &QProgressDialog::canceled, this, &HttpWindow::cancelDownload);
    connect(reply.get(), &QNetworkReply::downloadProgress,
            progressDialog, &ProgressDialog::networkReplyProgress);
    connect(reply.get(), &QNetworkReply::finished, progressDialog, &ProgressDialog::hide);
    progressDialog->show();

    statusLabel->setText(tr("Downloading 1%...").arg(url.toString()));
}

void HttpWindow::downloadFile()
{
    const QString urlSpec = urlLineEdit->text().trimmed();
    if (urlSpec.isEmpty())
        return;

    const QUrl newUrl = QUrl::fromUserInput(urlSpec);
    if (!newUrl.isValid())
    {
        QMessageBox::information(this, tr("Error"),
                                 tr("Invalid URL: %1: %2").arg(urlSpec, newUrl.errorString()));
        return;
    }

    QString fileName = newUrl.fileName();
    if (fileName.isEmpty())
        fileName = defaultFileLineEdit->text().trimmed();
    if (fileName.isEmpty())
        fileName = defaultFileName;
    QString downloadDirectory = QDir::cleanPath(downloadDirectoryLineEdit->text().trimmed());
    bool useDirectory = !downloadDirectory.isEmpty() && QFileInfo(downloadDirectory).isDir();
    if (useDirectory)
        fileName.prepend(downloadDirectory + '/');
    if (QFile::exists(fileName))
    {
        if (QMessageBox::question(this, tr("Overwrite Existing File"),
                                  tr("There already exists a file called %1%2."
                                     "Overwrite?")
                                  .arg(fileName, useDirectory
                                       ? QString()
                                       : QStringLiteral(" in the current directory")),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No)
                == QMessageBox::No)
        {
            return;
        }
        QFile::remove(fileName);
    }

    file = openFileForWrite(fileName);
    if (!file)
        return;

    downloadButton->setEnabled(false);

    startRequest(newUrl);
}

std::unique_ptr<QFile> HttpWindow::openFileForWrite(const QString &fileName)
{
   std::unique_ptr<QFile> file = std::make_unique<QFile>(fileName);
   if (!file->open(QIODevice::WriteOnly))
   {
       QMessageBox::information(this, tr("Error"),
                                tr("Unable to save the file %1: %2.")
                                .arg(QDir::toNativeSeparators(fileName),
                                     file->errorString()));
       return nullptr;
   }
   return file;
}

void HttpWindow::cancelDownload()
{
    statusLabel->setText(tr("Download canceled."));
    httpRequestAborted = true;
    reply->abort();
    downloadButton->setEnabled(true);
}

void HttpWindow::httpFinished()
{
    QFileInfo fi;
    if (file)
    {
        fi.setFile(file->fileName());
        file->close();
        file.reset();
    }

    QNetworkReply::NetworkError error = reply->error();
    const QString &errorString = reply->errorString();
    reply.reset();
    if (error != QNetworkReply::NoError)
    {
        QFile::remove(fi.absoluteFilePath());
        if (!httpRequestAborted)
        {
            statusLabel->setText(tr("Download failed:\n%1.").arg(errorString));
            downloadButton->setEnabled(true);
        }
        return;
    }

    statusLabel->setText(tr("Downloaded %1 bytes to %2\nin\n%3")
                         .arg(fi.size())
                         .arg(fi.fileName(), QDir::toNativeSeparators(fi.absoluteFilePath())));
    if (launchCheckBox->isChecked())
        QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absoluteFilePath()));
    downloadButton->setEnabled(true);
}

void HttpWindow::httpReadyRead()
{
    if (file)
        file->write(reply->readAll());
}

void HttpWindow::enableDownloadButton()
{
    downloadButton->setEnabled(!urlLineEdit->text().isEmpty());
}

void HttpWindow::slotAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator)
{
    QDialog authenticationDialog;
    Ui::Dialog ui;
    ui.setupUi(&authenticationDialog);
    authenticationDialog.adjustSize();
    ui.siteDescription->setText(tr("%1 at %2").arg(authenticator->realm(), url.host()));
    ui.userEdit->setText(url.userName());
    ui.passwordEdit->setText(url.password());

    if (authenticationDialog.exec() == QDialog::Accepted)
    {
        authenticator->setUser(ui.userEdit->text());
        authenticator->setPassword(ui.passwordEdit->text());
    }
}

#if QT_CONFIG(ssl)
void HttpWindow::sslErrors(const QList<QSslError> &errors)
{
    QString errorString;
    for (const QSslError &error : errors)
    {
        if (!errorString.isEmpty())
            errorString += '\n';
        errorString += error.errorString();
    }

    if (QMessageBox::warning(this, tr("SSL Errors"),
                             tr("One or more SSL errors has occured:\n%1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore)
    {
        reply->ignoreSslErrors();
    }
}
#endif













































