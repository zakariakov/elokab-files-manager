#-------------------------------------------------
#
# Project created by QtCreator 2014-09-10T12:36:51
#
#-------------------------------------------------


QT       += core gui dbus xml concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets concurrent

DEFINES -= DEBUG_APP
TARGET = elokab-fm
TEMPLATE = app

DESTDIR = ../../usr/bin

CONFIG += qt \
          release
CONFIG += thread
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build


LIBS += $(SUBLIBS)  -L../usr/lib -L$$PWD/../../usr/lib -lelokabmimicon
QMAKE_INCDIR += $$PWD/../../library $$PWD/../../library/include

INCLUDEPATH +=build
INCLUDEPATH+=../../common

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    tab.cpp \
    pathwidget.cpp \
    pagewidget.cpp \
    actions.cpp \
    iconprovider.cpp \
    placetree.cpp \
    filterbar.cpp \
#    mycomputer.cpp \
    trashview.cpp \
    mylistview.cpp \
    mytreeview.cpp \
    fileinformation.cpp \
    dialogactions.cpp \
    searchview.cpp \
    ../../common/filesutils/propertiesdlg.cpp \
    ../../common/filesutils/openwithdlg.cpp \
    ../../common/about/widgetabout.cpp \
    ../../common/filesutils/symlinkdlg.cpp \
    ../../common/filesutils/dialogxdesktop.cpp \
#  itemdelegate.cpp
 #    qitemdelegate.cpp
    udiskdevice.cpp \
    itemdelegate.cpp \
    messages.cpp

HEADERS  += \
    mainwindow.h \
    tab.h \
    pathwidget.h \
    pagewidget.h \
    actions.h \
    iconprovider.h \
    placetree.h \
    filterbar.h \
#    mycomputer.h \
    trashview.h \
    mylistview.h \
    mytreeview.h \
    settings.h \
    fileinformation.h \
    dialogactions.h \
    searchview.h \
    messages.h \
    ../../common/filesutils/propertiesdlg.h \
    ../../common/filesutils/openwithdlg.h \
    ../../common/about/widgetabout.h \
    ../../common/filesutils/symlinkdlg.h \
    ../../common/filesutils/dialogxdesktop.h \
#  itemdelegate.h
 #     qitemdelegate.h
    udiskdevice.h \
    itemdelegate.h \
    defines.h

FORMS    += mainwindow.ui \
    pathwidget.ui \
    fileinformation.ui \
    progressdlg.ui \
    dialogactions.ui \
    searchview.ui \
    ../../common/filesutils/propertiesdlg.ui \
    ../../common/filesutils/openwithdlg.ui \
    ../../common/about/widgetabout.ui \
    ../../common/filesutils/symlinkdlg.ui \
    ../../common/filesutils/dialogxdesktop.ui

RESOURCES += \
    icons.qrc

TRANSLATIONS    =../../usr/share/elokab/translations/ar/elokab-fm.ts\
                 ../../usr/share/elokab/translations/fr/elokab-fm.ts\
                 ../../usr/share/elokab/translations/en/elokab-fm.ts\

CODECFORTR = UTF-8
# install



 target.path = /usr/bin

 INSTALLS +=                  target
linux-g++*: {
             # Provide relative path from application to elokab librarys
             # ex:app=usr/bin/elokab-applications  library=usr/lib/libelokabmimicon.so
             QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../lib
             }
