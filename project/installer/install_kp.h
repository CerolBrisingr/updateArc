#ifndef INSTALL_KP_H
#define INSTALL_KP_H

#include "installer.h"

namespace Installer {

class InstallKp : public Installer
{
public:
    InstallKp(QString gw_path);
    ~InstallKp() override;

    QString prepare(QString temp_prefix) override;
    int install() override;
    int uninstall() override;

    QString _temp_filename = "";
    QString _target_filename = "arcdps_bhud.dll";
};

}; // namespace installer

#endif // INSTALL_KP_H
