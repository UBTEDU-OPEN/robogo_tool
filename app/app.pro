#-------------------------------------------------
#
# Project created by QtCreator 2021-01-18T11:19:00
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RoboGoTool
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

include(advanced_setting/advanced_setting.pri)
include(basic_setting/basic_setting.pri)
include(homepage/homepage.pri)
include(mainwindow/mainwindow.pri)
include(common/common.pri)
include(ellipse/ellipse.pri)
include(ota/ota.pri)
include(grpc/grpc.pri)

LIBS       += -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lopencv_calib3d -L$$_PRO_FILE_PWD_/libs -lPerspectiveCalibration

TRANSLATIONS += \
    ../languages/app_zh_CN.ts \
    ../languages/app_en_US.ts

CONFIG += C++11

INCLUDEPATH += /usr/include/opencv4/

INCLUDEPATH += /usr/local/include/
INCLUDEPATH += libs/include/

LIBS += -lgrpc++ -lprotobuf -Llibs -lsetting

SOURCES += \
        main.cpp

RESOURCES += \
    resource/resource.qrc

HEADERS += \
    libs/include/PerspectiveCalibration.h

DISTFILES += \
    libs/libPerspectiveCalibration.so

QMAKE_POST_LINK += \
    cp -a $$PWD/../languages/*.qm  /home/oneai/.config/robogo/ && \
    cp -a $$PWD/../languages/*.qm  $$PWD/../deploy/deb/home/oneai/.config/robogo/

CONFIG(release){
    DESTDIR = $$PWD/../deploy/deb/usr/bin/
}
