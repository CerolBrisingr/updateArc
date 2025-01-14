SOURCES += \
    helpers/sources/downloader.cpp \
    helpers/sources/logger.cpp \
    helpers/sources/version.cpp \
    classes/simple_json.cpp \
    installer/install_blishhud.cpp \
    installer/install_boontable.cpp \
    installer/install_kp.cpp \
    installer/installer.cpp \
    updater/arcupdater.cpp \
    updater/baseupdater.cpp \
    commandstring_editor/sources/form.cpp \
    main.cpp \
    main_window/sources/mainwindow.cpp \
    classes/fileinteractions.cpp \
    classes/settings.cpp \
    classes/updater.cpp \
    updater/config/githup_config.cpp \
    updater/githupdater.cpp

HEADERS += \
    helpers/includes/downloader.h \
    helpers/includes/logger.h \
    helpers/includes/version.h \
    classes/simple_json.h \
    installer/install_blishhud.h \
    installer/install_boontable.h \
    installer/install_kp.h \
    installer/installer.h \
    updater/arcupdater.h \
    updater/baseupdater.h \
    commandstring_editor/includes/form.h \
    main_window/includes/mainwindow.h \
    classes/fileinteractions.h \
    classes/settings.h \
    classes/updater.h \
    updater/config/githup_config.h \
    updater/githupdater.h

FORMS += \
    commandstring_editor/sources/form.ui \
    main_window/sources/mainwindow.ui
