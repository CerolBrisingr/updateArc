#ifndef TACOUPDATER_H
#define TACOUPDATER_H

#include "baseupdater.h"

namespace Updater {

class TacoUpdater : public BaseUpdater
{
    Q_OBJECT
public:
    TacoUpdater(QString& gw_path, QPushButton* install_button, QToolButton* remove_button,
                QCheckBox* checkbox, QString settings_key);

    int update() override;
    int remove() override;

private:
    const QString _taco_path;
    const QString _taco_install_key = "Installed/TacoVersion";

    int16_t inquireCurrentTacoVersion(QString &tacoLink);
    bool canUpdateTaco(int16_t &onlineVersion);
};

} // namespace Updater

#endif // TACOUPDATER_H
