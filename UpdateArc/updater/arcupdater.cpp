#include "arcupdater.h"

namespace Updater {

ArcUpdater::ArcUpdater(QString& gw_path, QPushButton* install_button, QToolButton* remove_button,
                       QCheckBox* checkbox, QString settings_key)
    :BaseUpdater(gw_path, install_button, remove_button, checkbox, settings_key)
{}

void ArcUpdater::removeSlot()
{
    _install_button->setEnabled(false);
    _remove_button->setEnabled(false);
    bool workingInstall = verifyArcInstallation();
    if (_settings.hasKey("updaters/block_arcdps") && !workingInstall) {
        _settings.removeKey("updaters/block_arcdps");
        Log::write("  Removed blocker for ArcDPS update.\n");
    } else {
        remove();
    }
    _remove_button->setEnabled(true);
    _install_button->setEnabled(true);
}

int ArcUpdater::remove()
{
    int err = 0;
    // ArcDPS still installed? Provide option to install blocker
    if (verifyArcInstallation()) {
        Log::write("  Removing ArcDPS. Will block the current version from being re-installed.\n");
        Log::write("  Click again to remove this blocker.\n");

        QString sLocalHash = FileInteractions::calculateHashFromFile(_gw_path + "/d3d11.dll");
        Log::write("  Blocking installation of version " + sLocalHash + "\n");
        _settings.setValue(_arc_blocker_key, sLocalHash);
    }

    fileRemoval(err);
    return 0;
}

bool ArcUpdater::fileRemoval(int& err) {
    FileInteractions::removeFile(_gw_path + "/bin64", "d3d9.dll");
    FileInteractions::removeFile(_gw_path + "", "d3d11.dll");
    FileInteractions::removeFile(_gw_path + "/bin64", "d3d9_arcdps_buildtemplates.dll");  // No longer available but purge remains

    if (verifyArcInstallation()) {
        Log::write("  ERROR: ArcDPS is still installed!\n");
        Log::write("         Make sure that the game is not running.\n");
        Log::write("         Make sure that the launcher is not running.\n");
        Log::write("         Then try again.\n");
        err = 1;
        return false;
    }
    return true;
}

int ArcUpdater::update()
{
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

int ArcUpdater::runArcUpdate()
{
    QString sLocalHash;
    int err = 0;
    QString sRemoteHash = getRemoteHash();

    testCanUpdate(err, sRemoteHash);
    if (err != 0) {
        return err;
    }

    if (!runningUpdate(err, sRemoteHash, sLocalHash)) {
        return err;
    }

    testUpdatedVersion(err, sRemoteHash, sLocalHash);
    return err;
}

QString ArcUpdater::getRemoteHash()
{
    // Read md5 hash of online version
    QString output;
    if (0 != Downloader::singleTextRequest(output, "https://www.deltaconnected.com/arcdps/x64/d3d11.dll.md5sum")){
        Log::write("  Download failed\n");
        return "";
    }
    return output;
}

bool ArcUpdater::isBlockedArcVersion(QString sRemoteHash)
{
    if (!_settings.hasKey(_arc_blocker_key)) {
        return false;
    }

    QString blockedHash = _settings.getValue(_arc_blocker_key);
    Log::write("    Blocked md5:  " + blockedHash + "\n");
    Log::write("    Received md5: " + sRemoteHash + "\n");
    if (sRemoteHash.contains(blockedHash)) {
        return true;
    } else {
        return false;
    }
}

bool ArcUpdater::verifyArcInstallation()
{
    return QDir(_gw_path).exists("d3d11.dll");
}

bool ArcUpdater::downloadArc(QString pathname)
{
    if (0 != Downloader::singleDownload("https://www.deltaconnected.com/arcdps/x64/d3d11.dll", pathname)) {
        Log::write("  Download failed\n");
        return false;
    }
    return true;
}

void ArcUpdater::testCanUpdate(int& err, QString sRemoteHash)
{
    if (sRemoteHash.isEmpty()) {
        Log::write("    Could not read source hash file\n");
        err = 1;
        return;
    }
    if (isBlockedArcVersion(sRemoteHash)) {
        Log::write("    Curently available version is blocked. Choose [-] again to remove the blocker!\n");
        err = 1;
        return;
    } else {
        // Remove previous blocker, if there is one
        _settings.removeKey(_arc_blocker_key);
    }
}

bool ArcUpdater::runningUpdate(int& err, QString sRemoteHash, QString sLocalHash)
{
    QString targetpath = _gw_path;
    bool installed = verifyArcInstallation();
    if (installed) {
        Log::write("    ArcDPS is seemingly already installed, looking for updates\n");
        sLocalHash = FileInteractions::calculateHashFromFile(_gw_path + "/d3d11.dll");
        if (sLocalHash.isEmpty()) {
            Log::write("    Could not calculate hash value for ArcDPS library.\n");
            err = 1;
            return false;
        }
        if (sRemoteHash.contains(sLocalHash)) {
            Log::write("    Match! Already running newest version!\n");
            return false;
        } else {
            Log::write("    No Match! Downloading new version!\n");
            if (!downloadArc(targetpath)) {
                err = 1;
                return false;
            }
        }
    } else {
        Log::write("    ArcDPS not (fully) installed, fixing that\n");
        if (!downloadArc(targetpath)) {
            err = 1;
            return false;
        }
    }
    return true;
}

void ArcUpdater::testUpdatedVersion(int& err, QString sRemoteHash, QString sLocalHash)
{
    // Verify correct download
    sLocalHash = FileInteractions::calculateHashFromFile(_gw_path + "/d3d11.dll");
    if (sLocalHash.isEmpty()) {
        Log::write("    Could not calculate hash value for downloaded ArcDPS library.\n");
        fileRemoval(err);
        err = 1;
        return;
    }
    Log::write("     File: " + sLocalHash + "\n");
    if (!sRemoteHash.contains(sLocalHash)) {
        Log::write("    Something is wrong, hashes do not match!\n");
        if (!fileRemoval(err)) {
            Log::write("    Ouch! Now you have a file that does not match the hash and I could not remove it either.\n");
        }
        err = 1;
        return;
    } else {
        Log::write("    Hashes match, update successful!\n");
        if (_settings.getValueWrite("customize/arcdps_dx9", "off") == "on") {
            Log::write("    Moving dx9 copy in place.\n");
            FileInteractions::copyFileTo(_gw_path + "/d3d11.dll", _gw_path + "/bin64/d3d9.dll");
        }
    }
}

} // namespace Updater
