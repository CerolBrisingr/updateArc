#include "install_blishhud.h"

namespace Installer {

InstallBlishhud::InstallBlishhud(QString gw_path)
    :Installer()
{
    _install_path = gw_path + "/addons/Blish-HUD";
}

InstallBlishhud::~InstallBlishhud()
{}

QString InstallBlishhud::prepare(QString temp_prefix)
{
    setUpTempPath(temp_prefix);
    _temp_filename = _temp_path + "/download.zip";
    return _temp_filename;
}

int InstallBlishhud::install()
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

    if (!QDir(_temp_path).exists(_verification_file)) {
        Log::write("    archive extraction did not produce expected results. Keeping downloads.\n");
        return 1;
    }

    Log::write("    Moving files to target location\n");
    FileInteractions::removeFile("", _temp_filename);
    FileInteractions::copyFolderTo(_temp_path, _install_path);
    FileInteractions::removeFolder(_temp_path);

    return 0;
}

int InstallBlishhud::uninstall()
{
    FileInteractions::removeFolder(_install_path);
    Log::write("Removed install folder for <Blish-HUD>\n");
    return 0;
}

} // namespace installer
