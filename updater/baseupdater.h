#ifndef BASEDOWNLOADER_H
#define BASEDOWNLOADER_H

#include <QObject>
#include <QPushButton>
#include <QCheckBox>
#include <classes/settings.h>
#include <classes/logger.h>
#include <classes/fileinteractions.h>

namespace Updater {

struct Config
{
    const QString _gw_path;
    QPushButton* const _button;
    QCheckBox* const _checkbox;
    // Work with a vector this time and iterate.
    int _version_digits;

    Config(QString gw_path, QPushButton* button, QCheckBox* checkbox, int version_digits = 0)
        :_gw_path(gw_path)
        ,_button(button)
        ,_checkbox(checkbox)
        ,_version_digits(version_digits)
    {}
};

class BaseUpdater: public QObject
{
    Q_OBJECT
public:
    BaseUpdater(Config config);
    virtual ~BaseUpdater();
    virtual int update() = 0;
    int autoUpdate();

private slots:
    virtual void updateSlot();

protected:
    Config _config;
    Settings _settings; // "settings.ini"
};

} // namespace Downloader

#endif // BASEDOWNLOADER_H
