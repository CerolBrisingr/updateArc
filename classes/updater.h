#ifndef UPDATER_H
#define UPDATER_H

#include <QSettings>
#include <QRegularExpression>
#include <QVersionNumber>
#include <QtZlib/zlib.h>

#include "fileinteractions.h"
#include "downloader.h"

class UpdateTool
{
public:

    UpdateTool();
    ~UpdateTool();

    int arcUninstaller();

    int updateArc();
    int updateTaco();
    int updateTekkit();
    int update7zip();
private:

    QString _ini_path = "settings.ini";
    QString _taco_install_key   = "Installed/TacoVersion";
    QString _tekkit_install_key = "Installed/TekkitVersion";
    QString _arc_blocker_key = "Blocker/ArcDPS";

    bool hasSetting(QString key);
    void setSetting(QString key, QString value);
    QString getSetting(QString key, QString default_value = "");
    void removeSetting(QString key);

    QString getRemoteHash();
    bool isBlockedArcVersion(QString sRemoteHash);
    bool verifyArcInstallation();
    bool downloadArc(QString pathname);

    int16_t inquireCurrentTacoVersion(QString &tacoLink);
    bool canUpdateTaco(int16_t &onlineVersion);

    QVersionNumber inquireCurrentTekkitVersion(QString &tekkitLink);
    bool canUpdateTekkit(QVersionNumber &onlineVersion);

    QVersionNumber inquireCurrent7zipVersion(QString &sevenZipLink);

};

#endif // UPDATER_H
