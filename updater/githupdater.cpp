#include "githupdater.h"

namespace Updater {

GitHupdater::GitHupdater(QString &gw_path, QPushButton *install_button, QToolButton *remove_button,
                         QCheckBox *checkbox, QString settings_key, QString github_user, QString github_project)
    :BaseUpdater(gw_path, install_button, remove_button, checkbox, settings_key)
    , _github_user(github_user)
    , _github_project(github_project)
    , _version_key("version_installed/" + settings_key)
{

}

GitHupdater::~GitHupdater()
{}

int GitHupdater::update()
{
    int err = 0;
    return err;
}

int GitHupdater::remove()
{
    return 0;
}

QJsonObject GitHupdater::fetchReleases(int& err)
{
    QString answer;
    if (0 != downloader::singleTextRequest(answer, "https://api.github.com/repos/" + _github_user + "/" + _github_project + "/releases")) {
        Log::write("Failed to request releases from " + _github_project + " repository\n");
        err = 1;
        return SimpleJson::toJson("{}");
    }

    return SimpleJson::toJson(answer);
}

} // namespace Updater
