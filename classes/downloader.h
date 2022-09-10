#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QtCore>
#include <QtNetwork>

#include <cstdio>
#include "logger.h"

QT_BEGIN_NAMESPACE
class QSslError;
QT_END_NAMESPACE

// forward declarations
class Request;
class Downloader;


enum RequestType {
    _file,
    _fileNamed,
    _htmlBody
};


class Downloader : public QObject
{
    Q_OBJECT
public:
    static int16_t singleDownload(QString address, QString pathname = "", QString filename = "", uint16_t forwards = 3);
    static int16_t singleTextRequest(QString &output, QString address, uint16_t forwardings = 3);

public:
    Downloader();
    ~Downloader() override;

    bool _print_debug;

    int fetch();
    void setTargetPath(QString filePath);
    bool allDownloadsDone();

    Request *addFileRequest(QString address, QString filename = "", uint16_t forwards = 3);
    Request *addTextRequest(QString address, uint16_t forwards = 3);
    void printRequests();
    void dropRequests();

public slots:
    void processReply(QNetworkReply *netReply);
    void error(QNetworkReply::NetworkError err);
    void sslErrors(const QList<QSslError> &sslErrors);

private:
    QNetworkAccessManager _net_manager;
    QNetworkRequest _request;
    QString _targetPath = "";
    QString _setting_path = "settings.ini";

    QEventLoop _waitLoop;
    bool _hasError;
    bool _will_shut_down = false;

    QVector<Request *> _taskList;
    QVector<QNetworkReply *> _currentDownloads;
    QVector<bool> _receivedFlags;

    void addRequest(Request * newRequest);
    void errorMsg(std::string msg, bool bIsFatal = true);

};


class Request
{
public:
    Request(QString address, RequestType requestType, QString filename, uint16_t forwards = 3)
        :_address(address)
        ,_requestType(requestType)
        ,_filename(filename)
        ,_forwards_allowed(forwards)
    {}
    ~Request() {}

    QString getRequestString();
    QString getRequestAddress();
    QString getTargetFilename();
    RequestType getRequestType();
    void setResult(int16_t error, QString output = "Success");
    QString getResult();
    int16_t getError();
    uint16_t getAllowedForwards();

private:
    QString _address;
    RequestType _requestType;
    QString _output;
    QString _filename;
    int16_t _error;
    uint16_t _forwards_allowed;
};

#endif // DOWNLOADER_H
