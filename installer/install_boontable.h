#ifndef INSTALL_BOONTABLE_H
#define INSTALL_BOONTABLE_H

#include "installer.h"

namespace installer {

class install_boontable : public installer
{
public:
    install_boontable(QString gw_path);
    ~install_boontable() override;

    QString prepare(QString temp_prefix) override;
    int install() override;
    int uninstall() override;

    QString _temp_filename = "";
    QString _target_filename = "d3d9_arcdps_table.dll";
};

}; // namespace installer

#endif // INSTALL_BOONTABLE_H
