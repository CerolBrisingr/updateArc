#ifndef ARCDOWNLOADER_H
#define ARCDOWNLOADER_H

#include "baseupdater.h"

namespace Updater {

class ArcUpdater : public BaseDownloader
{
    Q_OBJECT
public:
    ArcUpdater(Config config);
    int remove() override;
    int update() override;

private:
    QString _arc_blocker_key = "updaters/block_arcdps";

    int runArcUpdate();

    QString getRemoteHash();
    bool isBlockedArcVersion(QString sRemoteHash);
    bool verifyArcInstallation();
    bool downloadArc(QString pathname);
};

} // namespace Downloader

#endif // ARCDOWNLOADER_H
