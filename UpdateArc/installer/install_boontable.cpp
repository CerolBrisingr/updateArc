#include "install_boontable.h"

namespace Installer {

InstallBoontable::InstallBoontable(QString gw_path)
    :Installer()
{
    _install_path = gw_path + "/bin64";
}

InstallBoontable::~InstallBoontable()
{}

QString InstallBoontable::prepare(QString temp_prefix)
{
    setUpTempPath(temp_prefix);
    _temp_filename = _temp_path + "/" + _target_filename;
    return _temp_filename;
}

int InstallBoontable::install()
{
    if(!isPrepared()) {
        Log::write("    Install is not set up yet!\n");
        return 1;
    }

    if (!QDir(_temp_path).exists(_target_filename)) {
        Log::write("    download did not produce expected results. Keeping what we got.\n");
        return 1;
    }

    Log::write("    Removing old files\n");
    this->uninstall();

    Log::write("    Moving file to target location\n");
    FileInteractions::copyFileTo(_temp_path + "/" + _target_filename, _install_path + "/" + _target_filename);
    FileInteractions::removeFolder(_temp_path);

    return 0;
}

int InstallBoontable::uninstall()
{
    FileInteractions::removeFile(_install_path, _target_filename);
    Log::write("  Removed boontable dll\n");
    return 0;
}

} // namespace installer
