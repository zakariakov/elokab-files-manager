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

#include "elokabapplication.h"

#include <QSettings>
#include <QLocale>
#include "eicon.h"
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>
#include <QDir>
#include <QIcon>
ElokabApplication::ElokabApplication(int &argc, char **argv):
    QApplication(argc, argv)
{
    setApplicationVersion("0.3");
    setOrganizationName("elokab");
    msetting=new ElokabSettings;
    connect(msetting,SIGNAL(iconThemeChanged()),this,SLOT(setIconsThemeName()));
}

 void  ElokabApplication::setIconsThemeName()
 {

     EIcon::instance()->seTemeName(msetting->loadIconThems());
 }

 const QString ElokabApplication::translationPath()
{
     QDir appDir(applicationDirPath());
     appDir.cdUp();
    QString dirPath=  appDir.absolutePath()+"/share/"+organizationName();

    /// جلب اعدادات اللغة
    QSettings globalSetting(organizationName(),"elokabsettings");
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
     qDebug()<<dirPath+"/translations/"+locale+"/"+applicationName();


         QLocale lx=QLocale(locale);
         setLayoutDirection(lx.textDirection());

          qDebug()<<locale<<layoutDirection();
    return dirPath+"/translations/"+locale+"/"+applicationName();

}
