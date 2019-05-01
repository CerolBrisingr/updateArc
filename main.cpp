#include <QCoreApplication>
#include <iostream>
#include <QCryptographicHash>
#include <QFile>
#include <QTimer>
#include <QString>
#include <QTextStream>
#include "downloader.h"

#include <chrono>
#include <thread>

int updater();
int arcUninstaller();
int selfUninstaller();

bool verifyUpdaterLocation();
bool verifyArcInstallation();
bool downloadArc(downloader& netSource);
QString getHashFromFile(QString sFile);
QString getRemoteHash(downloader& netSource);


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList args = app.instance()->arguments();

    QString command;

    // Read input argument, ignore arguments following the first one
    args.takeFirst(); // skip program name
    if (args.isEmpty()) {
        command = "-update";
    } else {
        command = args.takeFirst();
    }

    int returnValue = 1;
    if (command == "-update") {
        returnValue = updater();
        std::cout << "Updater finished" << std::endl;
    } else if (command == "-remove") {
        returnValue = arcUninstaller();
        std::cout << "Uninstall finished" << std::endl;
    } else {
        std::cout << "Invalid command, either use \"-update\" or \"-remove\"!" << std::endl;
        returnValue = 1;
    }

    if (QDir("").exists("d3d9.dll.md5sum")) {
        QFile md5sum("d3d9.dll.md5sum");
        md5sum.remove();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    return returnValue;
}


int arcUninstaller() {

    if (QDir("").exists("d3d9.dll.md5sum")) {
        QFile md5sum("d3d9.dll.md5sum");
        md5sum.remove();
    }

    // In case the user put that file to the others
    if (QDir("bin64").exists("d3d9.dll.md5sum")) {
        QFile md5sum("bin64/d3d9.dll.md5sum");
        md5sum.remove();
    }

    if (QDir("bin64").exists("d3d9.dll")) {
        QFile md5sum("bin64/d3d9.dll");
        md5sum.remove();
    }

    if (QDir("bin64").exists("d3d9_arcdps_buildtemplates.dll")) {
        QFile md5sum("bin64/d3d9_arcdps_buildtemplates.dll");
        md5sum.remove();
    }

    return 0;
}


int updater() {

    downloader netSource;
    netSource.setTargetPath("");

    if (!verifyUpdaterLocation()) {
        return 1;
    }

    QString sLocalHash;
    QString sRemoteHash = getRemoteHash(netSource);
    if (sRemoteHash.isEmpty()) {
        return 1;
    }
    netSource.setTargetPath("bin64");

    if (verifyArcInstallation()) {

        std::cout << "ArcDPS is already installed, looking for updates" << std::endl;

        sLocalHash = getHashFromFile("bin64/d3d9.dll");
        if (sLocalHash.isEmpty()) {
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
    sLocalHash = getHashFromFile("bin64/d3d9.dll");
    if (sLocalHash.isEmpty()) {
        return 1;
    }
    std::cout << "File: " << sLocalHash.toStdString() << std::endl;
    if (!sRemoteHash.contains(sLocalHash)) {
        std::cout << "Something is wrong, hashes do not match!" << std::endl;
        return 1;
    } else {
        std::cout << "Hashes match, update successful!" << std::endl;
        return 0;
    }
}


bool verifyUpdaterLocation() {

    bool existGw2_64 = QDir("").exists("Gw2-64.exe");
    bool existGw2_32 = QDir("").exists("Gw2.exe");
    if (!(existGw2_32 ||  existGw2_64)){
        std::cout << "Could not find gw2 executable. Updater seems to be at wrong location" << std::endl;
        return false;
    }

    bool existBin64  = QDir("bin64").exists();
    if (!(existBin64)) {
        std::cout << "Missing target folder \"bin64\"" << std::endl;
        return false;
    }

    return true;
}

bool verifyArcInstallation() {
    bool existd3d9   = QDir("bin64").exists("d3d9.dll");
    bool existBuilds = QDir("bin64").exists("d3d9_arcdps_buildtemplates.dll");

    return existd3d9 && existBuilds;
}

bool downloadArc(downloader& netSource) {

    QVector<QString> arcFiles;
    arcFiles.append("https://www.deltaconnected.com/arcdps/x64/d3d9.dll");
    arcFiles.append("https://www.deltaconnected.com/arcdps/x64/buildtemplates/d3d9_arcdps_buildtemplates.dll");
    if (0 != netSource.fetch(arcFiles)) {
        std::cout << "Download failed" << std::endl;
        return false;
    }
    return true;
}


QString getHashFromFile(QString sFile) {

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
    QFile referenceFile("d3d9.dll.md5sum");
    if (!referenceFile.open(QFile::ReadOnly | QFile::Text)) {
        std::cout << "Could not open reference file" << std::endl;
        return "";
    }
    QTextStream line(&referenceFile);
    return line.readLine();
}
