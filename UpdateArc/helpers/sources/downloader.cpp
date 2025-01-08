#include "downloader.h"


DownloadContainer::~DownloadContainer() {
    if (_networkReply != nullptr) {
        _networkReply->deleteLater();
    }
}

void DownloadContainer::markReceived() {
    _wasReceived = true;
}

bool DownloadContainer::isReceived() const {
    return _wasReceived;
}

void DownloadContainer::setNetworkReply(QNetworkReply* reply) {
    _networkReply = reply;
}

bool DownloadContainer::relatesTo(QNetworkReply* reply) const {
    return reply == _networkReply;
}

std::shared_ptr<DownloadRequest> DownloadContainer::getSpecification() const {
    return _specification;
}

Downloader::Downloader()
{
    logDebug(QSslSocket::sslLibraryBuildVersionString() + "\n");
    if (QSslSocket::supportsSsl()) {
        logDebug("      Supporting SSL\n");
    } else {
        logDebug("      No support for SSL!\n");
    }

    connect(&_net_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));

    // Errors
    connect(&_net_manager, &QNetworkAccessManager::authenticationRequired,
            this, [this]{errorMsg("authenticationRequired");});
    connect(&_net_manager, &QNetworkAccessManager::preSharedKeyAuthenticationRequired,
            this, [this]{errorMsg("preSharedKeyAuthenticationRequired");});
    connect(&_net_manager, &QNetworkAccessManager::proxyAuthenticationRequired,
            this, [this]{errorMsg("proxyAuthenticationRequired");});
    connect(&_net_manager, &QNetworkAccessManager::sslErrors,
            this, [this]{errorMsg("sslErrors");});
    connect(&_net_manager, &QObject::objectNameChanged,
            this, [this]{errorMsg("objectNameChanged");});
    connect(&_net_manager, &QObject::destroyed,
            this, [this]{errorMsg("network manager terminated", false);});
}

Downloader::~Downloader()
{
    _will_shut_down = true;
}

void Downloader::setPrintDebug(const bool doDebug)
{
    _print_debug = doDebug;
}

bool Downloader::getPrintDebug()
{
    return _print_debug;
}

void Downloader::logDebug(QString msg)
{
    if(_print_debug) {
        Log::write(msg);
    }
}

int Downloader::fetch()
{
    _hasError = false;
    for (auto& task: _downloadTasks) {
        const auto& specification = task.getSpecification();
        QUrl url = QUrl(specification->getRequestAddress());

        logDebug("      Starting request: " + url.url() + "\n");
        auto qRequest = QNetworkRequest(url);

        qRequest.setMaximumRedirectsAllowed(specification->getAllowedForwards());

        QNetworkReply* reply = _net_manager.get(qRequest);

#if QT_CONFIG(ssl)
        connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
                SLOT(sslErrors(QList<QSslError>)));
#endif
        task.setNetworkReply(reply);
    }

    return _waitLoop.exec();
}

void Downloader::setTargetPath(QString filePath)
{
    if (filePath.isEmpty()) {
        _targetPath = "";
        return;
    } else if (!filePath.endsWith('/')) {
        filePath.append('/');
    }

    _targetPath = filePath;
}

void Downloader::sslErrors(const QList<QSslError> &sslErrors)
{
#if QT_CONFIG(ssl)
    for (const QSslError &error : sslErrors)
        fprintf(stderr, "      SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
    _waitLoop.exit(1);
}

DownloadContainer* Downloader::findCorrespondingDownload(QNetworkReply* reply) {
    for (auto& task: _downloadTasks) {
        if (task.relatesTo(reply)) {
            return &task;
        }
    }
    throw std::invalid_argument("no entry relies to given reply pointer");
}

void Downloader::addRequest(std::shared_ptr<DownloadRequest> newRequest)
{
    _downloadTasks.emplaceBack(newRequest);
}

void Downloader::processReply(QNetworkReply* netReply)
{
    logDebug("      Recieved reply from URL: " + netReply->url().toString() + "\n");
    int16_t _err = 0;

    auto* activeDownload = findCorrespondingDownload(netReply);
    activeDownload->markReceived();
    auto specification = activeDownload->getSpecification();

    QUrl url = netReply->url();
    if (netReply->error()) {
        // Request failed
        fprintf(stderr, "      Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(netReply->errorString()));
        _err = 1;
    } else {
        // Request successful
        if (specification->getRequestType() == DownloadType::HTMLBODY) {
            // Write response to QString
            QString _output;
            QTextStream streamer(&_output);
            streamer << netReply->read(netReply->bytesAvailable());
            specification->setResult(_err, _output);
        } else {
            QString targetFilename =specification->getTargetFilename();
            // Write response to file
            if (targetFilename.isEmpty()) {
                // Invalid target name
                logDebug("      Download requested but filename empty!\n");
                _err = 1;
            } else {
                // Target name not empty
                QString fullPath = _targetPath;
                fullPath.append(targetFilename);

                QFile file(fullPath);
                logDebug("      Saving file to: " + fullPath + "\n");
                if (file.open(QFile::WriteOnly)) {
                    file.write(netReply->read(netReply->bytesAvailable()));
                    file.close();
                    logDebug("      File saved to: " + fullPath + "\n");
                } else {
                    logDebug("      Cannot write target file\n");
                    _err = 1;
                }
            }
            specification->setResult(_err);
        }
    }

    if (_err != 0) {
        _hasError = true;
    }

    if (allDownloadsDone()) {
        _downloadTasks.clear();
        _waitLoop.exit(_hasError);
    }
}

bool Downloader::allDownloadsDone() {
    for(const auto& activeDownload: _downloadTasks) {
        if(!activeDownload.isReceived()) {
         return false;
        }
    }
    return true;
}

std::shared_ptr<DownloadRequest> Downloader::addFileRequest(QString address, QString filename, uint16_t forwards)
{
    DownloadType requestType;
    if (filename.isEmpty()) {
        requestType = DownloadType::STDFILE;
    } else {
        requestType = DownloadType::NAMEDFILE;
    }

    auto newRequest = std::make_shared<DownloadRequest>(address, requestType, filename, forwards);
    addRequest(newRequest);
    return newRequest;
}

std::shared_ptr<DownloadRequest> Downloader::addTextRequest(QString address, uint16_t forwards)
{
    DownloadType requestType = DownloadType::HTMLBODY;
    auto newRequest = std::make_shared<DownloadRequest>(address, requestType, "", forwards);
    addRequest(newRequest);
    return newRequest;
}

void Downloader::printRequests()
{
    for (const auto& download: _downloadTasks) {
        Log::write(download.getSpecification()->getRequestString() + "\n");
    }
}

void Downloader::dropRequests()
{
    _downloadTasks.clear();
}

int16_t Downloader::singleDownload(QString address, QString pathname, QString filename, uint16_t forwards)
{
    Downloader netSource;
    netSource.setTargetPath(pathname);
    auto request = netSource.addFileRequest(address, filename, forwards);
    netSource.fetch();

    int16_t error_msg = request->getError();
    return error_msg;
}

int16_t Downloader::singleTextRequest(QString &output, QString address, uint16_t forwardings)
{
    Downloader netSource;
    auto request = netSource.addTextRequest(address, forwardings);
    netSource.fetch();

    output = request->getResult();
    int16_t error_msg = request->getError();
    return error_msg;
}

void Downloader::error(QNetworkReply::NetworkError err)
{
    errorMsg("network error");
    Q_UNUSED(err)
    _waitLoop.exit(1);
}

void Downloader::errorMsg(std::string msg, bool bIsFatal)
{
    if (!_will_shut_down) {
        Log::write(msg + "\n");
    }
    if (bIsFatal) {
        _waitLoop.exit(1);
    }
}

QString DownloadRequest::getRequestString() const
{
    switch(_requestType) {
        case DownloadType::STDFILE: return "File";
        case DownloadType::NAMEDFILE: return "File with custom name";
        case DownloadType::HTMLBODY: return "Text";
    }
    return "";
}

QString DownloadRequest::getRequestAddress() const
{
    return _address;
}

QString DownloadRequest::getTargetFilename() const
{
    switch (_requestType) {
        case DownloadType::STDFILE:
            return QFileInfo(_address).fileName();
        case DownloadType::NAMEDFILE:
            return _filename;
        case DownloadType::HTMLBODY:
            return "";
    }
    return "";
}

DownloadType DownloadRequest::getRequestType() const
{
    return _requestType;
}

void DownloadRequest::setResult(const int16_t error, const QString output)
{
    _error = error;
    _output = output;
}

QString DownloadRequest::getResult() const
{
    if (_error == 0) {
        QString returnString;
        QTextStream streamer(&returnString);
        streamer << _output;
        return returnString;
    } else {
        return "Error";
    }
}

int16_t DownloadRequest::getError() const
{
    return _error;
}

uint16_t DownloadRequest::getAllowedForwards() const
{
    return _forwards_allowed;
}
