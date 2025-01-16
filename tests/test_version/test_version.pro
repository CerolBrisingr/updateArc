QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

include(../../UpdateArc/common_settings_and_includepaths.pri)

SOURCES +=  tst_version.cpp \
    $$PROJECT/helpers/sources/logger.cpp \
    $$PROJECT/helpers/sources/version.cpp

HEADERS += \
    $$PROJECT/helpers/includes/version.h \
    $$PROJECT/helpers/includes/logger.h
