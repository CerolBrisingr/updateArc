#include "downloader.h"

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
    for (const auto* task: _taskList) {
        QUrl url = QUrl(task->getRequestAddress());

        logDebug("      Starting request: " + url.url() + "\n");
        _request = QNetworkRequest(url);

        _request.setMaximumRedirectsAllowed(task->getAllowedForwards());

        _hasError = false;
        QNetworkReply* reply = _net_manager.get(_request);

#if QT_CONFIG(ssl)
        connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
                SLOT(sslErrors(QList<QSslError>)));
#endif

        _currentDownloads.append(reply);
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

void Downloader::addRequest(Request *newRequest)
{
    _receivedFlags.append(false);
    _taskList.append(newRequest);
}

void Downloader::processReply(QNetworkReply* netReply)
{
    logDebug("      Recieved reply from URL: " + netReply->url().toString() + "\n");
    int16_t _err = 0;

    qsizetype gotId = _currentDownloads.indexOf(netReply);
    _receivedFlags[gotId] = true;

    QUrl url = netReply->url();
    if (netReply->error()) {
        // Request failed
        fprintf(stderr, "      Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(netReply->errorString()));
        _err = 1;
    } else {
        // Request successful
        QString targetFilename =_taskList[gotId]->getTargetFilename();
        if (_taskList[gotId]->getRequestType() == RequestType::HTMLBODY) {
            // Write response to QString
            QString _output;
            QTextStream streamer(&_output);
            streamer << netReply->read(netReply->bytesAvailable());
            _taskList[gotId]->setResult(_err, _output);
        } else {
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
            _taskList[gotId]->setResult(_err);
        }
    }

    if (_err != 0) {
        _hasError = true;
    }

    netReply->deleteLater();

    if (allDownloadsDone()) {
        _receivedFlags.clear();
        _currentDownloads.clear();
        _taskList.clear();
        _waitLoop.exit(_hasError);
    }
}

bool Downloader::allDownloadsDone() {
    for(const auto& isReceived: _receivedFlags) {
        if(!isReceived) {
         return false;
        }
    }
    return true;
}

Request *Downloader::addFileRequest(QString address, QString filename, uint16_t forwards)
{
    RequestType requestType;
    if (filename.isEmpty()) {
        requestType = RequestType::STDFILE;
    } else {
        requestType = RequestType::NAMEDFILE;
    }

    Request* newRequest = new Request(address, requestType, filename, forwards);
    addRequest(newRequest);
    return newRequest;
}

Request *Downloader::addTextRequest(QString address, uint16_t forwards)
{
    RequestType requestType = RequestType::HTMLBODY;
    Request* newRequest = new Request(address, requestType, "", forwards);
    addRequest(newRequest);
    return newRequest;
}

void Downloader::printRequests()
{
    for (int count = 0; count < _taskList.length(); count++) {
        Log::write(_taskList[count]->getRequestString() + "\n");
    }
}

void Downloader::dropRequests()
{
    _receivedFlags.clear();
    for (int count = 0; count < _taskList.length(); count++) {
        delete(&_taskList[count]);
    }
    _taskList.clear();
}

int16_t Downloader::singleDownload(QString address, QString pathname, QString filename, uint16_t forwards)
{
    Downloader netSource;
    netSource.setTargetPath(pathname);
    Request* request = netSource.addFileRequest(address, filename, forwards);
    netSource.fetch();

    int16_t error_msg = request->getError();
    delete request;
    return error_msg;
}

int16_t Downloader::singleTextRequest(QString &output, QString address, uint16_t forwardings)
{
    Downloader netSource;
    Request* request = netSource.addTextRequest(address, forwardings);
    netSource.fetch();

    output = request->getResult();
    int16_t error_msg = request->getError();
    delete request;
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

QString Request::getRequestString() const
{
    switch(_requestType) {
        case RequestType::STDFILE: return "File";
        case RequestType::NAMEDFILE: return "File with custom name";
        case RequestType::HTMLBODY: return "Text";
    }
    return "";
}

QString Request::getRequestAddress() const
{
    return _address;
}

QString Request::getTargetFilename() const
{
    switch (_requestType) {
        case RequestType::STDFILE:
            return QFileInfo(_address).fileName();
        case RequestType::NAMEDFILE:
            return _filename;
        case RequestType::HTMLBODY:
            return "";
    }
    return "";
}

RequestType Request::getRequestType() const
{
    return _requestType;
}

void Request::setResult(const int16_t error, const QString output)
{
    _error = error;
    _output = output;
}

QString Request::getResult() const
{
    if (_error == 0) {
        QString retString;
        QTextStream streamer(&retString);
        streamer << _output;
        return retString;
    } else {
        return "Error";
    }
}

int16_t Request::getError() const
{
    return _error;
}

uint16_t Request::getAllowedForwards() const
{
    return _forwards_allowed;
}
