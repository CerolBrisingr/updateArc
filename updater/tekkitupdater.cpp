#include "tekkitupdater.h"

namespace Updater {
using namespace Tekkit;

TekkitUpdater::TekkitUpdater(QString &gw_path, QPushButton* install_button, QToolButton* remove_button,
                             QCheckBox* checkbox, QString settings_key)
    :BaseUpdater(gw_path, install_button, remove_button, checkbox, settings_key)
    ,_taco_info(getTacoMarkerPath(), "version_installed/GW2TacO")
    ,_blish_info(getBlishMarkerPath(), "version_installed/Blish-HUD")
{}

int TekkitUpdater::update()
{
    Log::write("Starting <Tekkit> update\n");
    int n_targets = prepareTargets();
    if (n_targets == 0) {
        Log::write("Aborted <Tekkit> update\n");
        return 1;
    }
    QString tekkit_link;
    Version online_version = inquireCurrentTekkitVersion(tekkit_link);
    if (canUpdateTekkit(online_version)) {
        Log::write("    Starting download of new version " + online_version.toString() + "\n");
        if (0 != downloader::singleDownload(tekkit_link, "", _filename)) {
            Log::write("    Download failed\n");
            return 1;
        }

        Log::write("    Moving file in place(s)\n");
        updateTarget(_taco_info);
        updateTarget(_blish_info);

        fileInteractions::removeFile("", _filename);

        Log::write("    Registering newly installed version\n");
        _settings.setValue(_tekkit_install_key, online_version.toString());
    } else {
        Log::write("    Online version is already registered, no update needed!\n");
    }
    Log::write("Ended <Tekkit> update\n");
    return 0;
}

void TekkitUpdater::updateTarget(const TkTarget& target)
{
    if (isTargetSetUp(target) && (target._install_path != "")) {
        fileInteractions::removeFile(target._install_path, _filename);
        fileInteractions::copyFileTo(_filename, target._install_path + QDir::separator() + _filename);
    }
}

void TekkitUpdater::removeTarget(const TkTarget& target)
{
    fileInteractions::removeFile(target._install_path, _filename);
}

int TekkitUpdater::remove()
{
    removeTarget(_blish_info);
    removeTarget(_taco_info);
    _settings.removeKey(_tekkit_install_key);
    Log::write("Tekkit markers removed!\n");
    return 0;
}

int TekkitUpdater::prepareTargets()
{
    int n_targets = 0;
    n_targets += cleanOrCount(_taco_info);
    n_targets += cleanOrCount(_blish_info);

    if (n_targets == 0) {
        Log::write("    No targets for Tekkit markers are available.\n");
        remove();
    }
    return n_targets;
}

int TekkitUpdater::cleanOrCount(const Tekkit::TkTarget &target)
{
    if (!isTargetSetUp(target)) {
        removeTarget(target);
        return 0;
    }
    return 1;
}

Version TekkitUpdater::inquireCurrentTekkitVersion(QString &tekkit_link) {
    QString tekkit_body;
    downloader::singleTextRequest(tekkit_body, "http://tekkitsworkshop.net/index.php/gw2-taco/download");

    QRegularExpression re("href=\"(/index\\.php/gw2-taco/download/send/\\d-taco-marker-packs/\\d+-all-in-one)\">--- ALL-IN-ONE --- (\\d+\\.\\d+\\.\\d+) - \\d+\\.\\d+\\.\\d+</a>");
    QRegularExpressionMatchIterator matches = re.globalMatch(tekkit_body);
    Version latest_version(std::vector<int>(_version_digits, 0));
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        Version reading_version(match.captured(2), _version_digits);
        if (reading_version > latest_version) {
            latest_version = reading_version;
            tekkit_link = "http://tekkitsworkshop.net" + match.captured(1);
        }
    }
    return latest_version;
}

bool TekkitUpdater::isTargetSetUp(const TkTarget& target)
{
    return _settings.hasKey(target._target_version_key);
}

QString TekkitUpdater::getTacoMarkerPath()
{
    return _gw_path + "/addons/TacO/POIs";
}

QString TekkitUpdater::getBlishMarkerPath()
{
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::StandardLocation::DocumentsLocation);
    if (paths.empty()) {
        return "";
    }
    return paths[0] + "/Guild Wars 2/addons/blishhud/markers";
}

bool TekkitUpdater::canUpdateTekkit(Version &online_version)
{
    Version current_version(_settings.getValue(_tekkit_install_key, "tekkit_not_installed"), _version_digits);
    Log::write("    Tekkit version available:   " + online_version.toString() + "\n");
    Log::write("    Tekkit version I installed: " + current_version.toString() + "\n");
    return online_version > current_version;
}

} // namespace Updater
