#include "install_kp.h"

namespace Installer {

InstallKp::InstallKp(QString gw_path)
    :Installer()
{
    _install_path = gw_path + "/bin64";
}

InstallKp::~InstallKp()
{}

QString InstallKp::prepare(QString temp_prefix)
{
    setUpTempPath(temp_prefix);
    _temp_filename = _temp_path + "/bridge.zip";
    return _temp_filename;
}

int InstallKp::install()
{
    if(!isPrepared()) {
        Log::write("    Install is not set up yet!\n");
        return 1;
    }

    Log::write("    Extracting archive\n");
    if (!FileInteractions::unzipArchive(_temp_filename, _temp_path)) {
        Log::write("    archive extraction failed\n");
        return 1;
    }

    if (!QDir(_temp_path).exists(_target_filename)) {
        Log::write("    download did not produce expected results. Keeping what we got so far.\n");
        return 1;
    }

    Log::write("    Moving file to target location\n");
    FileInteractions::copyFileTo(_temp_path + "/" + _target_filename, _install_path + "/" + _target_filename);
    FileInteractions::removeFolder(_temp_path);

    return 0;
}

int InstallKp::uninstall()
{
    FileInteractions::removeFile(_install_path, _target_filename);
    Log::write("Removed ArcDPS/Blish-HUD bridge dll\n");
    return 0;
}

} // namespace installer
