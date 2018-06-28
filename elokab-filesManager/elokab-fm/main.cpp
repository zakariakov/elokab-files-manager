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
#include <QTranslator>
#include <QLibraryInfo>
/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */

int main(int argc, char *argv[])
{

    ElokabApplication a(argc, argv);
    QStringList args = a.arguments();
    if(args.count()>1)
    {

        QUrl url(args.at(1));

        QString  pathUrl;

        //         if(url.isLocalFile())
        //             pathUrl=url.toLocalFile();
        //         else
        pathUrl= url.toString();
        if(pathUrl.startsWith("file://"))
            pathUrl.remove("file://");
        QFileInfo fi(pathUrl);
        if(!fi.isDir()){

            EMimIcon::launchApplication(pathUrl);
            return 0;
        }
    }

    /// جلب ترجمة البرنامج من مجلد البرنامج

    a.setApplicationName("elokab-fm");
    a.setApplicationVersion("0.1");
    a.setOrganizationName("elokab");
    /// مسار مجلد البرنامج
    QString translatPath= Edir::dataDir()+"/translations";
    /// جلب اعدادات اللغة
    QSettings globalSetting("elokab","elokabsettings");
    globalSetting.beginGroup("Language");
    QString locale=globalSetting.value("Name","Default").toString();
    globalSetting.endGroup();
    /// اذاكانت اللغة الافتراضية
    if(locale=="Default")
        locale = QLocale::system().name().section("_",0,0);
    /// اللغة الحالية لجميع البرنامج
    QLocale::setDefault(QLocale(locale));
    /// جلب ترجمات كيوتي
    QString translatorFileName = QLatin1String("qt_");
    translatorFileName += locale;
    QTranslator *translatorsys = new QTranslator;
    if (translatorsys->load(translatorFileName, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        QApplication::installTranslator(translatorsys);
    /// جلب ترجمة البرنامج من مجلد البرنامج
    QTranslator translator;
    translator.load(QString(translatPath+"/"+locale+"/"+a.applicationName()));
    a.installTranslator(&translator);
    QLocale lx=QLocale(locale);
  a.setLayoutDirection(lx.textDirection());
    //  a.setLayoutDirection(Qt::LeftToRight);

    a.setWindowIcon(EIcon::fromTheme("system-file-manager","folder"));

    MainWindow w;
    w.show();

    return a.exec();
}
