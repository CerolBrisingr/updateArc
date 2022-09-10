#include "downloader.h"

Downloader::Downloader()
{
    QSettings setting(_setting_path, QSettings::IniFormat);
    _print_debug = setting.value("debug/downloader", "off").toString() == "on";

    if (_print_debug) {
    Log::write(QSslSocket::sslLibraryBuildVersionString() + "\n");
    }
    if (QSslSocket::supportsSsl()) {
        if (_print_debug) {
            Log::write("      Supporting SSL\n");
        }
    } else {
        if (_print_debug) {
            Log::write("      No support for SSL!\n");
        }
    }

    connect(&_net_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));

    // Errors
    connect(&_net_manager, &QNetworkAccessManager::authenticationRequired,
            this, [this]{errorMsg("authenticationRequired");});
    //connect(&netManager, &QNetworkAccessManager::encrypted,
    //        this, [this]{errorMsg("encrypted", false);});
    connect(&_net_manager, &QNetworkAccessManager::networkAccessibleChanged,
            this, [this]{errorMsg("networkAccessibleChanged");});
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

int Downloader::fetch()
{
    for (int i = 0; i < _taskList.length(); i++) {
        QUrl url = QUrl(_taskList[i]->getRequestAddress());

        if (_print_debug) {
            Log::write("      Starting request: " + url.url() + "\n");
        }
        _request = QNetworkRequest(url);

        _request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, _taskList[i]->getAllowedForwards() > 0);
        _request.setMaximumRedirectsAllowed(_taskList[i]->getAllowedForwards());

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
    if (_print_debug) {
        Log::write("      Recieved reply from URL: " + netReply->url().toString() + "\n");
    }
    int16_t _err = 0;

    int gotId =  _currentDownloads.indexOf(netReply);
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
                if (_print_debug) {
                    Log::write("      Download requested but filename empty!\n");
                }
                _err = 1;
            } else {
                // Target name not empty
                QString fullPath = _targetPath;
                fullPath.append(targetFilename);

                QFile file(fullPath);
                if (_print_debug) {
                    Log::write("      Saving file to: " + fullPath + "\n");
                }
                if (file.open(QFile::WriteOnly)) {
                    file.write(netReply->read(netReply->bytesAvailable()));
                    file.close();
                    if (_print_debug) {
                        Log::write("      File saved to: " + fullPath + "\n");
                    }
                } else {
                    if (_print_debug) {
                        Log::write("      Cannot write target file\n");
                    }
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
    bool flag = true;
     for(int count=0; count<_receivedFlags.length(); count++) {
         if(!_receivedFlags[count]) {
             flag = false;
             break;
         }
     }
     return flag;
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

QString Request::getRequestString()
{
    switch(_requestType) {
    case RequestType::STDFILE: return "File";
    case RequestType::NAMEDFILE: return "File with custom name";
    case RequestType::HTMLBODY: return "Text";
    }
    return "";
}

QString Request::getRequestAddress()
{
    return _address;
}

QString Request::getTargetFilename()
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

RequestType Request::getRequestType()
{
    return _requestType;
}

void Request::setResult(int16_t error, QString output)
{
    _error = error;
    _output = output;
}

QString Request::getResult()
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

int16_t Request::getError()
{
    return _error;
}

uint16_t Request::getAllowedForwards()
{
    return _forwards_allowed;
}
