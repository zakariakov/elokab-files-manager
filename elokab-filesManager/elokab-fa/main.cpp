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

#include "filesactions.h"

#include <ElokabApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QIcon>
#include <QDir>
#include <QSettings>

#include <QLocale>

#include <QDebug>
/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */

int main(int argc, char *argv[])
{

      ElokabApplication a(argc, argv);
           /// جلب ترجمة البرنامج من مجلد البرنامج
          a.setApplicationName("elokab-fa");
    QTranslator translator;
           translator.load(a.translationPath());
           a.installTranslator(&translator);
    QStringList args = a.arguments();

    //------------------------------------argument-------------------------------------------------

    qDebug()<<"FilesActions"<<args.count();
    if(args.count()==3)
    {
        FilesActions w;

        QString action = args.at(1);
        QString str = args.at(2);
        QStringList list=str.split(",");
        if (action == "-rm" )
        {

            qDebug()<<"remove"<<str;

            w.removeFiles(list);

        }
        else if (action == "-mt" )
        {

            qDebug()<<"move to trash"<<str;

            w.moveFilesToTrash(list);

        }
        return 0;
    }

    if(args.count()==4)
    {
        FilesActions w;

        QString action = args.at(1);
        QString str = args.at(2);
         QString dist = args.at(3);
        QStringList list=str.split(",");

        if (action == "-m")
        {
            qDebug()<<"move"<<str;
            w.moveFiles(list,dist);

        }
        else if (action == "-p"  )
        {
            qDebug()<<"past"<<str;
             w.pastFiles(list,dist);
        }



        return 0;
    }
    QString help=QObject::tr
            ("Elokab Files Actions Gui Version 0.1 .\n"
             " \n"
             "Usage: elokab-fa [OPTION]..File(s)..\n"
             "Usage: elokab-fa [OPTION]..SOURCE... DIRECTORY..\n"
             "OPTION:\n"
             " -rm          Remove File(s).\n"
             " -mt          Move File(s) to trash.\n"
             " -m           Rename SOURCE to DEST, or move SOURCE(s) to DIRECTORY.\n"
             " -p           Copy SOURCE to DEST, or multiple SOURCE(s) to DIRECTORY.\n"

           );
     qWarning() <<help; return 0;



   //  FilesActions w;
    // w.show();

     //return a.exec();
}
