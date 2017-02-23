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

#ifndef DESKTOPFILE_H
#define DESKTOPFILE_H

#include <QHash>
#include <QVariant>

class DesktopFile : public QHash<QString, QVariant>
{
     public:

          /**
     * @brief DesktopFile
     * @param fileName مسار الملف
     * @param lC اللغة
          */
      //    explicit DesktopFile(const QString &fileName ,const QString &lC="", const QString &group= "Desktop Entry");
    explicit DesktopFile(const QString &fileName,const QString &group,const QString &lC="");
   // static QHash<QString ,QVariant> entry(const QString &fileName ,const QString &lC="");


};

#endif // DESKTOPFILE_H
