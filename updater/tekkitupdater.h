#ifndef TEKKITUPDATER_H
#define TEKKITUPDATER_H

#include "baseupdater.h"

namespace Updater {

class TekkitUpdater : public BaseUpdater
{
public:
    TekkitUpdater(QString& gw_path, QPushButton* install_button, QToolButton* remove_button,
                  QCheckBox* checkbox, QString settings_key);

    int update() override;
    int remove() override;
private:
    const QString _tekkit_install_key = "version_installed/TekkitVersion";
    const QString _tekkit_path_key = "customize/TekkitPath";
    const QString _tekkit_path;

    bool canUpdateTekkit(QVersionNumber &onlineVersion);
    QVersionNumber inquireCurrentTekkitVersion(QString &tekkitLink);
    QString getTekkitPath();
};

} // namespace Updater

#endif // TEKKITUPDATER_H
