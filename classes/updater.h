#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QSettings>
#include <QRegularExpression>
#include <QVersionNumber>
#include <QTextStream>

#include <chrono>
#include <thread>

#include "fileinteractions.h"
#include "logger.h"
#include "downloader.h"
#include "settings.h"

class UpdateTool: public QObject
{
    Q_OBJECT
public:

    UpdateTool(Settings* settings);
    ~UpdateTool();

    bool verifyLocation();

    int updateTaco();
    int updateTekkit();

    bool startGW2(QStringList arguments = QStringList());
    bool startTacO();

    QString _gw_path = "../..";  // Will possibly be updated by verifyLocation()

private:

    Settings* _settings;
    QString _taco_install_key   = "Installed/TacoVersion";
    QString _tekkit_install_key = "Installed/TekkitVersion";
    QString _tekkit_path_key = "Installed/TekkitPath";
    QString _arc_blocker_key = "updaters/block_arcdps";
    QString _taco_path;
    QString _tekkit_path;

    void updateTargetPaths(QString gw_path);

    int16_t inquireCurrentTacoVersion(QString &tacoLink);
    bool canUpdateTaco(int16_t &onlineVersion);

    QVersionNumber inquireCurrentTekkitVersion(QString &tekkitLink);
    bool canUpdateTekkit(QVersionNumber &onlineVersion);

    QStringList loadGW2Arguments();

};

#endif // UPDATER_H
