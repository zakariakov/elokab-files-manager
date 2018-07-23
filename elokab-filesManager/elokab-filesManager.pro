TEMPLATE    = subdirs
SUBDIRS	    = elokab-fm \
                elokab-fa


  LIBS += -lmagic

linux-g++*: {
             # Provide relative path from application to elokab librarys
             # ex:app=usr/bin/elokab-applications  library=usr/lib/libelokabmimicon.so
             QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../lib
             }
