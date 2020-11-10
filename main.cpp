#include "main.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    bool doExit;
    bool undoInstall;
    int returnValue = 1;
    UpdateTool _updater;

    evaluateInput(app, doExit, undoInstall);

    std::cout << "Searching gw2 install" << std::endl;
    if (verifyUpdaterLocation()) {
        // Perform core functionality
        if (undoInstall) {
            std::cout << "Running removal" << std::endl;
            returnValue = _updater.arcUninstaller();
            std::cout << "Uninstall finished" << std::endl;
        } else {
            std::cout << "Running updater" << std::endl;
            returnValue += _updater.updateArc();
            //returnValue += _updater.updateTaco();
            //returnValue += _updater.updateTekkit();
            std::cout << "Updater finished" << std::endl;
        }
    }

    // Exit after timer or stay open at user command
    if (doExit) {
        if (returnValue == 0) {
            std::cout << "Update completed successfully. Window closing in 10 seconds!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10));
        } else {
            std::cout << "An error occured. Please close manually!" << std::endl;
            std::cout << "Press <Enter>" << std::endl;
            std::cin.ignore();
        }
        return returnValue;
    } else {
        std::cout << std::endl << "No \"-closeAfter\" requested. Press <Enter> to close." << std::endl;
        std::cin.ignore();
        return returnValue;
    }
}

void evaluateInput(QCoreApplication &app, bool &doExit, bool &undoInstall){

    QStringList args = app.instance()->arguments();
    QString argument;

    doExit = false;
    undoInstall = false;

    // Read input argument, ignore arguments following the first one
    args.takeFirst(); // skip program name
    while (!args.isEmpty()) {
        argument = args.takeFirst();
        if (argument == "-removeArc")
            undoInstall = true;
        else if (argument == "-closeAfter") {
            doExit = true;
        } else {
            std::cout << "Invalid command \"" << argument.toStdString()
                      << "\", either use \"-closeAfter\" or \"-remove\"!" << std::endl;
        }
    }
}

bool verifyUpdaterLocation() {

    bool existGw2_64 = QDir("../").exists("Gw2-64.exe");
    bool existGw2_32 = QDir("../").exists("Gw2.exe");
    if (!(existGw2_32 ||  existGw2_64)){
        std::cout << "Could not find gw2 executable. Updater seems to be at wrong location" << std::endl;
        return false;
    }

    bool existBin64  = QDir("../bin64").exists();
    if (!(existBin64)) {
        std::cout << "Missing target folder \"bin64\"" << std::endl;
        return false;
    }

    return true;
}

