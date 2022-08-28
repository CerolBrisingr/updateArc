#ifndef FILEINTERACTIONS_H
#define FILEINTERACTIONS_H

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QtCore>
#include <iostream>

namespace fileInteractions
{
    bool unzipArchive(QString archive, QString targetPath);
    bool executeExternalWaiting(QString executablePath, QString working_directory = "");
    QString calculateHashFromFile(QString sFile);
    QString readFullFileString(QString filename);
    QString readFirstFileLine(QString filename);
    int writeFileString(QString filename, QString filecontent);
    int removeFile(QString pathstring, QString filename);
    void copyFolderTo(QString folderPath, QString targetPath);
    void copyFileTo(QString filePath, QString targetPath);
    void removeFolder(QString folderPath);
    void createFolder(QString folderPath);
};


#endif // FILEINTERACTIONS_H
