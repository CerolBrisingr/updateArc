#include "installer.h"

namespace installer {

installer::installer()
{}

installer::~installer()
{}

void installer::setUpTempPath(QString temp_prefix)
{
    _temp_prefix = temp_prefix;
    _temp_path = _temp_prefix + "_temp";

    // Removing possible remains from previous update
    fileInteractions::removeFolder(_temp_path);
    fileInteractions::createFolder(_temp_path);
    return;
}

void installer::cleanUpAfterInstall()
{
    // Don't do a thing it project is not configured anyway
    if(!isPrepared())
        return;
    // Remove files we don't need any more
    fileInteractions::removeFolder(_temp_path);
    _temp_path = "";
    _temp_prefix = "";
}

bool installer::isPrepared()
{
    // Preparation needs to set up a non-empty project name
    bool hasPrefix = !_temp_prefix.isEmpty();
    bool hasInstallPath = !_install_path.isEmpty();
    return hasPrefix && hasInstallPath;
}

} // namespace installer
