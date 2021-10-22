#include "arcupdater.h"

namespace Updater {

ArcUpdater::ArcUpdater(QString& gw_path, QPushButton* install_button, QToolButton* remove_button,
                       QCheckBox* checkbox)
    :BaseUpdater(gw_path, install_button, remove_button, checkbox)
{
}

void ArcUpdater::removeSlot()
{
    _install_button->setEnabled(false);
    _remove_button->setEnabled(false);
    if (_settings.hasKey("updaters/block_arcdps")) {
        _settings.removeKey("updaters/block_arcdps");
        Log::write("Removed blocker for ArcDPS update.\n");
    } else {
        remove();
    }
    _remove_button->setEnabled(true);
    _install_button->setEnabled(true);
}

int ArcUpdater::remove()
{
    // ArcDPS still installed? Provide option to install blocker
    if (verifyArcInstallation()) {
        Log::write("Removing ArcDPS. Will block the current version from being re-installed.\n");
        Log::write("Click again to remove this blocker.\n");

        QString sLocalHash = fileInteractions::calculateHashFromFile(_gw_path + "/bin64/d3d9.dll");
        Log::write("Blocking installation of version " + sLocalHash + "\n");
        _settings.setValue(_arc_blocker_key, sLocalHash);
    }

    fileInteractions::removeFile(_gw_path + "/bin64", "d3d9.dll");
    fileInteractions::removeFile(_gw_path + "", "d3d11.dll");
    fileInteractions::removeFile(_gw_path + "/bin64", "d3d9_arcdps_buildtemplates.dll");  // No longer available but purge remains

    return 0;
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

int ArcUpdater::runArcUpdate() {

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
        _settings.removeKey(_arc_blocker_key);
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
        if (this->remove() == 1) {
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

QString ArcUpdater::getRemoteHash()
{
    // Read md5 hash of online version
    QString output;
    if (0 != downloader::singleTextRequest(output, "https://www.deltaconnected.com/arcdps/x64/d3d9.dll.md5sum")){
        Log::write("Download failed\n");
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
    return QDir(_gw_path + "/bin64").exists("d3d9.dll");
}

bool ArcUpdater::downloadArc(QString pathname)
{
    if (0 != downloader::singleDownload("https://www.deltaconnected.com/arcdps/x64/d3d9.dll", pathname)) {
        Log::write("Download failed\n");
        return false;
    }
    return true;
}

} // namespace Downloader
