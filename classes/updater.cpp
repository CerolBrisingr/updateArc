#include "updater.h"

UpdateTool::UpdateTool()
{
}

UpdateTool::~UpdateTool()
{
}

int UpdateTool::arcUninstaller() {

    bool blockerIsFresh = false;

    // ArcDPS still installed? Provide option to install blocker
    if (verifyArcInstallation()) {
        std::cout << "Do you want to block the current version from being re-installed? Enter (y)!" << std::endl;
        std::string userInput;
        std::getline(std::cin, userInput);
        if (userInput.compare("y") == 0) {
            QString sLocalHash = fileInteractions::calculateHashFromFile("../bin64/d3d9.dll");
            std::cout << "Blocking installation of version " << sLocalHash.toStdString() << std::endl;
            setSetting(_arc_blocker_key, sLocalHash);
            blockerIsFresh = true;
        } else {
            removeSetting(_arc_blocker_key);
        }
    }

    // Is an old blocker in place? Provide option to remove blocker
    if (hasSetting(_arc_blocker_key) && !blockerIsFresh) {
        std::cout << "Blocker detected. Do you want to remove it? Enter (y)!" << std::endl;
        std::string userInput;
        std::getline(std::cin, userInput);
        if (userInput.compare("y") == 0) {
            std::cout << "Removing blocker!" << std::endl;
            removeSetting(_arc_blocker_key);
        }
    }

    fileInteractions::removeFile("../bin64", "d3d9.dll");
    fileInteractions::removeFile("../bin64", "d3d9_arcdps_buildtemplates.dll");  // No longer available but purge remains

    return 0;
}

bool UpdateTool::verifyArcInstallation() {
    return QDir("../bin64").exists("d3d9.dll");
}

int UpdateTool::updateArc() {

    QString sLocalHash;
    QString sRemoteHash = getRemoteHash();
    if (sRemoteHash.isEmpty()) {
        std::cout << "Could not read source hash file" << std::endl;
        return 1;
    }
    if (isBlockedArcVersion(sRemoteHash)) {
        std::cout << "Curently available version is blocked. Run with \"-remove\" to remove blocker" << std::endl;
        return 1;
    } else {
        // Remove previous blocker, if there is one
        removeSetting(_arc_blocker_key);
    }
    QString targetpath = "../bin64";

    if (verifyArcInstallation()) {

        std::cout << "ArcDPS is seemingly already installed, looking for updates" << std::endl;

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

int UpdateTool::updateTaco()
{
    std::cout << "Starting <TacO> update" << std::endl;
    int _error = 0;
    QString tacoLink;
    int16_t onlineVersion = inquireCurrentTacoVersion(tacoLink);
    std::cout << "    Update link: " << tacoLink.toStdString() << std::endl;
    if (canUpdateTaco(onlineVersion)) {
        QString sevenZipPath;
        std::cout << "    Starting download" << std::endl;
        if (!downloader::singleDownload(tacoLink, "", "tacoArchive.zip")) {
            std::cout << "    Download of new version failed" << std::endl;
            return 1;
        }
        std::cout << "    Verify current 7-Zip version" << std::endl;
        if (update7zip() != 0) {
            std::cout << "    Cannot locate 7-Zip" << std::endl;
            return 1;
        }
        std::cout << "    Extracting archive" << std::endl;
        if (!fileInteractions::extractWith7zip("tacoArchive.zip", "TacoNew")) {
            std::cout << "    archive extraction failed" << std::endl;
            return 1;
        }
        setSetting(_taco_install_key, QVariant(onlineVersion).toString());
    } else {
        std::cout << "    Online version is already registered, no update needed!" << std::endl;
    }
    std::cout << "Ended <TacO> update" << std::endl;
    return _error;
}

int UpdateTool::updateTekkit()
{
    std::cout << "Starting <Tekkit> update" << std::endl;
    int _error = 0;
    QString tekkitLink;
    QVersionNumber onlineVersion = inquireCurrentTekkitVersion(tekkitLink);
    std::cout << "    Update link: " << tekkitLink.toStdString() << std::endl;
    if (canUpdateTekkit(onlineVersion)) {
        std::cout << "    I'll pretend I can already install Tekkit version " << onlineVersion.toString().toStdString() << std::endl;
        //downloader::singleDownload(tekkitLink, "", "tw_ALL_IN_ONE.taco");
        setSetting(_tekkit_install_key, onlineVersion.toString());
    } else {
        std::cout << "    Online version is already registered, no update needed!" << std::endl;
    }
    std::cout << "Ended <Tekkit> update" << std::endl;
    return _error;
}

int UpdateTool::update7zip() {

    std::cout << "    Searching 7Zip registry entry" << std::endl;
    QString path;
    QString sevenZipLink;
    QString sevenZipPath;
    QVersionNumber installed7zipVersion;

    if (fileInteractions::find7zip(path)) {
        std::cout << "    Install path: \"" << path.toStdString() << "\"" << std::endl;
        installed7zipVersion = QVersionNumber::fromString(fileInteractions::getVersionString(path));
    } else {
        installed7zipVersion = QVersionNumber(QVector<int>(4, 0));
    }
    std::cout << "    Version: " << installed7zipVersion.toString().toStdString() << std::endl;

    QVersionNumber current7zipVersion = inquireCurrent7zipVersion(sevenZipLink);
    std::cout << "    Online version: " << current7zipVersion.toString().toStdString() << std::endl;
    std::cout << "    DL Link         " << sevenZipLink.toStdString() << std::endl;

    std::cout << "!!!!! Faking old 7zip" << std::endl;
    installed7zipVersion = QVersionNumber(QVector<int>(4, 0));
    if (QVersionNumber::compare(current7zipVersion, installed7zipVersion) > 0) {
        if (!(downloader::singleDownload(sevenZipLink, "", "sevenZipInstaller.exe") == 0)) {
            return 1;
        }
        std::cout << "    Looks like we have a new version" << std::endl;
        QDir dir;
        QString executablePath = dir.absolutePath() + "/sevenZipInstaller.exe";
        if(!fileInteractions::executeExternalWaiting(executablePath)) {
            return 1;
        }
        fileInteractions::removeFile("", "sevenZipInstaller.exe");
    }

    if (!fileInteractions::find7zip(sevenZipPath)) {
        return 1;
    }
    return 0;
}

bool UpdateTool::hasSetting(QString key)
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    return setting.contains(key);
}

void UpdateTool::setSetting(QString key, QString value)
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    setting.setValue(key, value);
}

QString UpdateTool::getSetting(QString key, QString default_value)
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    return setting.value(key, default_value).toString();
}

void UpdateTool::removeSetting(QString key)
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    setting.remove(key);
    return;
}

QString UpdateTool::getRemoteHash() {

    // Read md5 hash of online version
    QString output = "https://www.deltaconnected.com/arcdps/x64/d3d9.dll.md5sum";
    if (0 != downloader::singleTextRequest(output)){
        std::cout << "Download failed" << std::endl;
        return "";
    }
    return output;
}

bool UpdateTool::isBlockedArcVersion(QString sRemoteHash) {
    if (!hasSetting(_arc_blocker_key)) {
        std::cout << "Did not find blocker!" << std::endl;
        return false;
    }

    QString blockedHash = getSetting(_arc_blocker_key);
    std::cout << "Blocked:  " << blockedHash.toStdString() << std::endl;
    std::cout << "Received: " << sRemoteHash.toStdString() << std::endl;
    if (sRemoteHash.contains(blockedHash)) {
        return true;
    } else {
        return false;
    }
}

bool UpdateTool::downloadArc(QString pathname) {
    // Only one download needed since build templates are gone
    if (0 != downloader::singleDownload("https://www.deltaconnected.com/arcdps/x64/d3d9.dll", pathname)) {
        std::cout << "Download failed" << std::endl;
        return false;
    }
    return true;
}

int16_t UpdateTool::inquireCurrentTacoVersion(QString &tacoLink) {
    QString tacoBody = "http://www.gw2taco.com";
    downloader::singleTextRequest(tacoBody);

    QRegularExpression re("https://www\\.dropbox\\.com/s/\\w+/GW2TacO_(\\d+)r\\.zip\\?dl=1");
    QRegularExpressionMatchIterator matches = re.globalMatch(tacoBody);
    int16_t latestVersion = 0;
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        if (match.captured(1).toInt() > latestVersion) {
            latestVersion = static_cast<int16_t>(match.captured(1).toInt());
            tacoLink = match.captured(0);
        }
    }
    return latestVersion;
}

bool UpdateTool::canUpdateTaco(int16_t &onlineVersion) {
    int16_t currentVersion = static_cast<int16_t>(getSetting(_taco_install_key, "0").toInt());
    std::cout << "    Taco version available: " << onlineVersion << std::endl;
    std::cout << "    Taco version installed: " << currentVersion << std::endl;
    return onlineVersion > currentVersion;
}

QVersionNumber UpdateTool::inquireCurrentTekkitVersion(QString &tekkitLink) {
    QString tekkitBody = "http://tekkitsworkshop.net/index.php/gw2-taco/download";
    downloader::singleTextRequest(tekkitBody);

    QRegularExpression re("href=\"(/index\\.php/gw2-taco/download/send/\\d-taco-marker-packs/\\d+-all-in-one)\">--- ALL-IN-ONE --- (\\d+)\\.(\\d+)\\.(\\d+) - \\d+\\.\\d+\\.\\d+</a>");
    QRegularExpressionMatchIterator matches = re.globalMatch(tekkitBody);
    QVersionNumber latestVersion = QVersionNumber(0, 0, 0);
    int a,b,c;
    QVersionNumber readingVersion;
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        a = match.captured(2).toInt();
        b = match.captured(3).toInt();
        c = match.captured(4).toInt();
        readingVersion = QVersionNumber(a, b, c);
        if (QVersionNumber::compare(readingVersion, latestVersion) > 0) {
            latestVersion = readingVersion;
            tekkitLink = "http://tekkitsworkshop.net" + match.captured(1);
        }
    }
    return latestVersion;
}

bool UpdateTool::canUpdateTekkit(QVersionNumber &onlineVersion)
{
    QVersionNumber currentVersion = QVersionNumber::fromString(getSetting(_tekkit_install_key, "0.0.0"));
    std::cout << "    Tekkit version available: " << onlineVersion.toString().toStdString() << std::endl;
    std::cout << "    Tekkit version installed: " << currentVersion.toString().toStdString() << std::endl;
    return QVersionNumber::compare(onlineVersion, currentVersion) > 0;
}

QVersionNumber UpdateTool::inquireCurrent7zipVersion(QString &sevenZipLink) {
    QString sevenZipBody = "https://sourceforge.net/projects/sevenzip/files/7-Zip/";
    downloader::singleTextRequest(sevenZipBody);

    QRegularExpression re("href=\"(/projects/sevenzip/files/7-Zip/(\\d+)\\.(\\d+)/)\"");
    QRegularExpressionMatchIterator matches = re.globalMatch(sevenZipBody);
    QVersionNumber latestVersion = QVersionNumber(QVector<int>(4, 0));
    QVector<int> versionVec(4, 0);
    QVersionNumber readingVersion;
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        versionVec[0] = match.captured(2).toInt();
        versionVec[1] = match.captured(3).toInt();
        readingVersion = QVersionNumber(versionVec);
        if (QVersionNumber::compare(readingVersion, latestVersion) > 0) {
            latestVersion = readingVersion;
            QString fileVersion = match.captured(2) + match.captured(3);
            sevenZipLink = "https://7-zip.org/a/7z" + fileVersion + "-x64.exe";
            //sevenZipLink = "https://sourceforge.net" + match.captured(1) + "7z" + fileVersion + "-x64.exe/download";
        }
    }
    return latestVersion;
}


