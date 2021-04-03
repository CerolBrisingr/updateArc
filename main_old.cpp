#include "main.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    bool undoInstall;
    int returnValue = 0;
    UpdateTool _updater;

    evaluateInput(app, undoInstall);

    std::cout << "Searching gw2 install" << std::endl;
    if (_updater.verifyLocation()) {
        // Perform core functionality
        if (undoInstall) {
            std::cout << "Running removal" << std::endl;
            returnValue += _updater.arcUninstaller();
            std::cout << "Uninstall finished" << std::endl;
        } else {
            std::cout << "Running updater" << std::endl;
            returnValue += _updater.updateArc();
            returnValue += _updater.updateTaco();
            returnValue += _updater.updateTekkit();
            std::cout << "Updater finished" << std::endl;
        }
    }

    // Execute starters
    _updater.startGW2();
    _updater.startTacO();

    // Exit after timer or stay open at user command
    if (_updater.getSetting2("ProgramFlow/CloseAfterNoError", "no") == "yes") {
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
        std::cout << std::endl << "Update completed successfully!" << std::endl;
        std::cout << "Press <Enter> to close." << std::endl;
        std::cin.ignore();
        return returnValue;
    }
}

void evaluateInput(QCoreApplication &app, bool &undoInstall){

    QStringList args = app.instance()->arguments();
    QString argument;

    undoInstall = false;

    // Read input argument, ignore arguments following the first one
    args.takeFirst(); // skip program name
    while (!args.isEmpty()) {
        argument = args.takeFirst();
        if (argument == "-removeArc")
            undoInstall = true;
        else {
            std::cout << "Invalid command \"" << argument.toStdString()
                      << "\", currently available: \"-removeArc\"!" << std::endl;
        }
    }
}

