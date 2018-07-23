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

#ifndef EICON_H
#define EICON_H
#include <QIcon>
#include <QHash>
//#include "eiconengin.h"
#define THEM_BACK   "Adwaita"
#define APP_EXE "application-x-executable"
#define THEM_PIXMAP   "/usr/share/pixmaps"
class EIcon
{
public:
    //!
    explicit  EIcon(){}
     static EIcon *instance();
    static  void seTemeName(const QString  &name);
    //!
    static  QIcon fromTheme(const QString  &iconName,const QString &fallback=QString());


private:
    static  bool hasPathIcon(const QString &name,const QString &path);
    static  bool searchIcon(const QString &name,const QString &path);

 static   QIcon iconFromThemePath(const QString &m_iconName, const QString &path);
 // static  void addIconFile(QIcon m_icon, const QString m_iconName, const QString &curPath);
    //!

    QHash<QString,QIcon>cachIcon;

};

#endif // EICON_H
