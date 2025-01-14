#ifndef INSTALLER_H
#define INSTALLER_H

#include "classes/fileinteractions.h"
#include "logger.h"
#include <memory>

namespace Installer {

class Installer
{
public:
    Installer();
    virtual ~Installer();

    virtual QString prepare(QString temp_prefix) = 0;  // Prepare download folder and return download path. Returns empty string on fail
    virtual int install() = 0;          // Verifies basic download completeness and then moves it in place
    virtual int uninstall() = 0;        // Removes installed file(s)

    QString _install_path = "";
    QString _temp_prefix = "";
    QString _temp_path = "";

protected:
    void setUpTempPath(QString temp_prefix);
    void cleanUpAfterInstall();
    bool isPrepared();
};

}; // namespace installer

#endif // INSTALLER_H
