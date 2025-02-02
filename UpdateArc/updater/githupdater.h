#ifndef GITHUPDATER_H
#define GITHUPDATER_H

#include "baseupdater.h"
#include <QObject>
#include <classes/simple_json.h>
#include "version.h"
#include <updater/config/githup_config.h>
#include <installer/installer.h>
#include <installer/install_blishhud.h>

namespace Updater {

class GitHupdater : public BaseUpdater
{
public:
    GitHupdater(QString& gw_path, QPushButton* install_button, QToolButton* remove_button,
                QCheckBox* checkbox, Updater::Config::GithupdateConfig cfg,
                const std::shared_ptr<Installer::Installer> installer);
    ~GitHupdater() override;

    int update() override;
    int remove() override;

private:
    const Updater::Config::GithupdateConfig _cfg;
    const QString _version_key;
    const std::shared_ptr<Installer::Installer> _installer;

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
