#include <QCoreApplication>

#include <iostream>
#include "classes/downloader.h"
#include "classes/fileinteractions.h"
#include <chrono>
#include <thread>

int updater();
int arcUninstaller();

bool verifyUpdaterLocation();
bool verifyArcInstallation();
bool blockerIsInstalled();
bool isBlocked(QString sRemoteHash);
bool downloadArc(QString pathname);
QString getRemoteHash();

void evaluateInput(QCoreApplication &app, bool &doExit, bool &undoInstall);

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    bool doExit;
    bool undoInstall;
    int returnValue = 1;

    evaluateInput(app, doExit, undoInstall);

    std::cout << "Searching gw2 install" << std::endl;
    if (verifyUpdaterLocation()) {
        // Perform core functionality
        if (undoInstall) {
            std::cout << "Running removal" << std::endl;
            returnValue = arcUninstaller();
            std::cout << "Uninstall finished" << std::endl;
        } else {
            std::cout << "Running updater" << std::endl;
            returnValue = updater();
            std::cout << "Updater finished" << std::endl;
        }

        fileInteractions::removeFile("", "d3d9.dll.md5sum");
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
        std::cout << std::endl << "No \"-closeAfter\" requested. Close at own leisure." << std::endl;
        return app.exec();
    }
}

void evaluateInput(QCoreApplication &app, bool &doExit, bool &undoInstall){

    QStringList args = app.instance()->arguments();
    QString argument;

    doExit = false;
    undoInstall = false;

    // Read input argument, ignore arguments following the first one
    args.takeFirst(); // skip program name
    while (!args.isEmpty()) {
        argument = args.takeFirst();
        if (argument == "-remove")
            undoInstall = true;
        else if (argument == "-closeAfter") {
            doExit = true;
        } else {
            std::cout << "Invalid command \"" << argument.toStdString()
                      << "\", either use \"-closeAfter\" or \"-remove\"!" << std::endl;
        }
    }
}

int arcUninstaller() {

    bool blockerIsFresh = false;

    // ArcDPS still installed? Provide option to install blocker
    if (verifyArcInstallation()) {
        std::cout << "Do you want to block the current version from being re-installed? Enter (y)!" << std::endl;
        std::string userInput;
        std::getline(std::cin, userInput);
        if (userInput.compare("y") == 0) {
            QString sLocalHash = fileInteractions::calculateHashFromFile("../bin64/d3d9.dll");
            std::cout << "Blocking installation of version " << sLocalHash.toStdString() << std::endl;
            fileInteractions::writeFileString("blocker.md5sum", sLocalHash + "  d3d9.dll");
            blockerIsFresh = true;
        } else {
            fileInteractions::removeFile("", "blocker.md5sum");
        }
    }

    // Is an old blocker in place? Provide option to remove blocker
    if (blockerIsInstalled() && !blockerIsFresh) {
        std::cout << "Blocker detected. Do you want to remove it? Enter (y)!" << std::endl;
        std::string userInput;
        std::getline(std::cin, userInput);
        if (userInput.compare("y") == 0) {
            std::cout << "Removing blocker!" << std::endl;
            fileInteractions::removeFile("", "blocker.md5sum");
        }
    }

    fileInteractions::removeFile("../bin64", "d3d9.dll.md5sum");  // Only there if user put it there
    fileInteractions::removeFile("../bin64", "d3d9.dll");
    fileInteractions::removeFile("../bin64", "d3d9_arcdps_buildtemplates.dll");  // No longer available but purge remains

    return 0;
}

int updater() {

    QString sLocalHash;
    QString sRemoteHash = getRemoteHash();
    if (sRemoteHash.isEmpty()) {
        std::cout << "Could not read source hash file" << std::endl;
        return 1;
    }
    if (isBlocked(sRemoteHash)) {
        std::cout << "Curently available version is blocked. Run with \"-remove\" to remove blocker" << std::endl;
        return 1;
    } else {
        // Remove previous blocker, if there is one
        fileInteractions::removeFile("", "blocker.md5sum");
    }
    QString targetpath = "../bin64";

    if (verifyArcInstallation()) {

        std::cout << "ArcDPS is already installed, looking for updates" << std::endl;

        sLocalHash = fileInteractions::calculateHashFromFile("../bin64/d3d9.dll");
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
            if (!downloadArc(targetpath)) {
                return 1;
            }
        }

    } else {
        std::cout << "ArcDPS not (fully) installed, fixing that" << std::endl;
        if (!downloadArc(targetpath)) {
            return 1;
        }
    }

    // Verify correct download
    sLocalHash = fileInteractions::calculateHashFromFile("../bin64/d3d9.dll");
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
    return QDir("../bin64").exists("d3d9.dll");
}

bool blockerIsInstalled() {
    return QDir("").exists("blocker.md5sum");
}

bool isBlocked(QString sRemoteHash) {
    if (!blockerIsInstalled()) {
        return false;
    }

    QString blockedHash = fileInteractions::readFirstFileLine("blocker.md5sum");
    if (sRemoteHash.compare(blockedHash) == 0) {
        return true;
    } else {
        return false;
    }
}

bool downloadArc(QString pathname) {
    // Only one download needed since build templates are gone
    if (0 != downloader::singleDownload("https://www.deltaconnected.com/arcdps/x64/d3d9.dll", pathname)) {
        std::cout << "Download failed" << std::endl;
        return false;
    }
    return true;
}

QString getRemoteHash() {

    // Read md5 hash of online version
    if (0 != downloader::singleDownload("https://www.deltaconnected.com/arcdps/x64/d3d9.dll.md5sum")){
        std::cout << "Download failed" << std::endl;
        return "";
    }
    return fileInteractions::readFirstFileLine("d3d9.dll.md5sum");
}
