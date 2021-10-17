#include "downloader.h"

downloader::downloader()
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

    connect(&netManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));

    // Errors
    connect(&netManager, &QNetworkAccessManager::authenticationRequired,
            this, [this]{errorMsg("authenticationRequired");});
    //connect(&netManager, &QNetworkAccessManager::encrypted,
    //        this, [this]{errorMsg("encrypted", false);});
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
            this, [this]{errorMsg("network manager terminated", false);});
}

downloader::~downloader()
{
    _will_shut_down = true;
}

int downloader::fetch()
{
    for (int i = 0; i < taskList.length(); i++) {
        QUrl url = QUrl(taskList[i]->getRequestAddress());

        if (_print_debug) {
            Log::write("      Starting request: " + url.url() + "\n");
        }
        request = QNetworkRequest(url);

        request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, taskList[i]->getAllowedForwards() > 0);
        request.setMaximumRedirectsAllowed(taskList[i]->getAllowedForwards());

        hasError = false;
        QNetworkReply* reply = netManager.get(request);

#if QT_CONFIG(ssl)
        connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
                SLOT(sslErrors(QList<QSslError>)));
#endif

        currentDownloads.append(reply);
    }

    return waitLoop.exec();
}

void downloader::setTargetPath(QString filePath)
{
    if (filePath.isEmpty()) {
        this->targetPath = "";
        return;
    } else if (!filePath.endsWith('/')) {
        filePath.append('/');
    }

    this->targetPath = filePath;
}

void downloader::sslErrors(const QList<QSslError> &sslErrors)
{
#if QT_CONFIG(ssl)
    for (const QSslError &error : sslErrors)
        fprintf(stderr, "      SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
    waitLoop.exit(1);
}

void downloader::addRequest(Request *newRequest)
{
    receivedFlags.append(false);
    taskList.append(newRequest);
}

void downloader::processReply(QNetworkReply* netReply)
{
    if (_print_debug) {
        Log::write("      Recieved reply from URL: " + netReply->url().toString() + "\n");
    }
    int16_t _err = 0;

    int gotId =  currentDownloads.indexOf(netReply);
    receivedFlags[gotId] = true;

    QUrl url = netReply->url();
    if (netReply->error()) {
        // Request failed
        fprintf(stderr, "      Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(netReply->errorString()));
        _err = 1;
    } else {
        // Request successful
        QString targetFilename =taskList[gotId]->getTargetFilename();
        if (taskList[gotId]->getRequestType() == type::htmlBody) {
            // Write response to QString
            QString _output;
            QTextStream streamer(&_output);
            streamer << netReply->read(netReply->bytesAvailable());
            taskList[gotId]->setResult(_err, _output);
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
                QString fullPath = this->targetPath;
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
            taskList[gotId]->setResult(_err);
        }
    }

    if (_err != 0) {
        hasError = true;
    }

    netReply->deleteLater();

    if (allDownloadsDone()) {
        receivedFlags.clear();
        currentDownloads.clear();
        taskList.clear();
        waitLoop.exit(hasError);
    }
}

bool downloader::allDownloadsDone() {
    bool flag = true;
     for(int count=0; count<receivedFlags.length(); count++) {
         if(!receivedFlags[count]) {
             flag = false;
             break;
         }
     }
     return flag;
}

Request *downloader::addFileRequest(QString address, QString filename, uint16_t forwards)
{
    type requestType;
    if (filename.isEmpty()) {
        requestType = type::file;
    } else {
        requestType = type::fileNamed;
    }

    Request* newRequest = new Request(address, requestType, filename, forwards);
    addRequest(newRequest);
    return newRequest;
}

Request *downloader::addTextRequest(QString address, uint16_t forwards)
{
    type requestType = type::htmlBody;
    Request* newRequest = new Request(address, requestType, "", forwards);
    addRequest(newRequest);
    return newRequest;
}

void downloader::printRequests()
{
    for (int count = 0; count < taskList.length(); count++) {
        Log::write(taskList[count]->getRequestString() + "\n");
    }
}

void downloader::dropRequests()
{
    receivedFlags.clear();
    for (int count = 0; count < taskList.length(); count++) {
        delete(&taskList[count]);
    }
    taskList.clear();
}

int16_t downloader::singleDownload(QString address, QString pathname, QString filename, uint16_t forwards)
{
    downloader netSource;
    netSource.setTargetPath(pathname);
    Request* request = netSource.addFileRequest(address, filename, forwards);
    netSource.fetch();

    int16_t error_msg = request->getError();
    delete request;
    return error_msg;
}

int16_t downloader::singleTextRequest(QString &output, QString address, uint16_t forwardings)
{
    downloader netSource;
    Request* request = netSource.addTextRequest(address, forwardings);
    netSource.fetch();

    output = request->getResult();
    int16_t error_msg = request->getError();
    delete request;
    return error_msg;
}

void downloader::error(QNetworkReply::NetworkError err)
{
    errorMsg("network error");
    Q_UNUSED(err)
    waitLoop.exit(1);
}

void downloader::errorMsg(std::string msg, bool bIsFatal)
{
    if (!_will_shut_down) {
        Log::write(msg + "\n");
    }
    if (bIsFatal) {
        waitLoop.exit(1);
    }
}

QString Request::getRequestString()
{
    switch(this->_requestType) {
    case type::file: return "File";
    case type::fileNamed: return "File with custom name";
    case type::htmlBody: return "Text";
    }
    return "";
}

QString Request::getRequestAddress()
{
    return this->_address;
}

QString Request::getTargetFilename()
{
    switch (this->_requestType) {
    case type::file:
        return QFileInfo(this->_address).fileName();
    case type::fileNamed:
        return this->_filename;
    case type::htmlBody:
        return "";
    }
    return "";
}

type Request::getRequestType()
{
    return this->_requestType;
}

void Request::setResult(int16_t error, QString output)
{
    this->_error = error;
    this->_output = output;
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
    return this->_error;
}

uint16_t Request::getAllowedForwards()
{
    return this->_forwards_allowed;
}
