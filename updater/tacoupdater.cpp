#include "tacoupdater.h"

namespace Updater {

TacoUpdater::TacoUpdater(QString &gw_path, QPushButton* install_button, QToolButton* remove_button,
                         QCheckBox* checkbox, QString settings_key)
    :BaseUpdater(gw_path, install_button, remove_button, checkbox, settings_key)
    ,_taco_path(gw_path + "/addons/TacO")
{}

int TacoUpdater::update()
{
    Log::write("Starting <TacO> update\n");
    QString tacoLink;
    QString tempTaco = "TacoNew";
    int16_t onlineVersion = inquireCurrentTacoVersion(tacoLink);
    if (canUpdateTaco(onlineVersion)) {
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
        _settings.setValue(_taco_install_key, QVariant(onlineVersion).toString());
    } else {
        Log::write("    Online version is already registered, no update needed!\n");
    }
    Log::write("Ended <TacO> update\n");
    return 0;
}

int TacoUpdater::remove()
{
    Log::write("    TacO removal not implemented!\n");
    return -1;
}

int16_t TacoUpdater::inquireCurrentTacoVersion(QString &tacoLink) {
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

bool TacoUpdater::canUpdateTaco(int16_t &onlineVersion) {
    int16_t currentVersion = static_cast<int16_t>(_settings.getValue(_taco_install_key, "0").toInt());
    Log::write("    Taco version available:   " + QString(onlineVersion) + "\n");
    Log::write("    Taco version I installed: " + QString(currentVersion) + "\n");
    return onlineVersion > currentVersion;
}

} // namespace Updater
