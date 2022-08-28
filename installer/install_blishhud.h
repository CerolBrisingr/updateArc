#ifndef INSTALL_BLISHHUD_H
#define INSTALL_BLISHHUD_H

#include "installer.h"

namespace installer {

class install_blishhud : public installer
{
public:
    install_blishhud(QString gw_path);
    ~install_blishhud() override;

    QString prepare(QString temp_prefix) override;
    int install() override;
    int uninstall() override;

    QString _temp_filename = "";
    QString _verification_file = "Blish HUD.exe";
};

}; // namespace installer

#endif // INSTALL_BLISHHUD_H
