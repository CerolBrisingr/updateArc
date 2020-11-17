#include "fileinteractions.h"


bool fileInteractions::find7zip(QString &path) {
    if (!searchPathAt("HKEY_CURRENT_USER\\SOFTWARE\\7-Zip", path)) {
        if (!searchPathAt("HKEY_LOCAL_MACHINE\\SOFTWARE\\7-Zip", path)) {
            return false;
        }
    }
    if (QDir(path).exists("7z.exe")) {
        path += "7z.exe";
        return true;
    }
    return false;
}

bool fileInteractions::searchPathAt(QString key, QString &path) {
    QSettings zipRegistry(key, QSettings::NativeFormat);
    if (zipRegistry.contains("Path")) {
        path = zipRegistry.value("Path").toString();
        return true;
    } else if (zipRegistry.contains("Path64")) {
        path = zipRegistry.value("Path64").toString();
        return true;
    }
    return false;
}


bool fileInteractions::extractWith7zip(QString archivePath, QString outputName) {
    QProcess use7zip;
    QStringList arguments;
    QString sevenZipPath;
    find7zip(sevenZipPath);
    use7zip.setProgram(sevenZipPath);
    arguments << "x" << archivePath << "-o" + outputName;
    use7zip.setArguments(arguments);
    use7zip.start();
    if (!use7zip.waitForStarted()) {
        std::cout << "      Program start failed" << std::endl;
    }
    else {
        std::cout << "      Program started" << std::endl;
        if (!use7zip.waitForFinished()) {
            std::cout << "      Something else went wrong" << std::endl;
            return false;
        }
        else
            std::cout << "      Program finished" << std::endl;
    }
    QString output = use7zip.readAllStandardOutput();
    return output.contains("Everything is Ok");
}


bool fileInteractions::executeExternalWaiting(QString executablePath, QString working_directory) {
    QProcess m_agent;
    m_agent.setWorkingDirectory(working_directory);
    QStringList args = QStringList();
    args = QStringList({"-Command", QString("Start-Process %1 -Verb runAs -Wait").arg(executablePath)});
    m_agent.start("powershell", args);

    if (!m_agent.waitForStarted()) {
        std::cout << "      Program start failed" << std::endl;
    }
    else {
        std::cout << "      Program started" << std::endl;
        if (!m_agent.waitForFinished()) {
            std::cout << "      Something else went wrong" << std::endl;
        }
        else {
            std::cout << "      Program finished" << std::endl;
            return true;
        }
    }
    return false;
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

void fileInteractions::copyFolderTo(QString folderPath, QString targetPath) {
    QDir folder(folderPath);
    QDir target(targetPath);
    if (folder.exists()) {

        foreach (QString subfolder, folder.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            if (!QDir(targetPath + QDir::separator() + subfolder).exists()) {
                target.mkpath(subfolder);
            }
            copyFolderTo(folderPath + QDir::separator() + subfolder, targetPath + QDir::separator() + subfolder);
        }

        foreach (QString filename, folder.entryList(QDir::Files)) {
            if (QDir(targetPath).exists(filename)) {
                removeFile(targetPath, filename);
            }
            QFile::copy(folderPath + QDir::separator() + filename, targetPath + QDir::separator() + filename);
        }
    }
}

void fileInteractions::copyFileTo(QString filePath, QString targetPath) {
    QFile source(filePath);
    QFileInfo target(targetPath);
    if (!source.exists()) {
        return;
    }
    if (!target.absoluteDir().exists()) {
        QDir("").mkpath(target.absolutePath());
    }
    source.copy(targetPath);
}

void fileInteractions::removeFolder(QString folderPath) {
    QDir folder(folderPath);
    if (folder.exists()) {
        folder.removeRecursively();
    }
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
