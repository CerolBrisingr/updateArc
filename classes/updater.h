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

    bool startGW2();
    bool startTacO();

    bool hasSetting(QString key);
    QString getSetting(QString key, QString default_value = "");
    QString getSetting2(QString key, QString default_value = "");
private:

    QString _ini_path = "settings.ini";
    QString _taco_install_key   = "Installed/TacoVersion";
    QString _tekkit_install_key = "Installed/TekkitVersion";
    QString _arc_blocker_key = "Blocker/ArcDPS";
    QString _taco_path = "../addons/TacO";
    QString _tekkit_path = _taco_path + "/POIs";

    void setSetting(QString key, QString value);
    void removeSetting(QString key);

    QString getRemoteHash();
    bool isBlockedArcVersion(QString sRemoteHash);
    bool verifyArcInstallation();
    bool downloadArc(QString pathname);
    int runArcUpdate();

    int16_t inquireCurrentTacoVersion(QString &tacoLink);
    bool canUpdateTaco(int16_t &onlineVersion);

    QVersionNumber inquireCurrentTekkitVersion(QString &tekkitLink);
    bool canUpdateTekkit(QVersionNumber &onlineVersion);

    QVersionNumber inquireCurrent7zipVersion(QString &sevenZipLink);

    QStringList readGW2Arguments();

};

#endif // UPDATER_H
