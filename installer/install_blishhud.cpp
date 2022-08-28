#include "install_blishhud.h"

namespace installer {

install_blishhud::install_blishhud(QString gw_path)
    :installer(gw_path)
{}

install_blishhud::~install_blishhud()
{}

QString install_blishhud::prepare(QString temp_prefix)
{
    setUpTempPath(temp_prefix);
    _temp_filename = _temp_path + "/download.zip";
    return _temp_filename;
}

int install_blishhud::install()
{
    if(!isPrepared()) {
        Log::write("    Install is not set up yet!\n");
        return 1;
    }

    Log::write("    Extracting archive\n");
    if (!fileInteractions::unzipArchive(_temp_filename, _temp_path)) {
        Log::write("    archive extraction failed\n");
        return 1;
    }

    if (!QDir(_temp_path).exists(_verification_file)) {
        Log::write("    archive extraction did not produce expected results. Keeping downloads.\n");
        return 1;
    }

    Log::write("    Moving files to target location\n");
    fileInteractions::copyFolderTo(_temp_path, _install_path);
    fileInteractions::removeFolder(_temp_path);
    fileInteractions::removeFile("", _temp_filename);

    return 0;
}

int install_blishhud::uninstall()
{
    fileInteractions::removeFolder(_install_path);
    Log::write("Removed install folder for <Blish-HUD>\n");
    return 0;
}

} // namespace installer
