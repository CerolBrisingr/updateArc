#include <QCoreApplication>
#include <QCryptographicHash>
#include <QFile>
#include <QString>
#include <QTextStream>

#include <iostream>
#include "classes/downloader.h"
#include <chrono>
#include <thread>

int updater();
int arcUninstaller();

bool verifyUpdaterLocation();
bool verifyArcInstallation();
bool downloadArc(downloader& netSource);
QString calculateHashFromFile(QString sFile);
QString getRemoteHash(downloader& netSource);
QString readFileString(QString filename);
int removeFile(QString pathstring, QString filename);


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList args = app.instance()->arguments();

    QString argument;
    bool doExit  = true;
    bool undoInstall = false;
    int returnValue = 1;

    if (verifyUpdaterLocation()) {

        // Read input argument, ignore arguments following the first one
        args.takeFirst(); // skip program name
        while (!args.isEmpty()) {
            argument = args.takeFirst();
            if (argument == "-remove")
                undoInstall = true;
            else if (argument == "-keepOpen") {
                doExit = false;
            } else {
                std::cout << "Invalid command \"" << argument.toStdString()
                          << "\", either use \"-keepOpen\" or \"-remove\"!" << std::endl;
            }
        }

        // Perform core functionality
        if (undoInstall) {
            returnValue = arcUninstaller();
            std::cout << "Uninstall finished" << std::endl;
        } else {
            returnValue = updater();
            std::cout << "Updater finished" << std::endl;
        }

        if (QDir("").exists("d3d9.dll.md5sum")) {
            QFile md5sum("d3d9.dll.md5sum");
            md5sum.remove();
        }
    }

    // Exit after timer or stay open at user command
    if (doExit) {
        if (returnValue == 0) {
            std::cout << "Update completed successfully. Window closing in 10 seconds!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10));
        } else {
            std::cout << "An error occured. Please close manually!" << std::endl;
        }
        return returnValue;
    } else {
        std::cout << std::endl << "\"-keepOpen\" requested. Close at own leisure." << std::endl;
        return app.exec();
    }
}


int arcUninstaller() {

    removeFile("", "d3d9.dll.md5sum");
    removeFile("../bin64", "d3d9.dll.md5sum");  // Only there if user put it there
    removeFile("../bin64", "d3d9.dll");
    removeFile("../bin64", "d3d9_arcdps_buildtemplates.dll");  // No longer available but purge remains

    return 0;
}


int removeFile(QString pathstring, QString filename) {
    if (QDir(pathstring).exists(filename)) {
        QFile toRemove(pathstring + "/" + filename);
        toRemove.remove();
    }

    return 0;
}


int updater() {

    downloader netSource;
    netSource.setTargetPath("");

    QString sLocalHash;
    QString sRemoteHash = getRemoteHash(netSource);
    if (sRemoteHash.isEmpty()) {
        return 1;
    }
    netSource.setTargetPath("../bin64");

    if (verifyArcInstallation()) {

        std::cout << "ArcDPS is already installed, looking for updates" << std::endl;

        sLocalHash = calculateHashFromFile("../bin64/d3d9.dll");
        if (sLocalHash.isEmpty()) {
            std::cout << "Could not calculate hash value for ArcDPS library." << std::endl;
            return 1;
        }

        std::cout << "File: " << sLocalHash.toStdString() << std::endl;
        std::cout << "Ref:  " << sRemoteHash.toStdString() << std::endl;

        if (sRemoteHash.contains(sLocalHash)) {
            std::cout << "Match! Already running newest version!" << std::endl;
            return 0;
        } else {
            std::cout << "No Match! Downloading new version!" << std::endl;
            if (!downloadArc(netSource)) {
                return 1;
            }
        }

    } else {
        std::cout << "ArcDPS not (fully) installed, fixing that" << std::endl;
        if (!downloadArc(netSource)) {
            return 1;
        }
    }

    // Verify correct download
    sLocalHash = calculateHashFromFile("../bin64/d3d9.dll");
    if (sLocalHash.isEmpty()) {
        std::cout << "Could not calculate hash value for downloaded ArcDPS library." << std::endl;
        return 1;
    }
    std::cout << "File: " << sLocalHash.toStdString() << std::endl;
    if (!sRemoteHash.contains(sLocalHash)) {
        std::cout << "Something is wrong, hashes do not match!" << std::endl;
        if (arcUninstaller() == 1) {
            std::cout << "Removing files somehow went wrong, too! Is Gw2 running?" << std::endl;
        }
        return 1;
    } else {
        std::cout << "Hashes match, update successful!" << std::endl;
        return 0;
    }
}


bool verifyUpdaterLocation() {

    bool existGw2_64 = QDir("../").exists("Gw2-64.exe");
    bool existGw2_32 = QDir("../").exists("Gw2.exe");
    if (!(existGw2_32 ||  existGw2_64)){
        std::cout << "Could not find gw2 executable. Updater seems to be at wrong location" << std::endl;
        return false;
    }

    bool existBin64  = QDir("../bin64").exists();
    if (!(existBin64)) {
        std::cout << "Missing target folder \"bin64\"" << std::endl;
        return false;
    }

    return true;
}

bool verifyArcInstallation() {
    bool existd3d9   = QDir("../bin64").exists("d3d9.dll");

    return existd3d9;
}

bool downloadArc(downloader& netSource) {

    QVector<QString> arcFiles;
    // Append all files you want to download
    arcFiles.append("https://www.deltaconnected.com/arcdps/x64/d3d9.dll");
    if (0 != netSource.fetch(arcFiles)) {
        std::cout << "Download failed" << std::endl;
        return false;
    }
    return true;
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


QString getRemoteHash(downloader& netSource) {

    // Read hash from online version
    if (0 != netSource.fetch("https://www.deltaconnected.com/arcdps/x64/d3d9.dll.md5sum")){
        std::cout << "Download failed" << std::endl;
        return "";
    }
    return readFileString("d3d9.dll.md5sum");
}

QString readFileString(QString filename) {
    QFile referenceFile(filename);
    if (!referenceFile.open(QFile::ReadOnly | QFile::Text)) {
        std::cout << "Could not open reference file" << std::endl;
        return "";
    }
    QTextStream line(&referenceFile);
    return line.readLine();
}
