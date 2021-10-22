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

    UpdateTool();
    ~UpdateTool() = default;

    bool startGW2(QStringList arguments = QStringList());
    bool startTacO();

    bool isValid() const noexcept;
    const QString& getGwPath() const noexcept;
private:
    Settings _settings; // settings.ini
    const QString _gw_path;
    const QString _taco_path;
    const bool _valid;

    QString findGwInstall() const;
    QStringList loadGW2Arguments();

};

#endif // UPDATER_H
