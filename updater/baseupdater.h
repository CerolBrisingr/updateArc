#ifndef BASEDOWNLOADER_H
#define BASEDOWNLOADER_H

#include <QObject>
#include <QPushButton>
#include <QToolButton>
#include <QCheckBox>
#include <classes/settings.h>
#include <classes/logger.h>
#include <classes/fileinteractions.h>
#include <classes/downloader.h>

namespace Updater {

class BaseUpdater: public QObject
{
    Q_OBJECT
public:
    BaseUpdater(QString& gw_path, QPushButton* install_button, QToolButton* remove_button,
                QCheckBox* checkbox, QString settings_key);
    virtual ~BaseUpdater() override;
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
    CheckBoxSetting _box_setting;
};

} // namespace Updater

#endif // BASEDOWNLOADER_H
