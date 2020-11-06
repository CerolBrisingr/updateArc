#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QtCore>
#include <QtNetwork>
#include <iostream>

#include <cstdio>

QT_BEGIN_NAMESPACE
class QSslError;
QT_END_NAMESPACE

// forward declarations
class Request;
class downloader;


enum type {
    file,
    fileNamed,
    htmlBody
};


class downloader : public QObject
{
    Q_OBJECT
public:
    downloader(bool print_debug = false);
    ~downloader();

    bool _print_debug;

    int fetch();
    void setTargetPath(QString filePath);
    bool allDownloadsDone();

    Request *addFileRequest(QString address, QString filename = "");
    Request *addTextRequest(QString address);
    void printRequests();
    void dropRequests();

    static int16_t singleDownload(QString address, QString filename = "");
    static QString singleTextRequest(QString address);

public slots:
    void processReply(QNetworkReply *netReply);
    void error(QNetworkReply::NetworkError err);
    void sslErrors(const QList<QSslError> &sslErrors);

private:
    QNetworkAccessManager netManager;
    QNetworkRequest request;
    QString targetPath = "";

    QEventLoop waitLoop;
    bool hasError;
    bool _will_shut_down = false;

    QVector<Request *> taskList;
    QVector<QNetworkReply *> currentDownloads;
    QVector<bool> receivedFlags;

    void addRequest(Request * newRequest);
    void errorMsg(std::string msg, bool bIsFatal = true);

};


class Request
{
public:
    Request(QString address, type requestType, QString filename)
        :_address(address)
        ,_requestType(requestType)
        ,_filename(filename)
    {}
    ~Request();

    QString getRequestString();
    QString getRequestAddress();
    QString getTargetFilename();
    type getRequestType();
    void setResult(int16_t error, QString output = "Success");
    QString getResult();
    int16_t getError();

private:
    QString _address;
    type _requestType;
    QString _output;
    QString _filename;
    int16_t _error;
};

#endif // DOWNLOADER_H
