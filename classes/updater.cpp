#include "updater.h"

UpdateTool::UpdateTool()
    :_settings("settings.ini")
    ,_gw_path(findGwInstall())
    ,_blish_path(getBlishHudPath())
    ,_valid(_gw_path != "error")
{}

QString UpdateTool::getBlishHudPath() const noexcept
{
    Updater::Config::GithupdateConfig cfg = Updater::Config::getBlishConfig();
    return _gw_path + "/addons/" + cfg._github_project;
}

bool UpdateTool::isValid() const noexcept
{
    return _valid;
}

const QString& UpdateTool::getGwPath() const noexcept
{
    return _gw_path;
}

QString UpdateTool::findGwInstall() const
{
    bool existGw2_64 = false;
    bool existGw2_32 = false;

    QString gw_path = "..";
    for (int i = 0; i < 2; i++) {
        existGw2_64 = QDir(gw_path).exists("Gw2-64.exe");
        existGw2_32 = QDir(gw_path).exists("Gw2.exe");
        if (!(existGw2_32 || existGw2_64)){
            gw_path = "../..";
        } else {
            break;
        }
    }
    if (!(existGw2_32 || existGw2_64)){
        Log::write("Could not find gw2 executable. Updater seems to be at wrong location\n");
        return QString("error");
    }

    bool existBin64  = QDir(gw_path + "/bin64").exists();
    if (!(existBin64)) {
        Log::write("Missing target folder \"bin64\"\n");
        return QString("error");
    }
    return gw_path;
}

bool UpdateTool::startGW2(QStringList arguments)
{
    QProcess gw2;
    gw2.setWorkingDirectory(_gw_path);
    if (arguments.empty()) {
        gw2.setArguments(loadGW2Arguments());
    } else
    {
        gw2.setArguments(arguments);
    }
    if (QDir(_gw_path).exists("Gw2-64.exe")) {
        gw2.setProgram(_gw_path + "/Gw2-64.exe");
    } else {
        gw2.setProgram(_gw_path + "/Gw2.exe");
    }
    if(gw2.startDetached()) {
        Log::write("-- started GuildWars2\n");
        return true;
    } else {
        Log::write("-- start of GuildWars2 failed\n");
        return false;
    }
}

QStringList UpdateTool::loadGW2Arguments() {

    QString argument_chain = _settings.getValue("starters/gw2_arguments", "");
    QStringList arguments;
    QStringList split_by_spaces = argument_chain.split(" ", QString::SplitBehavior::SkipEmptyParts);
    for (auto& argument: split_by_spaces) {
        arguments.append(argument.trimmed());
    }
    return arguments;
}

bool UpdateTool::startTacO()
{
    QProcess taco;
    taco.setWorkingDirectory(_taco_path);
    taco.setProgram(_taco_path + "/GW2TacO.exe");
    if (taco.startDetached()) {
        Log::write("-- started TacO\n");
        return true;
    } else {
        Log::write("-- start of TacO failed\n");
        return false;
    }
}

bool UpdateTool::startBlish()
{
    QProcess taco;
    taco.setWorkingDirectory(_blish_path);
    taco.setProgram(_blish_path + "/Blish HUD.exe");
    if (taco.startDetached()) {
        Log::write("-- started Blish-HUD\n");
        return true;
    } else {
        Log::write("-- start of Blish-HUD failed\n");
        return false;
    }
}
