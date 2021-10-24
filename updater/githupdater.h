#ifndef GITHUPDATER_H
#define GITHUPDATER_H

#include "baseupdater.h"
#include <QObject>
#include <classes/simple_json.h>

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
    const QString _version_key;

    QJsonObject fetchReleases(int& err);
};

} // namespace Updater

#endif // GITHUPDATER_H
