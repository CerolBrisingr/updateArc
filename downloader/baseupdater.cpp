#include "baseupdater.h"

namespace Updater {

BaseDownloader::BaseDownloader(Config config)
    :_config(config)
    ,_settings("settings.ini")
{

}

} // namespace Downloader
