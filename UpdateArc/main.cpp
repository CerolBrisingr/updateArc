#include "mainwindow.h"

#include <QSettings>
#include <QApplication>
#include "downloader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Apply settings
    QSettings setting("settings.ini", QSettings::IniFormat);
    Downloader::setPrintDebug(setting.value("debug/downloader", "off").toString() == "on");

    w.show();

    w.evaluateAutorun();
    return a.exec();
}
