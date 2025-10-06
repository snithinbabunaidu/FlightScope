QT += testlib widgets

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

# Include paths
INCLUDEPATH += $$PWD/../src
INCLUDEPATH += $$PWD/../third-party

# Source files
SOURCES += \
    test_main.cpp \
    ../src/ui/mainwindow.cpp

# Header files
HEADERS += \
    ../src/ui/mainwindow.h

# Forms
FORMS += \
    ../src/ui/mainwindow.ui