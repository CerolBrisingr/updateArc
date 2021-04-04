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
#include "downloader.h"
#include "settings.h"

class UpdateTool: public QObject
{
    Q_OBJECT
public:

    UpdateTool(Settings* settings);
    ~UpdateTool();

    bool verifyLocation();
    int arcUninstaller();

    int updateArc();
    int updateTaco();
    int updateTekkit();

    bool startGW2(QStringList arguments = QStringList());
    bool startTacO();

signals:

    void write_log(QString text);

private:

    Settings* _settings;
    QString _taco_install_key   = "Installed/TacoVersion";
    QString _tekkit_install_key = "Installed/TekkitVersion";
    QString _arc_blocker_key = "updaters/block_arcdps";
    QString _gw_path = "../..";  // Will possibly be updated by verifyLocation()
    QString _taco_path;
    QString _tekkit_path;

    void updateTargetPaths(QString gw_path);

    QString getRemoteHash();
    bool isBlockedArcVersion(QString sRemoteHash);
    bool verifyArcInstallation();
    bool downloadArc(QString pathname);
    int runArcUpdate();

    int16_t inquireCurrentTacoVersion(QString &tacoLink);
    bool canUpdateTaco(int16_t &onlineVersion);

    QVersionNumber inquireCurrentTekkitVersion(QString &tekkitLink);
    bool canUpdateTekkit(QVersionNumber &onlineVersion);

    QStringList loadGW2Arguments();

    void write(QString text);
    void writeline(QString text);
    QString _streamline;
    QTextStream _stream;
    void write();       // Writes content of _stream, flushes _stream
    void writeline();   // Writes content of _stream, flushes _stream, adds newline

};

#endif // UPDATER_H
