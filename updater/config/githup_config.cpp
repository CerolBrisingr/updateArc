#include "githup_config.h"

namespace Updater {

namespace Config {

GithupdateConfig getTacoConfig()
{
    GithupdateConfig taco_config;
    taco_config._github_user = "BoyC";
    taco_config._github_project = "GW2TacO";
    taco_config._verifiction_file = "GW2TacO.exe";
    taco_config._tag_suffix = "r";
    taco_config._version_digits = 2;

    return taco_config;
}

GithupdateConfig getBlishConfig()
{
    GithupdateConfig blish_config;
    blish_config._github_user = "blish-hud";
    blish_config._github_project = "Blish-HUD";
    blish_config._verifiction_file = "Blish HUD.exe";
    blish_config._tag_prefix = "v";
    blish_config._version_digits = 3;

    return blish_config;
}

} // namespace Config

} // namespace Updater
