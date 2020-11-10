#include "fileinteractions.h"

fileInteractions::fileInteractions()
{

}

QString fileInteractions::calculateHashFromFile(QString sFile) {
    // Read hash of currently 'installed' arcdps
    QCryptographicHash crypto(QCryptographicHash::Md5);
    QFile currentDll(sFile);
    if (!currentDll.open(QFile::ReadOnly)) {
        std::cout << "Unable to read \"" << sFile.toStdString() << "\"" << std::endl;
        return "";
    }
    while(!currentDll.atEnd()){
        crypto.addData(currentDll.read(8192));
    }
    return crypto.result().toHex();
}

QString fileInteractions::readFullFileString(QString filename)
{
    QFile referenceFile(filename);
    if (!referenceFile.open(QFile::ReadOnly | QFile::Text)) {
        std::cout << "Could not open reference file" << std::endl;
        return "";
    }
    QTextStream streamer(&referenceFile);
    QString output = streamer.readAll();
    referenceFile.close();
    return output;
}

QString fileInteractions::readFirstFileLine(QString filename) {
    QFile referenceFile(filename);
    if (!referenceFile.open(QFile::ReadOnly | QFile::Text)) {
        std::cout << "Could not open reference file" << std::endl;
        return "";
    }
    QTextStream line(&referenceFile);
    QString stringLine = line.readLine();
    referenceFile.close();
    return stringLine;
}

int fileInteractions::writeFileString(QString filename, QString filecontent) {
    QFile targetFile(filename);
    if (!targetFile.open(QFile::WriteOnly | QFile::Text)) {
        std::cout << "Could not open target file" << std::endl;
        return 1;
    }
    QTextStream line(&targetFile);
    line << filecontent;
    targetFile.close();
    return 0;
}

int fileInteractions::removeFile(QString pathstring, QString filename) {
    if (QDir(pathstring).exists(filename)) {
        if (!pathstring.isEmpty()) {
            pathstring = pathstring + "/";
        }
        QFile toRemove(pathstring + filename);
        toRemove.remove();
    }

    return 0;
}

QString fileInteractions::getVersionString(QString fName)
{
    // first of all, GetFileVersionInfoSize
    DWORD dwHandle;
    DWORD dwLen = GetFileVersionInfoSize(fName.toStdWString().c_str(), &dwHandle);

    // GetFileVersionInfo
    LPVOID lpData = new BYTE[dwLen];
    if (!GetFileVersionInfo(fName.toStdWString().c_str(), dwHandle, dwLen, lpData))
    {
        qDebug() << "error in GetFileVersionInfo";
        delete[] lpData;
        return "";
    }

    // VerQueryValue
    VS_FIXEDFILEINFO* lpBuffer = NULL;
    UINT uLen;

    if (!VerQueryValue(lpData,
                       QString("\\").toStdWString().c_str(),
                       (LPVOID*)& lpBuffer,
                       &uLen))
    {

        qDebug() << "error in VerQueryValue";
        delete[] lpData;
        return "";
    }
    else
    {
        return QString::number((lpBuffer->dwFileVersionMS >> 16) & 0xffff) + "." +
                QString::number((lpBuffer->dwFileVersionMS) & 0xffff) + "." +
                QString::number((lpBuffer->dwFileVersionLS >> 16) & 0xffff) + "." +
                QString::number((lpBuffer->dwFileVersionLS) & 0xffff);
    }
}
