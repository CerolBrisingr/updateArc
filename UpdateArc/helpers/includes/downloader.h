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
enum DownloadType {
    STDFILE,
    NAMEDFILE,
    HTMLBODY
};

// Carries all data needed to perform one web interaction
class DownloadRequest
{
public:
    DownloadRequest(const QString address, const DownloadType requestType, const QString filename = "", const uint16_t forwards = 3)
        :_address(address)
        ,_requestType(requestType)
        ,_filename(filename)
        ,_forwards_allowed(forwards)
    {}
    ~DownloadRequest() {}

    QString getRequestString() const;
    QString getRequestAddress() const;
    QString getTargetFilename() const;
    DownloadType getRequestType() const;
    void setResult(const int16_t error, const QString output = "Success");
    QString getResult() const;
    int16_t getError() const;
    uint16_t getAllowedForwards() const;

private:
    QString _address;
    DownloadType _requestType;
    QString _output;
    QString _filename;
    int16_t _error;
    uint16_t _forwards_allowed;
};


class DownloadContainer
{
public:
    DownloadContainer(std::shared_ptr<DownloadRequest>specification)
        :_specification(specification)
        ,_networkReply(nullptr)
        ,_wasReceived(false)
    {}
    ~DownloadContainer();

    void setNetworkReply(QNetworkReply* reply);
    bool relatesTo(QNetworkReply* reply) const;

    std::shared_ptr<DownloadRequest> getSpecification() const;
    void markReceived();
    bool isReceived() const;
private:
    std::shared_ptr<DownloadRequest> _specification;
    QNetworkReply* _networkReply;
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

    std::shared_ptr<DownloadRequest> addFileRequest(QString address, QString filename = "", uint16_t forwards = 3);
    std::shared_ptr<DownloadRequest> addTextRequest(QString address, uint16_t forwards = 3);
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

    //QVector<std::shared_ptr<Request>> _taskList;
    QVector<DownloadContainer> _downloadTasks;

    DownloadContainer* findCorrespondingDownload(QNetworkReply* reply);
    void addRequest(std::shared_ptr<DownloadRequest> newRequest);
    void errorMsg(std::string msg, bool bIsFatal = true);
    void logDebug(QString msg);

};

#endif // DOWNLOADER_H
