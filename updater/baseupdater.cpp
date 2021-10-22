#include "baseupdater.h"

namespace Updater {

BaseUpdater::BaseUpdater(QString& gw_path, QPushButton* install_button, QToolButton* remove_button,
                         QCheckBox* checkbox)
    :_gw_path(gw_path)
    ,_install_button(install_button)
    ,_remove_button(remove_button)
    ,_checkbox(checkbox)
    ,_settings("settings.ini")
{
    connect(_install_button, SIGNAL(clicked()),
            this, SLOT(updateSlot()));

    connect(_remove_button, SIGNAL(clicked()),
            this, SLOT(removeSlot()));
}

BaseUpdater::~BaseUpdater()
{}

void BaseUpdater::updateSlot()
{
    _install_button->setEnabled(false);
    _remove_button->setEnabled(false);
    update();
    _remove_button->setEnabled(true);
    _install_button->setEnabled(true);
}

void BaseUpdater::removeSlot()
{
    _install_button->setEnabled(false);
    _remove_button->setEnabled(false);
    remove();
    _remove_button->setEnabled(true);
    _install_button->setEnabled(true);
}

int BaseUpdater::autoUpdate()
{
    if (_checkbox->isChecked()) {
        _install_button->setEnabled(false);
        _remove_button->setEnabled(false);
        auto result = update();
        _remove_button->setEnabled(true);
        _install_button->setEnabled(true);
        return result;
    } else {
        return 1;
    }
}

} // namespace Downloader
