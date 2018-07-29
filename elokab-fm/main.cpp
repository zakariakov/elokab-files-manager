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

#include "mainwindow.h"
#include <ElokabApplication>
#include "defines.h"
#include "messages.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QUrl>
/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */

int main(int argc, char *argv[])
{

    ElokabApplication a(argc, argv);
    a.setApplicationName(D_APPNAME);
    a.setApplicationVersion(D_APPVERSION);
    a.setOrganizationName(D_APPORGNAME);

    QStringList args = a.arguments();

    QString  pathUrl;

    if(args.count()>1)
    {
        for (int i = 1; i < args.count(); ++i) {
           //-------------------------------
            QUrl url(args.at(i));
            if(url.toString()=="--debug"){
                Messages::instance()->setDebug(true);
            }else{
                 pathUrl= url.toString();
                if(pathUrl.startsWith("file://"))
                    pathUrl.remove("file://");
                QFileInfo fi(pathUrl);
                if(!fi.isDir()){
                    EMimIcon::launchApplication(pathUrl);
                    return 0;
                }
            }
            //-------------------------------
        }


    }


    /// جلب ترجمة البرنامج من مجلد البرنامج



 QTranslator translator;
        translator.load(a.translationPath());
        a.installTranslator(&translator);
 qApp->setLayoutDirection(a.layoutDirection());

    a.setWindowIcon(EIcon::fromTheme("system-file-manager","folder"));

    MainWindow w(pathUrl);
    w.show();

    return a.exec();
}
