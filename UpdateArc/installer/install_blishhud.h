#ifndef INSTALL_BLISHHUD_H
#define INSTALL_BLISHHUD_H

#include "installer.h"

namespace Installer {

class InstallBlishhud : public Installer
{
public:
    InstallBlishhud(QString gw_path);
    ~InstallBlishhud() override;

    QString prepare(QString temp_prefix) override;
    int install() override;
    int uninstall() override;

    QString _temp_filename = "";
    QString _verification_file = "Blish HUD.exe";
};

}; // namespace installer

#endif // INSTALL_BLISHHUD_H
