#ifndef BASEDOWNLOADER_H
#define BASEDOWNLOADER_H

#include <QObject>

namespace Downloader {

struct Config
{
    QString _target_path;
    QString _web_source;
    // Work with a vector this time and iterate.
    int _version_digits;
};

class BaseDownloader
{
    Q_OBJECT
public:
    BaseDownloader();
    virtual ~BaseDownloader();

public slots:
    virtual void update() = 0;
    virtual void remove() = 0;
};

} // namespace Downloader

#endif // BASEDOWNLOADER_H
