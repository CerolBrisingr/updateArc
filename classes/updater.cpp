#include "updater.h"
#include "fileinteractions.h"

UpdateTool::UpdateTool(Settings* settings)
    :_settings(settings)
{
    updateTargetPaths(_gw_path);
}

UpdateTool::~UpdateTool()
{
}

bool UpdateTool::verifyLocation() {
    bool existGw2_64 = false;
    bool existGw2_32 = false;
    QString gw_path = "..";

    for (int i = 0; i < 2; i++) {
        existGw2_64 = QDir(gw_path).exists("Gw2-64.exe");
        existGw2_32 = QDir(gw_path).exists("Gw2.exe");
        if (!(existGw2_32 || existGw2_64)){
            gw_path = "../..";
        } else {
            break;
        }
    }
    if (!(existGw2_32 || existGw2_64)){
        Log::write("Could not find gw2 executable. Updater seems to be at wrong location\n");
        return false;
    }

    updateTargetPaths(gw_path);
    bool existBin64  = QDir(_gw_path + "/bin64").exists();
    if (!(existBin64)) {
        Log::write("Missing target folder \"bin64\"\n");
        return false;
    }

    return true;
}

int UpdateTool::arcUninstaller() {

    // ArcDPS still installed? Provide option to install blocker
    if (verifyArcInstallation()) {
        Log::write("Removing ArcDPS. Will block the current version from being re-installed.\n");
        Log::write("Click again to remove this blocker.\n");

        QString sLocalHash = fileInteractions::calculateHashFromFile(_gw_path + "/bin64/d3d9.dll");
        Log::write("Blocking installation of version " + sLocalHash + "\n");
        _settings->setValue(_arc_blocker_key, sLocalHash);
    }

    fileInteractions::removeFile(_gw_path + "/bin64", "d3d9.dll");
    fileInteractions::removeFile(_gw_path + "", "d3d11.dll");
    fileInteractions::removeFile(_gw_path + "/bin64", "d3d9_arcdps_buildtemplates.dll");  // No longer available but purge remains

    return 0;
}

bool UpdateTool::verifyArcInstallation() {
    return QDir(_gw_path + "/bin64").exists("d3d9.dll");
}

int UpdateTool::updateArc() {
    Log::write("Starting <ArcDPS> update\n");
    int out = runArcUpdate();
    if (out == 0) {
        Log::write("    ArcDPS updater executed sucessfully\n");
    } else {
        Log::write("    ArcDPS update failed\n");
    }
    Log::write("Ended <ArcDPS> update\n");
    return out;
}

int UpdateTool::runArcUpdate() {

    QString sLocalHash;
    QString sRemoteHash = getRemoteHash();
    if (sRemoteHash.isEmpty()) {
        Log::write("    Could not read source hash file\n");
        return 1;
    }
    if (isBlockedArcVersion(sRemoteHash)) {
        Log::write("    Curently available version is blocked. Run with \"-remove\" to remove blocker\n");
        return 1;
    } else {
        // Remove previous blocker, if there is one
        _settings->removeKey(_arc_blocker_key);
    }
    QString targetpath = _gw_path + "/bin64";

    if (verifyArcInstallation()) {
        Log::write("    ArcDPS is seemingly already installed, looking for updates\n");
        sLocalHash = fileInteractions::calculateHashFromFile(_gw_path + "/bin64/d3d9.dll");
        if (sLocalHash.isEmpty()) {
            Log::write("    Could not calculate hash value for ArcDPS library.\n");
            return 1;
        }
        if (sRemoteHash.contains(sLocalHash)) {
            Log::write("    Match! Already running newest version!\n");
            return 0;
        } else {
            Log::write("    No Match! Downloading new version!\n");
            if (!downloadArc(targetpath)) {
                return 1;
            }
        }
    } else {
        Log::write("    ArcDPS not (fully) installed, fixing that\n");
        if (!downloadArc(targetpath)) {
            return 1;
        }
    }

    // Verify correct download
    sLocalHash = fileInteractions::calculateHashFromFile(_gw_path + "/bin64/d3d9.dll");
    if (sLocalHash.isEmpty()) {
        Log::write("    Could not calculate hash value for downloaded ArcDPS library.\n");
        return 1;
    }
    Log::write("     File: " + sLocalHash + "\n");
    if (!sRemoteHash.contains(sLocalHash)) {
        Log::write("    Something is wrong, hashes do not match!\n");
        if (arcUninstaller() == 1) {
            Log::write("    Removing files somehow went wrong, too! Is Gw2 running?\n");
        }
        return 1;
    } else {
        Log::write("    Hashes match, update successful!\n");
        Log::write("    Moving dx11 copy in place.\n");
        fileInteractions::copyFileTo(_gw_path + "/bin64/d3d9.dll", _gw_path + "/d3d11.dll");
        return 0;
    }
}

int UpdateTool::updateTaco()
{
    Log::write("Starting <TacO> update\n");
    QString tacoLink;
    QString tempTaco = "TacoNew";
    int16_t onlineVersion = inquireCurrentTacoVersion(tacoLink);
    if (canUpdateTaco(onlineVersion)) {
        QString sevenZipPath;

        // Removing possible remains from previous update
        fileInteractions::removeFolder(tempTaco);

        Log::write("    Starting download\n");
        if (0 != downloader::singleDownload(tacoLink, "", "tacoArchive.zip")) {
            Log::write("    Download of new version failed\n");
            return 1;
        }
        Log::write("    Extracting archive\n");
        if (!fileInteractions::unzipArchive("tacoArchive.zip", tempTaco)) {
            Log::write("    archive extraction failed\n");
            return 1;
        }
        if (!QDir(tempTaco).exists("GW2TacO.exe")) {
            Log::write("    archive extraction did not produce expected results. Keeping downloads.\n");
            return 1;
        }

        Log::write("    Moving files to target location\n");
        fileInteractions::copyFolderTo(tempTaco, _taco_path);
        fileInteractions::removeFolder(tempTaco);
        fileInteractions::removeFile("", "tacoArchive.zip");

        Log::write("    Registering newly installed version\n");
        _settings->setValue(_taco_install_key, QVariant(onlineVersion).toString());
    } else {
        Log::write("    Online version is already registered, no update needed!\n");
    }
    Log::write("Ended <TacO> update\n");
    return 0;
}

int UpdateTool::updateTekkit()
{
    Log::write("Starting <Tekkit> update\n");
    QString tekkitLink;
    QString filename = "tw_ALL_IN_ONE.taco";
    QVersionNumber onlineVersion = inquireCurrentTekkitVersion(tekkitLink);
    if (canUpdateTekkit(onlineVersion)) {
        Log::write("    Starting download of new version " + onlineVersion.toString() + "\n");
        if (0 != downloader::singleDownload(tekkitLink, "", filename)) {
            Log::write("    Download failed\n");
            return 1;
        }

        Log::write("    Moving file in place\n");
        fileInteractions::removeFile(_tekkit_path, filename);

        fileInteractions::copyFileTo(filename, _tekkit_path + QDir::separator() + filename);
        fileInteractions::removeFile("", filename);

        Log::write("    Registering newly installed version\n");
        _settings->setValue(_tekkit_install_key, onlineVersion.toString());
    } else {
        Log::write("    Online version is already registered, no update needed!\n");
    }
    Log::write("Ended <Tekkit> update\n");
    return 0;
}

bool UpdateTool::startGW2(QStringList arguments)
{
    QProcess gw2;
    gw2.setWorkingDirectory(_gw_path);
    if (arguments.empty()) {
        gw2.setArguments(loadGW2Arguments());
    } else
    {
        gw2.setArguments(arguments);
    }
    if (QDir(_gw_path).exists("Gw2-64.exe")) {
        gw2.setProgram(_gw_path + "/Gw2-64.exe");
    } else {
        gw2.setProgram(_gw_path + "/Gw2.exe");
    }
    if(gw2.startDetached()) {
        Log::write("-- started GuildWars2\n");
        return true;
    } else {
        Log::write("-- start of GuildWars2 failed\n");
        return false;
    }
}

QStringList UpdateTool::loadGW2Arguments() {

    QString argument_chain = _settings->getValue("starters/gw2_arguments", "");
    QStringList arguments;
    QStringList split_by_spaces = argument_chain.split(" ", QString::SplitBehavior::SkipEmptyParts);
    for (auto argument: split_by_spaces) {
        arguments.append(argument.trimmed());
    }
    return arguments;
}

bool UpdateTool::startTacO()
{
    QProcess taco;
    taco.setWorkingDirectory(_taco_path);
    taco.setProgram(_taco_path + "/GW2TacO.exe");
    if (taco.startDetached()) {
        Log::write("-- started TacO\n");
        return true;
    } else {
        Log::write("-- start of TacO failed\n");
        return false;
    }
}

void UpdateTool::updateTargetPaths(QString gw_path) {
    _gw_path = gw_path;
    _taco_path = _gw_path + "/addons/TacO";
    auto tekkit_user_path = _settings->getValueWrite(_tekkit_path_key);
    if ((tekkit_user_path.length() == 0) || !QDir(tekkit_user_path).exists()) {
        _tekkit_path = _taco_path + "/POIs";
        return;
    }
    Log::write("-- Found custom path for tekkit: " + tekkit_user_path+ "\n");
    _tekkit_path = tekkit_user_path;
}

QString UpdateTool::getRemoteHash() {

    // Read md5 hash of online version
    QString output;
    if (0 != downloader::singleTextRequest(output, "https://www.deltaconnected.com/arcdps/x64/d3d9.dll.md5sum")){
        Log::write("Download failed\n");
        return "";
    }
    return output;
}

bool UpdateTool::isBlockedArcVersion(QString sRemoteHash) {
    if (!_settings->hasKey(_arc_blocker_key)) {
        return false;
    }

    QString blockedHash = _settings->getValue(_arc_blocker_key);
    Log::write("    Blocked md5:  " + blockedHash + "\n");
    Log::write("    Received md5: " + sRemoteHash + "\n");
    if (sRemoteHash.contains(blockedHash)) {
        return true;
    } else {
        return false;
    }
}

bool UpdateTool::downloadArc(QString pathname) {
    // Only one download needed since build templates are gone
    if (0 != downloader::singleDownload("https://www.deltaconnected.com/arcdps/x64/d3d9.dll", pathname)) {
        Log::write("Download failed\n");
        return false;
    }
    return true;
}

int16_t UpdateTool::inquireCurrentTacoVersion(QString &tacoLink) {
    QString tacoBody;
    downloader::singleTextRequest(tacoBody, "http://www.gw2taco.com");
    QRegularExpression re("https://github\\.com/BoyC/GW2TacO/releases/download/\\d+\\.\\d+r/GW2TacO_(?<version>\\d+)r\\.zip");
    QRegularExpressionMatchIterator matches = re.globalMatch(tacoBody);
    int16_t latestVersion = 0;
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        if (match.captured("version").toInt() > latestVersion) {
            latestVersion = static_cast<int16_t>(match.captured(1).toInt());
            tacoLink = match.captured(0);
        }
    }
    return latestVersion;
}

bool UpdateTool::canUpdateTaco(int16_t &onlineVersion) {
    int16_t currentVersion = static_cast<int16_t>(_settings->getValue(_taco_install_key, "0").toInt());
    Log::write("    Taco version available:   " + QString(onlineVersion) + "\n");
    Log::write("    Taco version I installed: " + QString(currentVersion) + "\n");
    return onlineVersion > currentVersion;
}

QVersionNumber UpdateTool::inquireCurrentTekkitVersion(QString &tekkitLink) {
    QString tekkitBody;
    downloader::singleTextRequest(tekkitBody, "http://tekkitsworkshop.net/index.php/gw2-taco/download");

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
    QVersionNumber currentVersion = QVersionNumber::fromString(_settings->getValue(_tekkit_install_key, "0.0.0"));
    Log::write("    Tekkit version available:   " + onlineVersion.toString() + "\n");
    Log::write("    Tekkit version I installed: " + currentVersion.toString() + "\n");
    return QVersionNumber::compare(onlineVersion, currentVersion) > 0;
}
