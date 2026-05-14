#ifndef UPDATER_H
#define UPDATER_H

#include "helper/fileinteractions.h"
#include "helper/logger.h"
#include "helper/settings.h"
#include "config/githup_config.h"

#include <QObject>
#include <QSettings>
#include <QRegularExpression>
#include <QVersionNumber>
#include <QTextStream>

#include <chrono>
#include <thread>
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
