#include "downloader.h"

downloader::downloader()
{

    //QSslConfiguration sslConfiguration(QSslConfiguration::defaultConfiguration());

    connect(&netManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyReady(QNetworkReply*)));

    // Errors
    connect(&netManager, &QNetworkAccessManager::authenticationRequired,
            this, [this]{errorMsg("authenticationRequired");});
    connect(&netManager, &QNetworkAccessManager::encrypted,
            this, [this]{errorMsg("encrypted");});
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

    //connect(this, &downloader::test, this, [this]{errorMsg("Test");});

}

downloader::~downloader()
{
}

void downloader::fetch(QString str_url)
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

    std::cout << "Before timer" << std::endl;
    QTimer::singleShot(500, this, SLOT(timer()));
    std::cout << "After timer call" << std::endl;
    currentDownloads.append(reply);
    //connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
    //            this, SLOT(error(QNetworkReply::NetworkError)));
    //connect(reply, &QNetworkReply::finished,
    //            this, [this]{errorMsg("finished reply");});

    emit test("something");
    //mutex.lock();
    //receiving.wait(&mutex);
    //mutex.unlock();
}

void downloader::sslErrors(const QList<QSslError> &sslErrors)
{
#if QT_CONFIG(ssl)
    for (const QSslError &error : sslErrors)
        fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
}

void downloader::timer()
{
    std::cout << "Timeout!" << std::endl;
    receiving.wakeAll();
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

    receiving.wakeAll();
}

void downloader::error(QNetworkReply::NetworkError err)
{
    errorMsg("network error");
    Q_UNUSED(err);
    receiving.wakeAll();
}

void downloader::errorMsg(std::string msg)
{
    std::cout << msg << std::endl;
    if (msg != "destroyed") {
        receiving.wakeAll();
    }
}
