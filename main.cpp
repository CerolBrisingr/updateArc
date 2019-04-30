#include <QCoreApplication>
#include <iostream>
#include <QCryptographicHash>
#include <QFile>
#include <QTimer>
#include <QString>
#include <QTextStream>
#include "downloader.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    downloader netSource;

    netSource.fetch("https://www.google.de/images/branding/googlelogo/1x/googlelogo_color_272x92dp.png");

    // Read hash of currently 'installed' arcdps
    QCryptographicHash crypto(QCryptographicHash::Md5);
    QFile currentDll("../UpdateArc/d3d9.dll");
    if (!currentDll.open(QFile::ReadOnly)) {
        std::cout << "Unable to read arcdps file" << std::endl;
        return 1;
    }
    while(!currentDll.atEnd()){
      crypto.addData(currentDll.read(8192));
    }
    QString hash = crypto.result().toHex();

    std::cout << "File: " << hash.toStdString() << std::endl;


    // Read hash from online version
    QFile referenceFile("../UpdateArc/d3d9.dll.md5sum");
    if (!referenceFile.open(QFile::ReadOnly | QFile::Text)) {
        std::cout << "Could not open reference file" << std::endl;
        return 1;
    }
    QTextStream line(&referenceFile);
    QString refLine = line.readLine();

    std::cout << "Ref:  " << refLine.toStdString() << std::endl;

    if (refLine.contains(hash)) {
        std::cout << "Match! Already running newest version!" << std::endl;
        return 0;
    }

    std::cout << "End of program" << std::endl;
    return app.exec();


}
