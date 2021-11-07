#include "githupdater.h"

namespace Updater {

GitHupdater::GitHupdater(QString &gw_path, QPushButton *install_button, QToolButton *remove_button,
                         QCheckBox *checkbox, Updater::Config::GithupdateConfig cfg)
    :BaseUpdater(gw_path, install_button, remove_button, checkbox, cfg._github_project)
    , _github_user(cfg._github_user)
    , _github_project(cfg._github_project)
    , _install_path(gw_path + "/addons/" + cfg._github_project)
    , _version_key("version_installed/" + cfg._github_project)
    , _tag_prefix(cfg._tag_prefix)
    , _tag_suffix(cfg._tag_suffix)
    , _version_digits(cfg._version_digits)
    , _install_datatype(cfg._install_datatype)
    , _verifiction_file(cfg._verifiction_file)
{

}

GitHupdater::~GitHupdater()
{}

int GitHupdater::update()
{
    int err = 0;
    Log::write("Running updater for \"" + _github_project + "\"\n");
    QJsonArray releases = fetchReleases(err);
    if (err != 0) {
        Log::write("Failed to fetch release of " + _github_project + "\n");
        return err;
    }
    QJsonObject release = extractLatestFullRelease(releases);
    if (release.empty()) {
        Log::write("Could not locate any full release for " + _github_project + "\n");
    }
    Version online_version = extractVersion(release);
    Version local_version = readLocalVersion();
    if (compareVersions(local_version, online_version) != 0) {
        return 1;
    }
    installUpdate(release, online_version, err);

    if (err == 0) {
        Log::write("Update of \"" + _github_project + "\" completed\n");
    } else {
        Log::write("Update of \"" + _github_project + "\" aborted\n");
    }
    return err;
}

int GitHupdater::remove()
{
    fileInteractions::removeFolder(_install_path);
    _settings.removeKey(_version_key);
    Log::write("Removed install folder for <" + _github_project + ">\n");
    return 0;
}

QJsonArray GitHupdater::fetchReleases(int& err)
{
    QString answer;
    if (0 != downloader::singleTextRequest(answer, "https://api.github.com/repos/" + _github_user + "/" + _github_project + "/releases")) {
        Log::write("Failed to request releases from " + _github_project + " repository\n");
        err = 1;
        return QJsonArray();
    }

    QJsonDocument releases = SimpleJson::toJsonDoc(answer);
    if (releases.isNull() || !releases.isArray()) {
        err = 1;
        return QJsonArray();
    }
    return releases.array();
}

QJsonObject GitHupdater::extractLatestFullRelease(QJsonArray& releases)
{
    for (auto it = releases.begin(); it != releases.end(); ++it) {
        const bool is_prerelease = SimpleJson::getBool(it->toObject(), "prerelease", true);
        const bool is_draft = SimpleJson::getBool(it->toObject(), "draft", true);
        if (is_prerelease || is_draft) {
            continue;
        }
        return it->toObject();
    }
    return QJsonObject();
}

Version GitHupdater::extractVersion(QJsonObject& release)
{
    const auto tag = SimpleJson::getString(release, "tag_name");
    Version tag_version = Version(tag, _version_digits, _tag_prefix, _tag_suffix);
    Log::write("Extracted Version \"" + tag_version.toString() + "\" from tag \"" + tag + "\"\n");
    return tag_version;
}

Version GitHupdater::readLocalVersion()
{
    QString version_text = _settings.getValue(_version_key, "no_local_install");
    return Version(version_text, _version_digits);
}

int GitHupdater::compareVersions(Version& local_version, Version& online_version)
{
    if (local_version == online_version) {
        Log::write("Local and online version match: " + local_version.toString() + "\n");
        Log::write("Not updating " + _github_project + "\n");
        return 1;
    }
    Log::write("    Local version:  " + local_version.toString() + "\n");
    Log::write("    Online version: " + online_version.toString() + "\n");
    if (local_version > online_version) {
        Log::write("Looks like the online version did step back.\n");
        Log::write("Consider removing the current version. Will not update!\n");
        return 1;
    }
    Log::write("New version available for " + _github_project + ", starting Update!\n");
    return 0;
}

void GitHupdater::installUpdate(QJsonObject& release, Version& online_version, int& err)
{
    QJsonObject asset = getInstallAsset(release, err);
    if (err != 0) {
        return;
    }
    installAsset(asset, online_version, err);
    return;

}

QJsonObject GitHupdater::getInstallAsset(QJsonObject& release, int& err)
{
    QJsonArray assets = SimpleJson::getArray(release, "assets");
    Log::write("Scanning assets\n");
    for (auto it = assets.begin(); it != assets.end(); ++it) {
        QJsonObject asset = it->toObject();
        QString asset_type = SimpleJson::getString(asset, "content_type", "not_found");
        if (asset_type.compare("application/x-zip-compressed") != 0) {
            Log::write("    Found asset of type \"" + asset_type + "\". Skipping.\n");
            continue;
        }
        QString filename = SimpleJson::getString(asset, "name", "");
        Log::write("    Asset points to file \"" + filename + "\".\n");
        if (!filename.contains(_install_datatype)) {
            Log::write("    Asset file name does not contain \"" + _install_datatype + "\". Skipping.\n");
            continue;
        }
        return asset;
    }
    err = 1;
    return QJsonObject();
}

void GitHupdater::installAsset(QJsonObject& asset, Version& online_version, int& err) {

    QString temp_pathname = _github_project + "_tmp";
    QString temp_filename = _github_project + "_tmp" + _install_datatype;
    QString install_link = SimpleJson::getString(asset, "browser_download_url", "");
    if (install_link == "") {
        err = 1;
        return;
    }

    // Removing possible remains from previous update
    fileInteractions::removeFolder(temp_pathname);

    Log::write("    Starting download\n");
    if (0 != downloader::singleDownload(install_link, "", temp_filename)) {
        Log::write("    Download of new version failed\n");
        err = 1;
        return;
    }
    Log::write("    Extracting archive\n");
    if (!fileInteractions::unzipArchive(temp_filename, temp_pathname)) {
        Log::write("    archive extraction failed\n");
        err = 1;
        return;
    }
    if (!QDir(temp_pathname).exists(_verifiction_file)) {
        Log::write("    archive extraction did not produce expected results. Keeping downloads.\n");
        err = 1;
        return;
    }

    Log::write("    Moving files to target location\n");
    fileInteractions::copyFolderTo(temp_pathname, _install_path);
    fileInteractions::removeFolder(temp_pathname);
    fileInteractions::removeFile("", temp_filename);

    Log::write("    Registering newly installed version\n");
    _settings.setValue(_version_key, online_version.toString());
}

} // namespace Updater
