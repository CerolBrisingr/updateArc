#ifndef MAIN_H
#define MAIN_H

#include <QCoreApplication>

#include "classes/downloader.h"
#include "classes/fileinteractions.h"
#include "classes/updater.h"

#include <iostream>
#include <chrono>
#include <thread>

void evaluateInput(QCoreApplication &app, bool &undoInstall);

#endif // MAIN_H
