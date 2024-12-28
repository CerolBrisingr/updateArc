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
#include "updater/config/githup_config.h"

class UpdateTool: public QObject
{
    Q_OBJECT
public:

    UpdateTool();

    bool startGW2(QStringList arguments = QStringList());
    bool startTacO();
    bool startBlish();

    bool isValid() const noexcept;
    const QString& getGwPath() const noexcept;
private:
    Settings _settings; // settings.ini
    const QString _gw_path;
    const QString _taco_path;
    const QString _blish_path;
    const bool _valid;

    QString findGwInstall() const;
    QStringList loadGW2Arguments();

    QString getBlishHudPath() const noexcept;
};

#endif // UPDATER_H
