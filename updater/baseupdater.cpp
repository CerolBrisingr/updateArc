#include "baseupdater.h"

namespace Updater {

BaseUpdater::BaseUpdater(Config config)
    :_config(config)
    ,_settings("settings.ini")
{
    connect(_config._button, SIGNAL(clicked()),
            this, SLOT(updateSlot()));
}

BaseUpdater::~BaseUpdater()
{}

void BaseUpdater::updateSlot()
{
    _config._button->setEnabled(false);
    update();
    _config._button->setEnabled(true);
}

int BaseUpdater::autoUpdate()
{
    if (_config._checkbox->isChecked()) {
        return update();
    } else {
        return 1;
    }
}

} // namespace Downloader
