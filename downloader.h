#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QtCore>
#include <QtNetwork>
//#include <QObject>
//#include <QNetworkAccessManager>
//#include <QFile>
//#include <QNetworkReply>
#include <iostream>
//#include <QSslConfiguration>
//#include <QAuthenticator>
//#include <openssl/opensslv.h>

#include <cstdio>

QT_BEGIN_NAMESPACE
class QSslError;
QT_END_NAMESPACE

class downloader : public QObject
{
    Q_OBJECT

public:
    downloader();
    ~downloader();

    void fetch(QString str_url);

public slots:
    void replyReady(QNetworkReply *netReply);
    void error(QNetworkReply::NetworkError err);
    void sslErrors(const QList<QSslError> &sslErrors);
    void timer();

signals:
    void test(std::string dummy);

private:
    QNetworkAccessManager netManager;
    QNetworkRequest request;
    QNetworkReply* reply;
    QVector<QNetworkReply *> currentDownloads;
    QWaitCondition receiving;
    QMutex mutex;

    void errorMsg(std::string msg);

};

#endif // DOWNLOADER_H
