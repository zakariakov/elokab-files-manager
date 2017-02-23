
TEMPLATE    = subdirs
SUBDIRS	    =\
    library\
    elokab-filesManager

CONFIG += qt \
          release


INCLUDEPATH +=common\
              usr\
              etc
LIBS += $(SUBLIBS)  -L/usr/lib -lQtDBus -lQtXml -lQtGui -lQtCore -lpthread -lX11
linux-g++*: {
             # Provide relative path from application to elokab librarys
             # ex:app=usr/bin/elokab-applications  library=usr/lib/libelokabmimicon.so
             QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../lib
             }

#---------------------------------------------
#                   INSTALL
#---------------------------------------------

 MKDIR = mkdir -p /etc/xdg/autostart
 MKDIR = mkdir -p /etc/xdg/menus
 MKDIR = mkdir -p /usr/share/xsessions
 MKDIR = mkdir -p /usr/share/desktop-directories
 MKDIR = mkdir -p /usr/share/icons
 MKDIR = mkdir -p /usr/share/icons/hicolor
 MKDIR = mkdir -p /usr/share/icons/Elokab
 MKDIR = mkdir -p /usr/share/themes
 MKDIR = mkdir -p /usr/share/applications
 MKDIR = mkdir -p /usr/share/elokab

#---config files---------
 autostartConfig.files =etc/xdg/autostart/*
 autostartConfig.path=/etc/xdg/autostart/

 elokabConfig.files =etc/xdg/elokab
 elokabConfig.path=/etc/xdg/

 menusConfig.files =etc/xdg/menus/*
 menusConfig.path=/etc/xdg/menus/

#---DATA files---------------
 sessionData.files=usr/share/xsessions/*
 sessionData.path=/usr/share/xsessions/


 dsDirectoriesData.files=usr/share/desktop-directories/*
 dsDirectoriesData.path=/usr/share/desktop-directories/

 iconsData.files=usr/share/icons/hicolor/*
 iconsData.path=/usr/share/icons/hicolor/

 iconsElokab.files=usr/share/icons/Elokab/*
 iconsElokab.path=/usr/share/icons/Elokab/

 gtkTheme.files=usr/share/themes/*
 gtkTheme.path=/usr/share/themes/

 applicationsData.files=usr/share/applications/*
 applicationsData.path=/usr/share/applications/

 elokabData.files=usr/share/elokab/*
 elokabData.path=/usr/share/elokab

 INSTALLS +=    autostartConfig \
                elokabConfig \
                menusConfig \
                sessionData \
                dsDirectoriesData \
                iconsElokab\
                iconsData \
                gtkTheme\
                applicationsData \
                elokabData
