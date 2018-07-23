QT       += core gui dbus xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += DEBUG_APP
TARGET = elokab-fa
TEMPLATE = app

DESTDIR = ../usr/bin
LIBS += $(SUBLIBS)  -L../usr/lib -L$$PWD/../usr/lib -lelokabmimicon
QMAKE_INCDIR += $$PWD/../library $$PWD/../library/include
CONFIG += qt \
          release
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
INCLUDEPATH +=build
    LIBS += -lmagic

INCLUDEPATH+=../common

HEADERS += \
    progressdlg.h \
    filesactions.h \
    confirmdlg.h

SOURCES += \
    progressdlg.cpp \
    main.cpp \
    filesactions.cpp \
    confirmdlg.cpp


FORMS += \
    progressdlg.ui \
    confirmdlg.ui

TRANSLATIONS    =../usr/share/elokab/translations/ar/elokab-fa.ts\
                 ../usr/share/elokab/translations/fr/elokab-fa.ts\
                 ../usr/share/elokab/translations/en/elokab-fa.ts\

CODECFORTR = UTF-8

# install

 target.path = /usr/bin

 INSTALLS +=                  target
linux-g++*: {
             # Provide relative path from application to elokab librarys
             # ex:app=usr/bin/elokab-applications  library=usr/lib/libelokabmimicon.so
             QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../lib
             }
