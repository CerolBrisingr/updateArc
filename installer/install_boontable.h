#ifndef INSTALL_BOONTABLE_H
#define INSTALL_BOONTABLE_H

#include "installer.h"

namespace Installer {

class InstallBoontable : public Installer
{
public:
    InstallBoontable(QString gw_path);
    ~InstallBoontable() override;

    QString prepare(QString temp_prefix) override;
    int install() override;
    int uninstall() override;

    QString _temp_filename = "";
    QString _target_filename = "d3d9_arcdps_table.dll";
};

}; // namespace installer

#endif // INSTALL_BOONTABLE_H
