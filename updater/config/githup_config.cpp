#include "githup_config.h"

namespace Updater {

namespace Config {

GithupdateConfig getBoontableConfig()
{
    GithupdateConfig boontable_config;
    boontable_config._github_user = "knoxfighter";
    boontable_config._github_project = "GW2-ArcDPS-Boon-Table";
    boontable_config._tag_prefix = "v";
    boontable_config._version_digits = 3;
    boontable_config._content_type = "application/x-msdownload";
    boontable_config._install_datatype = "_table.dll";

    return boontable_config;
}

GithupdateConfig getKpConfig()
{
    GithupdateConfig kp_config;
    kp_config._github_user = "blish-hud";
    kp_config._github_project = "arcdps-bhud";
    kp_config._tag_prefix = "v";
    kp_config._version_digits = 3;
    kp_config._content_type = "application/octet-stream";
    kp_config._install_datatype = "-msvc.zip";

    return kp_config;
}

GithupdateConfig getBlishConfig()
{
    GithupdateConfig blish_config;
    blish_config._github_user = "blish-hud";
    blish_config._github_project = "Blish-HUD";
    blish_config._tag_prefix = "v";
    blish_config._version_digits = 3;

    return blish_config;
}

} // namespace Config

} // namespace Updater
