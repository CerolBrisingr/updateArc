QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

include($$PWD\..\..\project\whereisit.pri)

SOURCES +=  tst_experience.cpp
