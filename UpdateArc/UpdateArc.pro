QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 6): QT += widgets

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(common_settings_and_includepaths.pri)
include(whereisit.pri)

RC_ICONS = resources\arc.ico

VERSION = 7.0.0
QMAKE_TARGET_DESCRIPTION = "Updater for useful Guild Wars 2 add-ons"
QMAKE_TARGET_COPYRIGHT = "MIT"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
