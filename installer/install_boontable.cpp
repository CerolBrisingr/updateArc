#include "install_boontable.h"

namespace installer {

install_boontable::install_boontable(QString gw_path)
    :installer()
{
    _install_path = gw_path + "/bin64";
}

install_boontable::~install_boontable()
{}

QString install_boontable::prepare(QString temp_prefix)
{
    setUpTempPath(temp_prefix);
    _temp_filename = _temp_path + "/" + _target_filename;
    return _temp_filename;
}

int install_boontable::install()
{
    if(!isPrepared()) {
        Log::write("    Install is not set up yet!\n");
        return 1;
    }

    if (!QDir(_temp_path).exists(_target_filename)) {
        Log::write("    download did not produce expected results. Keeping what we got.\n");
        return 1;
    }

    Log::write("    Moving file to target location\n");
    fileInteractions::copyFileTo(_temp_path + "/" + _target_filename, _install_path + "/" + _target_filename);
    fileInteractions::removeFolder(_temp_path);

    return 0;
}

int install_boontable::uninstall()
{
    fileInteractions::removeFile(_install_path, _target_filename);
    Log::write("Removed boontable dll\n");
    return 0;
}

} // namespace installer
