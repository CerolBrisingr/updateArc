#include "downloader.h"

downloader::downloader()
{

    connect(&netManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyReady(QNetworkReply*)));

    // Errors
    connect(&netManager, &QNetworkAccessManager::authenticationRequired,
            this, [this]{errorMsg("authenticationRequired");});
    connect(&netManager, &QNetworkAccessManager::encrypted,
            this, [this]{errorMsg("encrypted", false);});
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
            this, [this]{errorMsg("destroyed");});

}

downloader::~downloader()
{
}

void downloader::fetch(const QString str_url)
{

    std::cout << QSslSocket::sslLibraryBuildVersionString().toStdString() << std::endl;
    if (QSslSocket::supportsSsl()) {
        std::cout << "Supporting SSL" << std::endl;
    } else {
        std::cout << "No support for SSL!" << std::endl;
    }

    QUrl url = QUrl(str_url);

    std::cout << "Starting request: " << url.url().toStdString() << std::endl;
    request = QNetworkRequest(url);
    reply = netManager.get(request);

#if QT_CONFIG(ssl)
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
            SLOT(sslErrors(QList<QSslError>)));
#endif

    currentDownloads.append(reply);

    loop.exec();
}

void downloader::sslErrors(const QList<QSslError> &sslErrors)
{
#if QT_CONFIG(ssl)
    for (const QSslError &error : sslErrors)
        fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
    QCoreApplication::instance()->quit();
}

void downloader::timer()
{
    std::cout << "Timeout!" << std::endl;
    QCoreApplication::instance()->quit();
}

void downloader::replyReady(QNetworkReply* netReply)
{
    std::cout << "slot triggered" << std::endl;

    QFile file("C:/Users/Sebastian/Downloads/test/downloadedFile.html");
    file.open(QFile::WriteOnly);

    std::cout << "URL: " << netReply->url().toString().toStdString() << std::endl;

    file.write(netReply->read(netReply->bytesAvailable()));

    file.close();
    netReply->deleteLater();

    loop.exit();
}

void downloader::error(QNetworkReply::NetworkError err)
{
    errorMsg("network error");
    Q_UNUSED(err);
    QCoreApplication::instance()->quit();
}

void downloader::errorMsg(std::string msg, bool err)
{
    std::cout << msg << std::endl;
    if (err) {
        QCoreApplication::instance()->quit();
    }
}
