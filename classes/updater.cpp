#include "updater.h"

UpdateTool::UpdateTool()
{
}

UpdateTool::~UpdateTool()
{
}

bool UpdateTool::verifyLocation() {

    bool existGw2_64 = QDir(_gw_path).exists("Gw2-64.exe");
    bool existGw2_32 = QDir(_gw_path).exists("Gw2.exe");
    if (!(existGw2_32 ||  existGw2_64)){
        std::cout << "Could not find gw2 executable. Updater seems to be at wrong location" << std::endl;
        return false;
    }

    bool existBin64  = QDir(_gw_path + "/bin64").exists();
    if (!(existBin64)) {
        std::cout << "Missing target folder \"bin64\"" << std::endl;
        return false;
    }

    return true;
}

int UpdateTool::arcUninstaller() {

    bool blockerIsFresh = false;

    // ArcDPS still installed? Provide option to install blocker
    if (verifyArcInstallation()) {
        std::cout << "Do you want to block the current version from being re-installed? Enter (y)!" << std::endl;
        std::string userInput;
        std::getline(std::cin, userInput);
        if (userInput.compare("y") == 0) {
            QString sLocalHash = fileInteractions::calculateHashFromFile(_gw_path + "/bin64/d3d9.dll");
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

    fileInteractions::removeFile(_gw_path + "/bin64", "d3d9.dll");
    fileInteractions::removeFile(_gw_path + "/bin64", "d3d9_arcdps_buildtemplates.dll");  // No longer available but purge remains

    return 0;
}

bool UpdateTool::verifyArcInstallation() {
    return QDir(_gw_path + "/bin64").exists("d3d9.dll");
}

int UpdateTool::updateArc() {
    std::cout << "Starting <ArcDPS> update" << std::endl;
    int out = runArcUpdate();
    if (out == 0) {
        std::cout << "    ArcDPS update sucessful" << std::endl;
    } else {
        std::cout << "    ArcDPS update failed" << std::endl;
    }
    std::cout << "Ended <ArcDPS> update" << std::endl;
    return out;
}

int UpdateTool::runArcUpdate() {

    QString sLocalHash;
    QString sRemoteHash = getRemoteHash();
    if (sRemoteHash.isEmpty()) {
        std::cout << "    Could not read source hash file" << std::endl;
        return 1;
    }
    if (isBlockedArcVersion(sRemoteHash)) {
        std::cout << "    Curently available version is blocked. Run with \"-remove\" to remove blocker" << std::endl;
        return 1;
    } else {
        // Remove previous blocker, if there is one
        removeSetting(_arc_blocker_key);
    }
    QString targetpath = _gw_path + "/bin64";

    if (verifyArcInstallation()) {
        std::cout << "    ArcDPS is seemingly already installed, looking for updates" << std::endl;
        sLocalHash = fileInteractions::calculateHashFromFile(_gw_path + "/bin64/d3d9.dll");
        if (sLocalHash.isEmpty()) {
            std::cout << "    Could not calculate hash value for ArcDPS library." << std::endl;
            return 1;
        }
        if (sRemoteHash.contains(sLocalHash)) {
            std::cout << "    Match! Already running newest version!" << std::endl;
            return 0;
        } else {
            std::cout << "    No Match! Downloading new version!" << std::endl;
            if (!downloadArc(targetpath)) {
                return 1;
            }
        }
    } else {
        std::cout << "    ArcDPS not (fully) installed, fixing that" << std::endl;
        if (!downloadArc(targetpath)) {
            return 1;
        }
    }

    // Verify correct download
    sLocalHash = fileInteractions::calculateHashFromFile(_gw_path + "/bin64/d3d9.dll");
    if (sLocalHash.isEmpty()) {
        std::cout << "    Could not calculate hash value for downloaded ArcDPS library." << std::endl;
        return 1;
    }
    std::cout << "     File: " << sLocalHash.toStdString() << std::endl;
    if (!sRemoteHash.contains(sLocalHash)) {
        std::cout << "    Something is wrong, hashes do not match!" << std::endl;
        if (arcUninstaller() == 1) {
            std::cout << "    Removing files somehow went wrong, too! Is Gw2 running?" << std::endl;
        }
        return 1;
    } else {
        std::cout << "    Hashes match, update successful!" << std::endl;
        return 0;
    }
}

int UpdateTool::updateTaco()
{
    std::cout << "Starting <TacO> update" << std::endl;
    QString tacoLink;
    QString tempTaco = "TacoNew";
    int16_t onlineVersion = inquireCurrentTacoVersion(tacoLink);
    if (canUpdateTaco(onlineVersion)) {
        QString sevenZipPath;

        // Removing possible remains from previous update
        fileInteractions::removeFolder(tempTaco);

        std::cout << "    Starting download" << std::endl;
        if (0 != downloader::singleDownload(tacoLink, "", "tacoArchive.zip")) {
            std::cout << "    Download of new version failed" << std::endl;
            return 1;
        }
        // Verify current 7-Zip version
        if (update7zip() != 0) {
            std::cout << "    Cannot locate 7-Zip" << std::endl;
            return 1;
        }
        std::cout << "    Extracting archive" << std::endl;
        if (!fileInteractions::extractWith7zip("tacoArchive.zip", tempTaco)) {
            std::cout << "    archive extraction failed" << std::endl;
            return 1;
        }
        fileInteractions::copyFolderTo(tempTaco, _taco_path);
        fileInteractions::removeFolder(tempTaco);
        fileInteractions::removeFile("", "tacoArchive.zip");
        setSetting(_taco_install_key, QVariant(onlineVersion).toString());
    } else {
        std::cout << "    Online version is already registered, no update needed!" << std::endl;
    }
    std::cout << "Ended <TacO> update" << std::endl;
    return 0;
}

int UpdateTool::updateTekkit()
{
    std::cout << "Starting <Tekkit> update" << std::endl;
    QString tekkitLink;
    QString filename = "tw_ALL_IN_ONE.taco";
    QVersionNumber onlineVersion = inquireCurrentTekkitVersion(tekkitLink);
    if (canUpdateTekkit(onlineVersion)) {
        std::cout << "    Starting download of new version " << onlineVersion.toString().toStdString() << std::endl;
        if (0 != downloader::singleDownload(tekkitLink, "", filename)) {
            std::cout << "    Download failed" << std::endl;
            return 1;
        }

        std::cout << "    Moving file in place" << std::endl;
        fileInteractions::removeFile(_tekkit_path, filename);

        fileInteractions::copyFileTo(filename, _tekkit_path + QDir::separator() + filename);
        fileInteractions::removeFile("", filename);

        std::cout << "    Registering newly installed version" << std::endl;
        setSetting(_tekkit_install_key, onlineVersion.toString());
    } else {
        std::cout << "    Online version is already registered, no update needed!" << std::endl;
    }
    std::cout << "Ended <Tekkit> update" << std::endl;
    return 0;
}

int UpdateTool::update7zip() {

    std::cout << "    Searching 7Zip installation" << std::endl;
    QString path;
    QString sevenZipLink;
    QString sevenZipPath;
    QVersionNumber installed7zipVersion;

    if (fileInteractions::find7zip(path)) {
        std::cout << "    Found 7-Zip at \"" << path.toStdString() << "\"" << std::endl;
        installed7zipVersion = QVersionNumber::fromString(fileInteractions::getVersionString(path));
    } else {
        installed7zipVersion = QVersionNumber(QVector<int>(4, 0));
    }
    QVersionNumber current7zipVersion = inquireCurrent7zipVersion(sevenZipLink);

    // Update/Install 7-Zip if necessary
    if (QVersionNumber::compare(current7zipVersion, installed7zipVersion) > 0) {
        std::cout << "    Did not find current version of 7-Zip, downloading version " << current7zipVersion.toString().toStdString() << std::endl;
        if (0 != downloader::singleDownload(sevenZipLink, "", "sevenZipInstaller.exe")) {
            std::cout << "    Download of 7-Zip failed" << std::endl;
            return 1;
        }

        // Install 7-Zip, remove installer
        QDir dir;
        QString executablePath = dir.absolutePath() + "/sevenZipInstaller.exe";
        if(!fileInteractions::executeExternalWaiting(executablePath)) {
            std::cout << "  7-Zip update not possible, execute \"sevenZipInstaller.exe\" to fix that" << std::endl;
            std::cout << "  TacO update failed, cannot unpack archive" << std::endl;
            return 1;
        }
    }

    // I don't care if the user aborts the install process as long as any version is installed, to be honest.
    // At that point it's their choice. But there needs to be SOME version installed.
    if (!fileInteractions::find7zip(sevenZipPath)) {
        std::cout << "  7-Zip still not installed, execute \"sevenZipInstaller.exe\" to fix that" << std::endl;
        std::cout << "  TacO update failed, cannot unpack archive" << std::endl;
        return 1;
    }
    fileInteractions::removeFile("", "sevenZipInstaller.exe");
    return 0;
}

bool UpdateTool::startGW2()
{
    QString setting = getSetting2("Starter/GuildWars2", "no");
    if (setting == "yes") {
        QProcess gw2;
        gw2.setWorkingDirectory(_gw_path);
        gw2.setArguments(readGW2Arguments());
        if (QDir(_gw_path).exists("Gw2-64.exe")) {
            gw2.setProgram(_gw_path + "/Gw2-64.exe");
        } else {
            gw2.setProgram(_gw_path + "/Gw2.exe");
        }
        if(gw2.startDetached()) {
            std::cout << "-- started GuildWars2" << std::endl;
            return true;
        } else {
            std::cout << "-- start of GuildWars2 failed" << std::endl;
            return false;
        }
    } else {
        std::cout << "-- no start for GuildWars2 requested" << std::endl;
        return true;
    }
}

QStringList UpdateTool::readGW2Arguments() {
    QString argumentChain = getSetting2("Starter/GuildWars2Arguments", "");
    QStringList splitByQuotes = argumentChain.split("\"", QString::SplitBehavior::SkipEmptyParts);
    QStringList splitArguments;
    for (int i = 0; i < splitByQuotes.length(); i++) {
        if (i % 2 == 0) {
            splitArguments << splitByQuotes[i].split(" ", QString::SplitBehavior::SkipEmptyParts);
        } else {
            splitArguments << splitByQuotes[i];
        }
    }
    return splitArguments;
}

bool UpdateTool::startTacO()
{
    QString tacoSetting = getSetting2("Starter/TacO", "no");
    QString gw2Setting = getSetting2("Starter/GuildWars2", "no");
    int waitTacO = QVariant(getSetting2("Starter/TacODelay_s", "90")).toInt();
    if ((tacoSetting == "yes") && (gw2Setting == "yes")) {
        QProcess taco;
        taco.setWorkingDirectory(_taco_path);
        taco.setProgram(_taco_path + "/GW2TacO.exe");
        std::cout << "Waiting for " << waitTacO << "seconds before we start TacO" << std::endl;
        for (int i = 0; i < waitTacO; i += 10) {
            std::cout << "Timer: " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
        if (taco.startDetached()) {
            std::cout << "-- started TacO" << std::endl;
            return true;
        } else {
            std::cout << "-- start of TacO failed" << std::endl;
            return false;
        }
    } else {
        std::cout << "-- no start for TacO requested" << std::endl;
        return true;
    }
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

QString UpdateTool::getSetting2(QString key, QString default_value)
{
    if (!hasSetting(key)) {
        setSetting(key, default_value);
    }

    QSettings setting(_ini_path, QSettings::IniFormat);
    return setting.value(key, default_value).toString();
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
        std::cout << "    Did not find blocker!" << std::endl;
        return false;
    }

    QString blockedHash = getSetting(_arc_blocker_key);
    std::cout << "    Blocked:  " << blockedHash.toStdString() << std::endl;
    std::cout << "    Received: " << sRemoteHash.toStdString() << std::endl;
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


