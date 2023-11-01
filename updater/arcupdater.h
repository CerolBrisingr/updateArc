#ifndef ARCDOWNLOADER_H
#define ARCDOWNLOADER_H

#include "baseupdater.h"

namespace Updater {

class ArcUpdater : public BaseUpdater
{
    Q_OBJECT
public:
    ArcUpdater(QString &gw_path, QPushButton* install_button, QToolButton* remove_button,
               QCheckBox* checkbox, QString settings_key);
    int remove() override;
    int update() override;

private slots:
    void removeSlot() override;

private:
    const QString _arc_blocker_key = "updaters/block_arcdps";

    bool fileRemoval(int& err);

    int runArcUpdate();
    QString getRemoteHash();
    bool isBlockedArcVersion(QString sRemoteHash);
    bool verifyArcInstallation();
    bool downloadArc(QString pathname);

    void testCanUpdate(int& err, QString sRemoteHash);
    bool runningUpdate(int& err, QString sRemoteHash, QString sLocalHash);
    void testUpdatedVersion(int& err, QString sRemoteHash, QString sLocalHash);
};

} // namespace Updater

#endif // ARCDOWNLOADER_H
