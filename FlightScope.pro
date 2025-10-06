QT += core gui widgets qml quick location positioning network websockets charts

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please do refer to the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Include paths
INCLUDEPATH += $$PWD/third-party
INCLUDEPATH += $$PWD/src

# Source files
SOURCES += \
    src/main.cpp \
    src/ui/mainwindow.cpp \
    src/ui/connectdialog.cpp \
    src/comm/udplink.cpp \
    src/comm/linkmanager.cpp \
    src/comm/mavlinkrouter.cpp \
    src/models/vehiclemodel.cpp \
    src/models/healthmodel.cpp \
    src/models/waypoint.cpp \
    src/models/missionmodel.cpp

# Header files
HEADERS += \
    src/ui/mainwindow.h \
    src/ui/connectdialog.h \
    src/comm/linkinterface.h \
    src/comm/udplink.h \
    src/comm/linkmanager.h \
    src/comm/mavlinkrouter.h \
    src/models/vehiclemodel.h \
    src/models/healthmodel.h \
    src/models/waypoint.h \
    src/models/missionmodel.h

# Forms
FORMS += \
    src/ui/mainwindow.ui

# Resources
RESOURCES += \
    resources/resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target