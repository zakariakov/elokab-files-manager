
TEMPLATE    = subdirs
SUBDIRS	    =\
    library\
    elokab-fm \
     elokab-fa

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

# MKDIR = mkdir -p /etc/xdg/elokab
# MKDIR = mkdir -p /usr/share/elokab
# MKDIR = mkdir -p /usr/share/applications
# MKDIR = mkdir -p /usr/lib/elokab

#---config files---------
 elokabConfig.files =etc/xdg/*
 elokabConfig.path=/etc/xdg/

#---DATA files---------------
 elokabData.files=usr/share/elokab/*
 elokabData.path=/usr/share/elokab

 applicationsData.files=usr/share/applications/*
 applicationsData.path=/usr/share/applications/

#  elokablib.files=usr/lib/elokab/*
# elokablib.path=/usr/lib/elokab

 INSTALLS +=    elokabConfig \
                elokabData \
                applicationsData
#                elokablib

