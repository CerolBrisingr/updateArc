QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    classes/logger.cpp \
    classes/simple_json.cpp \
    updater/arcupdater.cpp \
    updater/baseupdater.cpp \
    form.cpp \
    main.cpp \
    mainwindow.cpp \
    classes/downloader.cpp \
    classes/fileinteractions.cpp \
    classes/settings.cpp \
    classes/updater.cpp \
    updater/config/githup_config.cpp \
    updater/githupdater.cpp \
    updater/tekkitupdater.cpp \
    version_recognition/version.cpp

HEADERS += \
    classes/logger.h \
    classes/simple_json.h \
    updater/arcupdater.h \
    updater/baseupdater.h \
    form.h \
    mainwindow.h \
    classes/downloader.h \
    classes/fileinteractions.h \
    classes/settings.h \
    classes/updater.h \
    updater/config/githup_config.h \
    updater/githupdater.h \
    updater/tekkitupdater.h \
    version_recognition/version.h

FORMS += \
    form.ui \
    mainwindow.ui

RC_ICONS = resources\arc.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
