#ifndef ARCDOWNLOADER_H
#define ARCDOWNLOADER_H

#include "baseupdater.h"

namespace Updater {

class ArcUpdater : public BaseUpdater
{
    Q_OBJECT
public:
    ArcUpdater(QString gw_path, QPushButton* install_button, QToolButton* remove_button,
               QCheckBox* checkbox);
    int remove() override;
    int update() override;

private slots:
    void removeSlot() override;

private:
    const QString _arc_blocker_key = "updaters/block_arcdps";

    int runArcUpdate();
    QString getRemoteHash();
    bool isBlockedArcVersion(QString sRemoteHash);
    bool verifyArcInstallation();
    bool downloadArc(QString pathname);
};

} // namespace Downloader

#endif // ARCDOWNLOADER_H
