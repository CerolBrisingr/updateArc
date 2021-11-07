#ifndef TEKKITUPDATER_H
#define TEKKITUPDATER_H

#include "baseupdater.h"
#include "version_recognition/version.h"
#include <QStandardPaths>

namespace Updater {

namespace Tekkit {

enum class Target {
    TACO,
    BLISH
};

struct TkTarget {
    QString _install_path;
    QString _target_version_key;

    TkTarget(QString path, QString version_key)
        :_install_path(path)
        ,_target_version_key(version_key)
    {}
};

}

class TekkitUpdater : public BaseUpdater
{
public:
    TekkitUpdater(QString& gw_path, QPushButton* install_button, QToolButton* remove_button,
                  QCheckBox* checkbox, QString settings_key);

    int update() override;
    int remove() override;
private:
    const QString _tekkit_install_key = "version_installed/tekkit";
    const Tekkit::TkTarget _taco_info;
    const Tekkit::TkTarget _blish_info;
    const QString _tekkit_path_taco;
    const QString _tekkit_path_blish;
    const QString _filename = "tw_ALL_IN_ONE.taco";
    const size_t _version_digits = 3;

    int prepareTargets();
    int cleanOrCount(const Tekkit::TkTarget& target);
    bool canUpdateTekkit(Version &online_version);
    Version inquireCurrentTekkitVersion(QString &tekkit_link);
    bool isTargetSetUp(const Tekkit::TkTarget &target);
    void updateTarget(const Tekkit::TkTarget &target);
    QString getTacoMarkerPath();
    QString getBlishMarkerPath();
    void removeTarget(const Tekkit::TkTarget &target);
};

} // namespace Updater

#endif // TEKKITUPDATER_H
