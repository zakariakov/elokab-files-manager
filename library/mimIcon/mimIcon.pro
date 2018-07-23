QT       += core gui dbus
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets gui core
CONFIG      +=  plugin release
TARGET      = $$qtLibraryTarget(elokabmimicon)
DESTDIR = ../../usr/lib

TEMPLATE    = lib

LIBS        += -L. 
LIBS += -lmagic

HEADERS  += \
    menugen.h \
    desktopfile.h \
    edir.h \
    eicon.h \
    emimicon.h \
    elokabsettings.h \
    elokabapplication.h
SOURCES += \
    menugen.cpp \
    desktopfile.cpp \
    edir.cpp \
    eicon.cpp \
    emimicon.cpp \
    elokabsettings.cpp \
    elokabapplication.cpp

#target.path = $$[QT_INSTALL_PLUGINS]/designer
target.path = /usr/lib
INSTALLS    += target
