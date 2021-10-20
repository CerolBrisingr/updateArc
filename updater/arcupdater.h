#ifndef ARCDOWNLOADER_H
#define ARCDOWNLOADER_H

#include "baseupdater.h"
#include <QToolButton>

namespace Updater {

class ArcUpdater : public BaseUpdater
{
    Q_OBJECT
public:
    ArcUpdater(Config config, QToolButton* button_remove);
    int remove();
    int update() override;

private slots:
    void updateSlot() override;
    void removeSlot();

private:
    const QString _arc_blocker_key = "updaters/block_arcdps";
    QToolButton* _button_remove;

    int runArcUpdate();
    QString getRemoteHash();
    bool isBlockedArcVersion(QString sRemoteHash);
    bool verifyArcInstallation();
    bool downloadArc(QString pathname);
};

} // namespace Downloader

#endif // ARCDOWNLOADER_H
