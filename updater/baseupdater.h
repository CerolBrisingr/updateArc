#ifndef BASEDOWNLOADER_H
#define BASEDOWNLOADER_H

#include <QObject>
#include <QPushButton>
#include <QToolButton>
#include <QCheckBox>
#include <classes/settings.h>
#include <classes/logger.h>
#include <classes/fileinteractions.h>

namespace Updater {

struct Config
{
    const QString _gw_path;
    QPushButton* const _install_button;
    QToolButton* const _remove_button;
    QCheckBox* const _checkbox;

    Config(QString gw_path, QPushButton* install_button, QToolButton* remove_button,
           QCheckBox* checkbox)
        :_gw_path(gw_path)
        ,_install_button(install_button)
        ,_remove_button(remove_button)
        ,_checkbox(checkbox)
    {}
};

class BaseUpdater: public QObject
{
    Q_OBJECT
public:
    BaseUpdater(QString gw_path, QPushButton* install_button, QToolButton* remove_button,
                QCheckBox* checkbox);
    virtual ~BaseUpdater();
    virtual int update() = 0;
    virtual int remove() = 0;
    int autoUpdate();

private slots:
    void updateSlot();
    virtual void removeSlot();

protected:
    const QString _gw_path;
    QPushButton* const _install_button;
    QToolButton* const _remove_button;
    QCheckBox* const _checkbox;
    Settings _settings; // "settings.ini"
};

} // namespace Downloader

#endif // BASEDOWNLOADER_H
