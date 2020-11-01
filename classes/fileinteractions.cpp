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
