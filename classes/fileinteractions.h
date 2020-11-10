#ifndef FILEINTERACTIONS_H
#define FILEINTERACTIONS_H

#include <QCoreApplication>
#include <QCryptographicHash>

#include "classes/downloader.h"
#include "Windows.h"

class fileInteractions
{
public:
    fileInteractions();
    ~fileInteractions() {}

    static QString calculateHashFromFile(QString sFile);
    static QString readFullFileString(QString filename);
    static QString readFirstFileLine(QString filename);
    static int writeFileString(QString filename, QString filecontent);
    static int removeFile(QString pathstring, QString filename);
    static QString getVersionString(QString fName);
};

#endif // FILEINTERACTIONS_H
