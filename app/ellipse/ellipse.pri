INCLUDEPATH += $$PWD

INCLUDEPATH += $$PWD/qwt-6.1

LIBS += -L$$PWD/lib/ -lqwt

HEADERS += \
    $$PWD/ellipse.h \
    $$PWD/ellipsoid_fitting.h \
    $$PWD/ellipsetest.h

SOURCES += \
    $$PWD/ellipse.cpp \
    $$PWD/ellipsoid_fitting.cpp \
    $$PWD/ellipsetest.cpp
