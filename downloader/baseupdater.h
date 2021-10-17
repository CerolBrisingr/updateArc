#ifndef BASEDOWNLOADER_H
#define BASEDOWNLOADER_H

#include <QObject>
#include <classes/settings.h>
#include <classes/logger.h>
#include <classes/fileinteractions.h>

namespace Updater {

struct Config
{
    QString _gw_path;
    QString _web_source;
    // Work with a vector this time and iterate.
    int _version_digits;

    Config(QString gw_path, QString web_source, int version_digits = 0)
        :_gw_path(gw_path)
        ,_web_source(web_source)
        ,_version_digits(version_digits)
    {}
};

class BaseDownloader
{
    Q_OBJECT
public:
    BaseDownloader(Config config);
    virtual ~BaseDownloader();

public slots:
    virtual int autoUpdate() = 0;
    virtual int update() = 0;
    virtual int remove() = 0;

protected:
    Config _config;
    Settings _settings; // "settings.ini"
};

} // namespace Downloader

#endif // BASEDOWNLOADER_H
