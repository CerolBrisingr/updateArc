QT += core testlib widgets
QT += gui

CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

include(../../UpdateArc/common_settings_and_includepaths.pri)

SOURCES += tst_settings.cpp \
    window.cpp \
    $$PROJECT/helpers/sources/settings.cpp \
    $$PROJECT/classes/fileinteractions.cpp

HEADERS += \
    window.h \
    $$PROJECT/helpers/includes/settings.h \
    $$PROJECT/classes/fileinteractions.h
