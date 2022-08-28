#include "fileinteractions.h"

namespace fileInteractions {

bool unzipArchive(QString archive, QString targetPath) {
    QProcess m_agent;
    QFileInfo source(archive);
    QFileInfo target(targetPath);
    //m_agent.setWorkingDirectory(working_directory);
    QStringList args = QStringList();
    args = QStringList({"-Command", QString("Expand-Archive -LiteralPath \"%1\" -DestinationPath \"%2\"").arg(source.absoluteFilePath(), target.absoluteFilePath())});
    m_agent.start("powershell", args);

    if (!m_agent.waitForStarted()) {
        std::cout << "Could not start powershell to unpack archive" << std::endl;
    }
    else {
        if (!m_agent.waitForFinished()) {
            std::cout << "Unpacking did not finish sucessfully" << std::endl;
        }
        else {
            return true;
        }
    }
    return false;
}

bool executeExternalWaiting(QString executablePath, QString working_directory) {
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


QString calculateHashFromFile(QString sFile) {
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

QString readFullFileString(QString filename)
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

QString readFirstFileLine(QString filename) {
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

int writeFileString(QString filename, QString filecontent) {
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

int removeFile(QString pathstring, QString filename) {
    if (QDir(pathstring).exists(filename)) {
        if (!pathstring.isEmpty()) {
            pathstring = pathstring + "/";
        }
        QFile toRemove(pathstring + filename);
        toRemove.remove();
    }

    return 0;
}

void copyFolderTo(QString folderPath, QString targetPath) {
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

void copyFileTo(QString filePath, QString targetPath) {
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

void removeFolder(QString folderPath) {
    QDir folder(folderPath);
    if (folder.exists()) {
        folder.removeRecursively();
    }
}

void createFolder(QString folderPath)
{
    QDir folder(folderPath);
    if (!folder.exists()) {
        QDir("").mkpath(folder.absolutePath());
    }
}

}
