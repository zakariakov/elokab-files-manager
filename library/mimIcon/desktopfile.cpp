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

#include "desktopfile.h"
#include <QFileInfo>
#include <QFile>
#include <QDebug>
#include <QSettings>
#include <QTextCodec>

//___________________________________________________________________
DesktopFile::DesktopFile(const QString &fileName,const QString &group ,const QString &lC)
{

    if (!QFile::exists(fileName)) {
        return ;
    }
    bool  hasGroup=false;
    QString mgroup=group;
    if(mgroup.isEmpty()) mgroup=="General";

    QFile files(fileName);
    if(!files.open( QFile::ReadOnly))
        return;

    QTextStream textStream(&files);
    textStream.setCodec(QTextCodec::codecForName("UTF-8"));
    QString line ;//premier line;

    while   (!textStream.atEnd()) {

        line = textStream.readLine().trimmed();
        if(line.startsWith("#"))
            continue;


        if (line.startsWith('[') && line.endsWith(']'))
        {
            QString  section = line.mid(1, line.length()-2);
            if (section.trimmed() == mgroup)
                hasGroup = true;
            else
                hasGroup = false;

            continue;
        }

        if(!hasGroup)
            continue;

        if(!line.contains("="))
            continue;

        QString key=line.section("=",0,0).trimmed();
        QString value=line.section("=",1,1).trimmed();

        if (key.isEmpty())
            continue;

        if(key.contains("[")&&key.endsWith("]")){
            if(key.contains("["+lC+"]"))
                insert(key,value);
        }else{
            insert(key,value);
        }


    }



    files.close();

}



/*
QHash<QString ,QVariant>  DesktopFile::entry(const QString &fileName ,const QString &lC)
{


QHash<QString ,QVariant>hash;

    if (!QFile::exists(fileName)) {
        return hash;
    }

   QFile files(fileName);
       if(!files.open( QFile::ReadOnly))
           return hash;

//    QString nameLc;
//    QString commentLc;
//    QString genericLc;

        QTextStream textStream(&files);
        QString line = textStream.readLine();//premier line;

        while   (!line.isNull()) {

            line = textStream.readLine().trimmed();
            if(line.startsWith("#"))
                continue;
            if(!line.contains("="))
                continue;

            QString key=line.section("=",0,0);
            QString value=line.section("=",1,1);

            if (key.isEmpty())
                continue;

            if(key.contains("[")&&key.endsWith("]")){
                if(key.contains("["+lC+"]"))
                    hash[key]=value;
            }else{
                     hash[key]=value;
            }


        }

qDebug()<<hash;

files.close();
   return hash;

}

//_______
*/
