#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QtCore>
#include <QtNetwork>
#include <iostream>

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

    int fetch(QString strUrl);
    int fetch(QVector<QString> strUrlList);
    void setTargetPath(QString filePath);

public slots:
    void processReply(QNetworkReply *netReply);
    void error(QNetworkReply::NetworkError err);
    void sslErrors(const QList<QSslError> &sslErrors);

private:
    QNetworkAccessManager netManager;
    QNetworkRequest request;
    QVector<QNetworkReply *> currentDownloads;
    QString targetPath = "";

    QEventLoop waitLoop;

    void errorMsg(std::string msg, bool bIsFatal = true);

};

#endif // DOWNLOADER_H
