#include "mainwindow.h"
#include "classes/logger.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    w.evaluate_autorun();
    return a.exec();
}
