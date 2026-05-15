#include "ui/mainwindow.h"
#include "helper/downloader.h"

#include <QSettings>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Apply settings
    QString path = a.applicationDirPath() + "/settings.ini";
    QSettings setting(path, QSettings::IniFormat);
    Downloader::setPrintDebug(setting.value("debug/downloader", "off").toString() == "on");

    w.show();

    w.evaluateAutorun();
    return a.exec();
}
