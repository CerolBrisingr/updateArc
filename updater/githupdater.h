#ifndef GITHUPDATER_H
#define GITHUPDATER_H

#include "baseupdater.h"
#include <QObject>
#include <classes/simple_json.h>
#include <version_recognition/version.h>

namespace Updater {

class GitHupdater : public BaseUpdater
{
public:
    GitHupdater(QString& gw_path, QPushButton* install_button, QToolButton* remove_button,
                QCheckBox* checkbox, QString settings_key, QString github_user, QString github_project);
    ~GitHupdater() override;

    int update() override;
    int remove() override;

private:
    const QString _github_user;
    const QString _github_project;
    const QString _install_path;
    const QString _version_key;
    const QString _tag_prefix = "";
    const QString _tag_suffix = "r";
    const size_t _version_digits = 2;
    const QString _install_datatype = ".zip";
    const QString _verifiction_file = "GW2TacO.exe";

    QJsonArray fetchReleases(int& err);
    QJsonObject extractLatestFullRelease(QJsonArray &releases);
    Version extractVersion(QJsonObject &release);
    Version readLocalVersion();
    int compareVersions(Version &local_version, Version &online_version);
    void installUpdate(QJsonObject &release, Version &online_version, int &err);
    QJsonObject getInstallAsset(QJsonObject& release, int& err);
    void installAsset(QJsonObject &asset, Version &online_version, int &err);
};

} // namespace Updater

#endif // GITHUPDATER_H
