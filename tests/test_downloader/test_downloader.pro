QT += testlib network
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

include(../../UpdateArc/common_settings_and_includepaths.pri)

SOURCES +=  tst_downloader.cpp \
        $$PROJECT/helpers/sources/downloader.cpp \
        $$PROJECT/helpers/sources/logger.cpp

HEADERS += \
        $$PROJECT/helpers/includes/downloader.h \
        $$PROJECT/helpers/includes/logger.h
