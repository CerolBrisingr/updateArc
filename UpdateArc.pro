QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    classes/logger.cpp \
    classes/simple_json.cpp \
    downloader/basedownloader.cpp \
    form.cpp \
    main.cpp \
    mainwindow.cpp \
    classes/downloader.cpp \
    classes/fileinteractions.cpp \
    classes/settings.cpp \
    classes/updater.cpp

HEADERS += \
    classes/logger.h \
    classes/simple_json.h \
    downloader/basedownloader.h \
    form.h \
    mainwindow.h \
    classes/downloader.h \
    classes/fileinteractions.h \
    classes/settings.h \
    classes/updater.h

FORMS += \
    form.ui \
    mainwindow.ui

RC_ICONS = resources\arc.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
