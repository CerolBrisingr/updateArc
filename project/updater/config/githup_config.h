#ifndef GITHUP_CONFIG_H
#define GITHUP_CONFIG_H

#include <QString>

namespace Updater {

namespace Config {

struct GithupdateConfig
{
    QString _github_user;
    QString _github_project;
    QString _tag_prefix = "";
    QString _tag_suffix = "";
    QString _content_type = "application/x-zip-compressed";
    size_t _version_digits;
    QString _install_datatype = ".zip";
};

GithupdateConfig getBoontableConfig();
GithupdateConfig getKpConfig();
GithupdateConfig getBlishConfig();

} // namespace Config

} // namespace Updater

#endif // GITHUP_CONFIG_H
