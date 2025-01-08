#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QtCore>
#include <QtNetwork>
#include <memory>

#include "logger.h"

QT_BEGIN_NAMESPACE
class QSslError;
QT_END_NAMESPACE

// Differentiate requirements to individual requests
enum RequestType {
    STDFILE,
    NAMEDFILE,
    HTMLBODY
};

// Carries all data needed to perform one web interaction
class Request
{
public:
    Request(const QString address, const RequestType requestType, const QString filename = "", const uint16_t forwards = 3)
        :_address(address)
        ,_requestType(requestType)
        ,_filename(filename)
        ,_forwards_allowed(forwards)
    {}
    ~Request() {}

    QString getRequestString() const;
    QString getRequestAddress() const;
    QString getTargetFilename() const;
    RequestType getRequestType() const;
    void setResult(const int16_t error, const QString output = "Success");
    QString getResult() const;
    int16_t getError() const;
    uint16_t getAllowedForwards() const;

private:
    QString _address;
    RequestType _requestType;
    QString _output;
    QString _filename;
    int16_t _error;
    uint16_t _forwards_allowed;
};


class ActiveDownload
{
public:
    ActiveDownload(std::shared_ptr<Request>task, QNetworkReply* reply)
        :_task(task)
        ,_reply(reply)
        ,_wasReceived(false)
    {}
    ~ActiveDownload();

    bool relatesTo(QNetworkReply* reply) const;

    std::shared_ptr<Request> getTask() const;
    void received();
    bool isReceived() const;
private:
    std::shared_ptr<Request> _task;
    QNetworkReply* _reply;
    bool _wasReceived;
};


class Downloader : public QObject
{
    Q_OBJECT
public:
    // Debugging flag access
    static void setPrintDebug(const bool doDebug);
    static bool getPrintDebug();

    // Convenient downloads
    static int16_t singleDownload(QString address, QString pathname = "", QString filename = "", uint16_t forwards = 3);
    static int16_t singleTextRequest(QString &output, QString address, uint16_t forwardings = 3);

public:
    Downloader();
    ~Downloader() override;

    //bool _print_debug;

    int fetch();
    void setTargetPath(QString filePath);
    bool allDownloadsDone();

    std::shared_ptr<Request> addFileRequest(QString address, QString filename = "", uint16_t forwards = 3);
    std::shared_ptr<Request> addTextRequest(QString address, uint16_t forwards = 3);
    void printRequests();
    void dropRequests();

public slots:
    void processReply(QNetworkReply *netReply);
    void error(QNetworkReply::NetworkError err);
    void sslErrors(const QList<QSslError> &sslErrors);

private:
    inline static bool _print_debug = false; // Set up debug flag

    QNetworkAccessManager _net_manager;
    QString _targetPath = "";

    QEventLoop _waitLoop;
    bool _hasError;
    bool _will_shut_down = false;

    QVector<std::shared_ptr<Request>> _taskList;
    QVector<ActiveDownload> _activeDownloads;

    ActiveDownload* findCorrespondingDownload(QNetworkReply* reply);
    void addRequest(std::shared_ptr<Request> newRequest);
    void errorMsg(std::string msg, bool bIsFatal = true);
    void logDebug(QString msg);

};

#endif // DOWNLOADER_H
