/***************************************************************************
 *   elokab Copyright (C) 2014 AbouZakaria <yahiaui@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef EDIR_H
#define EDIR_H

#include <QObject>
#include<QDir>
class Edir
{

public:
    explicit   Edir(){}
    //!/usr/bin/prog
    static QString progPath(const QString &prog);
    //!$XDG_DATA_DIRS  default /usr/share/elokab
    static QString dataDir();
    static QString dataAppDir();
    //!$XDG_DATA_HOME default $HOME/.local/share
    static QString dataHomeDir();//$HOME/.local/share
    //!$XDG_DATA_HOME default $HOME/.local/share/elokab
    static QString dataHomeAppDir();

    //!XDG_CONFIG_HOME default  $HOME/.config/elokab
    static QString configDir();
    //!XDG_CONFIG_HOME default  $HOME/.config
    static QString configHomeDir();

    //!default /usr/share/applications $HOME/.local/share/applications
    static QStringList applicationsDirs();
    //!$HOME/.local/share/applications
    static QString applicationsHomeDir();

     static QString libDir();

      static QString thumbnaileCachDir();
    //!$XDG_CACHE_HOME default $HOME/.cache/elokab/+appName
    static QString cachDir();
    //!
    static QString desktopDir();
    //!
    static QString documentsDir();
    //!
    static QString musicDir();
    //!
    static QString moviesDir();
    //!
    static QString picturesDir();

};

#endif // EDIR_H
