#-------------------------------------------------
#
# Project created by QtCreator 2021-01-18T11:19:00
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OperationGuide
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(mainwindow/mainwindow.pri)
include(common/common.pri)

LIBS       += -L/usr/local/lib -lVLCQtCore -lVLCQtWidgets

TRANSLATIONS += \
    ../languages/operation_en_US.ts \
    ../languages/operation_zh_CN.ts



INCLUDEPATH += /usr/local/include/

SOURCES += \
        main.cpp

RESOURCES += \
    resource/resource.qrc

QMAKE_POST_LINK += \
#    cp -a $$PWD/../languages/*.qm  /home/oneai/.config/robogo/
    cp -a $$PWD/../languages/*.qm  $$PWD/../deploy/deb/home/oneai/.config/robogo/

CONFIG(release){
    DESTDIR = $$PWD/../deploy/deb/usr/bin/
}
