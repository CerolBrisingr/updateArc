#ifndef FILEINTERACTIONS_H
#define FILEINTERACTIONS_H

#include <QCoreApplication>
#include <QCryptographicHash>

#include "classes/downloader.h"
#include "Windows.h"

class fileInteractions
{
public:
    fileInteractions() {}
    ~fileInteractions() {}

    static bool find7zip(QString &path);
    static bool extractWith7zip(QString archivePath, QString outputName);
    static bool executeExternalWaiting(QString executablePath, QString working_directory = "");
    static QString calculateHashFromFile(QString sFile);
    static QString readFullFileString(QString filename);
    static QString readFirstFileLine(QString filename);
    static int writeFileString(QString filename, QString filecontent);
    static int removeFile(QString pathstring, QString filename);
    static void copyFolderTo(QString folderPath, QString targetPath);
    static void copyFileTo(QString filePath, QString targetPath);
    static void removeFolder(QString folderPath);
    static QString getVersionString(QString fName);

private:
    static bool searchPathAt(QString key, QString &path);

};


#endif // FILEINTERACTIONS_H
