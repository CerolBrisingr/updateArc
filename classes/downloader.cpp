#include "downloader.h"

downloader::downloader()
{

    std::cout << QSslSocket::sslLibraryBuildVersionString().toStdString() << std::endl;
    if (QSslSocket::supportsSsl()) {
        std::cout << "Supporting SSL" << std::endl;
    } else {
        std::cout << "No support for SSL!" << std::endl;
    }

    connect(&netManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));

    // Errors
    connect(&netManager, &QNetworkAccessManager::authenticationRequired,
            this, [this]{errorMsg("authenticationRequired");});
    //connect(&netManager, &QNetworkAccessManager::encrypted,
    //        this, [this]{errorMsg("encrypted", false);});
    connect(&netManager, &QNetworkAccessManager::networkAccessibleChanged,
            this, [this]{errorMsg("networkAccessibleChanged");});
    connect(&netManager, &QNetworkAccessManager::preSharedKeyAuthenticationRequired,
            this, [this]{errorMsg("preSharedKeyAuthenticationRequired");});
    connect(&netManager, &QNetworkAccessManager::proxyAuthenticationRequired,
            this, [this]{errorMsg("proxyAuthenticationRequired");});
    connect(&netManager, &QNetworkAccessManager::sslErrors,
            this, [this]{errorMsg("sslErrors");});
    connect(&netManager, &QObject::objectNameChanged,
            this, [this]{errorMsg("objectNameChanged");});
    connect(&netManager, &QObject::destroyed,
            this, [this]{errorMsg("network manager terminated", false);});

}

downloader::~downloader()
{
}

int downloader::fetch(QString strUrl) {

    // Pack link in list to call processing function
    QVector<QString> strUrlList;
    strUrlList.append(strUrl);

    return this->fetch(strUrlList);
}

int downloader::fetch(QVector<QString> strUrlList)
{

    for (int i = 0; i < strUrlList.size(); i++) {
        QUrl url = QUrl(strUrlList[i]);

        std::cout << "Starting request: " << url.url().toStdString() << std::endl;
        request = QNetworkRequest(url);
        QNetworkReply* reply = netManager.get(request);

#if QT_CONFIG(ssl)
        connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
                SLOT(sslErrors(QList<QSslError>)));
#endif

        currentDownloads.append(reply);
    }

    return waitLoop.exec();
}

void downloader::setTargetPath(QString filePath)
{
    if (filePath.isEmpty()) {
        this->targetPath = "";
        return;
    } else if (!filePath.endsWith('/')) {
        filePath.append('/');
    }

    this->targetPath = filePath;

}

void downloader::sslErrors(const QList<QSslError> &sslErrors)
{
#if QT_CONFIG(ssl)
    for (const QSslError &error : sslErrors)
        fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
    waitLoop.exit(1);
}

void downloader::processReply(QNetworkReply* netReply)
{

    std::cout << "Recieved reply from URL: " << netReply->url().toString().toStdString() << std::endl;

    QUrl url = netReply->url();
    if (netReply->error()) {
        fprintf(stderr, "Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(netReply->errorString()));
        waitLoop.exit(1);
    }

    QString targetFilename = QFileInfo(url.path()).fileName();
    if (targetFilename.isEmpty()) {
        std::cout << "Cannot resolve target filename" << std::endl;
        waitLoop.exit(1);
    }

    QString fullPath = this->targetPath;
    fullPath.append(targetFilename);

    QFile file(fullPath);
    if (!file.open(QFile::WriteOnly)) {
        std::cout << "Cannot write target file" << std::endl;
        waitLoop.exit(1);
    }

    file.write(netReply->read(netReply->bytesAvailable()));
    file.close();
    std::cout << "File saved to: " << fullPath.toStdString() << std::endl;

    currentDownloads.removeAll(netReply);
    netReply->deleteLater();

    if (currentDownloads.isEmpty()) {
        waitLoop.exit();
    }
}

void downloader::error(QNetworkReply::NetworkError err)
{
    errorMsg("network error");
    Q_UNUSED(err)
    waitLoop.exit(1);
}

void downloader::errorMsg(std::string msg, bool bIsFatal)
{
    std::cout << msg << std::endl;
    if (bIsFatal) {
        waitLoop.exit(1);
    }
}
