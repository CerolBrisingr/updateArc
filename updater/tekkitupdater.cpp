#include "tekkitupdater.h"

namespace Updater {

TekkitUpdater::TekkitUpdater(QString &gw_path, QPushButton* install_button, QToolButton* remove_button,
                             QCheckBox* checkbox, QString settings_key)
    :BaseUpdater(gw_path, install_button, remove_button, checkbox, settings_key)
    ,_tekkit_path(getTekkitPath())
{}

int TekkitUpdater::update()
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
        _settings.setValue(_tekkit_install_key, onlineVersion.toString());
    } else {
        Log::write("    Online version is already registered, no update needed!\n");
    }
    Log::write("Ended <Tekkit> update\n");
    return 0;
}

int TekkitUpdater::remove()
{
    Log::write("    Tekkit removal not implemented!\n");
    return -1;
}

QVersionNumber TekkitUpdater::inquireCurrentTekkitVersion(QString &tekkitLink) {
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

QString TekkitUpdater::getTekkitPath()
{
    auto tekkit_user_path = _settings.getValueWrite(_tekkit_path_key);
    if ((tekkit_user_path.length() == 0) || !QDir(tekkit_user_path).exists()) {
        return _gw_path + "/addons/TacO/POIs";
    }
    Log::write("-- Found custom path for tekkit: " + tekkit_user_path+ "\n");
    return tekkit_user_path;
}

bool TekkitUpdater::canUpdateTekkit(QVersionNumber &onlineVersion)
{
    QVersionNumber currentVersion = QVersionNumber::fromString(_settings.getValue(_tekkit_install_key, "0.0.0"));
    Log::write("    Tekkit version available:   " + onlineVersion.toString() + "\n");
    Log::write("    Tekkit version I installed: " + currentVersion.toString() + "\n");
    return QVersionNumber::compare(onlineVersion, currentVersion) > 0;
}

} // namespace Updater
