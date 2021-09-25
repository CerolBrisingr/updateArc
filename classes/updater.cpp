#include "updater.h"
#include "fileinteractions.h"

UpdateTool::UpdateTool(Settings* settings)
    :_settings(settings)
{
    updateTargetPaths(_gw_path);
    _stream.setString(&_streamline);
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
        writeline("Could not find gw2 executable. Updater seems to be at wrong location");
        return false;
    }

    updateTargetPaths(gw_path);
    bool existBin64  = QDir(_gw_path + "/bin64").exists();
    if (!(existBin64)) {
        writeline("Missing target folder \"bin64\"");
        return false;
    }

    return true;
}

int UpdateTool::arcUninstaller() {

    // ArcDPS still installed? Provide option to install blocker
    if (verifyArcInstallation()) {
        writeline("Removing ArcDPS. Will block the current version from being re-installed.");
        writeline("Click again to remove this blocker.");

        QString sLocalHash = fileInteractions::calculateHashFromFile(_gw_path + "/bin64/d3d9.dll");
        _stream << "Blocking installation of version " << sLocalHash; writeline();
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
    writeline("Starting <ArcDPS> update");
    int out = runArcUpdate();
    if (out == 0) {
        writeline("    ArcDPS updater executed sucessfully");
    } else {
        writeline("    ArcDPS update failed");
    }
    writeline("Ended <ArcDPS> update");
    return out;
}

int UpdateTool::runArcUpdate() {

    QString sLocalHash;
    QString sRemoteHash = getRemoteHash();
    if (sRemoteHash.isEmpty()) {
        writeline("    Could not read source hash file");
        return 1;
    }
    if (isBlockedArcVersion(sRemoteHash)) {
        writeline("    Curently available version is blocked. Run with \"-remove\" to remove blocker");
        return 1;
    } else {
        // Remove previous blocker, if there is one
        _settings->removeKey(_arc_blocker_key);
    }
    QString targetpath = _gw_path + "/bin64";

    if (verifyArcInstallation()) {
        writeline("    ArcDPS is seemingly already installed, looking for updates");
        sLocalHash = fileInteractions::calculateHashFromFile(_gw_path + "/bin64/d3d9.dll");
        if (sLocalHash.isEmpty()) {
            writeline("    Could not calculate hash value for ArcDPS library.");
            return 1;
        }
        if (sRemoteHash.contains(sLocalHash)) {
            writeline("    Match! Already running newest version!");
            return 0;
        } else {
            writeline("    No Match! Downloading new version!");
            if (!downloadArc(targetpath)) {
                return 1;
            }
        }
    } else {
        writeline("    ArcDPS not (fully) installed, fixing that");
        if (!downloadArc(targetpath)) {
            return 1;
        }
    }

    // Verify correct download
    sLocalHash = fileInteractions::calculateHashFromFile(_gw_path + "/bin64/d3d9.dll");
    if (sLocalHash.isEmpty()) {
        writeline("    Could not calculate hash value for downloaded ArcDPS library.");
        return 1;
    }
    _stream << "     File: " << sLocalHash; writeline();
    if (!sRemoteHash.contains(sLocalHash)) {
        writeline("    Something is wrong, hashes do not match!");
        if (arcUninstaller() == 1) {
            writeline("    Removing files somehow went wrong, too! Is Gw2 running?");
        }
        return 1;
    } else {
        writeline("    Hashes match, update successful!");
        writeline("    Moving dx11 copy in place.");
        fileInteractions::copyFileTo(_gw_path + "/bin64/d3d9.dll", _gw_path + "/d3d11.dll");
        return 0;
    }
}

int UpdateTool::updateTaco()
{
    writeline("Starting <TacO> update");
    QString tacoLink;
    QString tempTaco = "TacoNew";
    int16_t onlineVersion = inquireCurrentTacoVersion(tacoLink);
    if (canUpdateTaco(onlineVersion)) {
        QString sevenZipPath;

        // Removing possible remains from previous update
        fileInteractions::removeFolder(tempTaco);

        writeline("    Starting download");
        if (0 != downloader::singleDownload(tacoLink, "", "tacoArchive.zip")) {
            writeline("    Download of new version failed");
            return 1;
        }
        writeline("    Extracting archive");
        if (!fileInteractions::unzipArchive("tacoArchive.zip", tempTaco)) {
            writeline("    archive extraction failed");
            return 1;
        }
        if (!QDir(tempTaco).exists("GW2TacO.exe")) {
            writeline("    archive extraction did not produce expected results. Keeping downloads.");
            return 1;
        }

        writeline("    Moving files to target location");
        fileInteractions::copyFolderTo(tempTaco, _taco_path);
        fileInteractions::removeFolder(tempTaco);
        fileInteractions::removeFile("", "tacoArchive.zip");

        writeline("    Registering newly installed version");
        _settings->setValue(_taco_install_key, QVariant(onlineVersion).toString());
    } else {
        writeline("    Online version is already registered, no update needed!");
    }
    writeline("Ended <TacO> update");
    return 0;
}

int UpdateTool::updateTekkit()
{
    writeline("Starting <Tekkit> update");
    QString tekkitLink;
    QString filename = "tw_ALL_IN_ONE.taco";
    QVersionNumber onlineVersion = inquireCurrentTekkitVersion(tekkitLink);
    if (canUpdateTekkit(onlineVersion)) {
        _stream << "    Starting download of new version " << onlineVersion.toString(); writeline();
        if (0 != downloader::singleDownload(tekkitLink, "", filename)) {
            writeline("    Download failed");
            return 1;
        }

        writeline("    Moving file in place");
        fileInteractions::removeFile(_tekkit_path, filename);

        fileInteractions::copyFileTo(filename, _tekkit_path + QDir::separator() + filename);
        fileInteractions::removeFile("", filename);

        writeline("    Registering newly installed version");
        _settings->setValue(_tekkit_install_key, onlineVersion.toString());
    } else {
        writeline("    Online version is already registered, no update needed!");
    }
    writeline("Ended <Tekkit> update");
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
        writeline("-- started GuildWars2");
        return true;
    } else {
        writeline("-- start of GuildWars2 failed");
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

void UpdateTool::write(QString text)
{
    emit(write_log(text));
}

void UpdateTool::writeline(QString text)
{
    emit(write_log(text + "\n"));
}

void UpdateTool::write()
{
    emit(write_log(_stream.readAll()));
}

void UpdateTool::writeline()
{
    emit(write_log(_stream.readAll() + "\n"));
}

bool UpdateTool::startTacO()
{
    QProcess taco;
    taco.setWorkingDirectory(_taco_path);
    taco.setProgram(_taco_path + "/GW2TacO.exe");
    if (taco.startDetached()) {
        writeline("-- started TacO");
        return true;
    } else {
        writeline("-- start of TacO failed");
        return false;
    }
}

void UpdateTool::updateTargetPaths(QString gw_path) {
    _gw_path = gw_path;
    _taco_path = _gw_path + "/addons/TacO";
    _tekkit_path = _taco_path + "/POIs";
}

QString UpdateTool::getRemoteHash() {

    // Read md5 hash of online version
    QString output = "https://www.deltaconnected.com/arcdps/x64/d3d9.dll.md5sum";
    if (0 != downloader::singleTextRequest(output)){
        writeline("Download failed");
        return "";
    }
    return output;
}

bool UpdateTool::isBlockedArcVersion(QString sRemoteHash) {
    if (!_settings->hasKey(_arc_blocker_key)) {
        return false;
    }

    QString blockedHash = _settings->getValue(_arc_blocker_key);
    _stream << "    Blocked md5:  " << blockedHash; writeline();
    _stream << "    Received md5: " << sRemoteHash; writeline();
    if (sRemoteHash.contains(blockedHash)) {
        return true;
    } else {
        return false;
    }
}

bool UpdateTool::downloadArc(QString pathname) {
    // Only one download needed since build templates are gone
    if (0 != downloader::singleDownload("https://www.deltaconnected.com/arcdps/x64/d3d9.dll", pathname)) {
        writeline("Download failed");
        return false;
    }
    return true;
}

int16_t UpdateTool::inquireCurrentTacoVersion(QString &tacoLink) {
    QString tacoBody = "http://www.gw2taco.com";
    downloader::singleTextRequest(tacoBody);
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
    _stream << "    Taco version available:   " << onlineVersion; writeline();
    _stream << "    Taco version I installed: " << currentVersion; writeline();
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
    QVersionNumber currentVersion = QVersionNumber::fromString(_settings->getValue(_tekkit_install_key, "0.0.0"));
    _stream << "    Tekkit version available:   " << onlineVersion.toString(); writeline();
    _stream << "    Tekkit version I installed: " << currentVersion.toString(); writeline();
    return QVersionNumber::compare(onlineVersion, currentVersion) > 0;
}
