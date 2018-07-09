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

#include "menugen.h"
#include "desktopfile.h"
//#include "emimicon.h"
#include "edir.h"

#include <QFileInfo>
#include <QFile>
#include <QDebug>
#include <QSettings>
#include <QTextCodec>
#include <QStringList>
#include <QApplication>
/****************************************************************************
 *
 ****************************************************************************/
MenuGen::MenuGen(const QString &locale, QObject *parent) :
     QObject(parent), m_locale(locale)
{
//     m_sWatcher=new QFileSystemWatcher;
//     mtimer=new QTimer;
//     mtimer->setInterval(1000);
//     connect(mtimer, SIGNAL(timeout()), this, SLOT(genirateAppFile()));
//     connect(m_sWatcher, SIGNAL(directoryChanged(const QString&)), mtimer, SLOT(start()));

     loadSettings();
     saveSettings();
     checkAppsDesktopFile();
}
void MenuGen::updatApp()
{
//     mtimer->stop();
//     mtimer->start(1000);
}

/****************************************************************************
 *
 ****************************************************************************/
bool MenuGen::checkAppsDesktopFile()
{

     QFile fileApp( Edir::configDir()+"/applictions");

//     QString dirUsers="/usr/share/applications";
//     QString dirLocal=  QDir::homePath()+"/.local/share/applications";
    int count=0;
     foreach (QString s, Edir::applicationsDirs()) {
        count+= countDesktopDir(s);
     }
//     int count1=countDesktopDir(dirLocal);             /*ex:  /home/usrer/.local/share/applications  */
//     int count2=countDesktopDir(dirUsers);             /*     "/usr/share/applications"                         */


//     int count=count1+count2;                                    /*    عدد التطبيقات في المسارين                                  */

     if (count!=m_appCount||
               m_appCount<1||
               !fileApp.exists()||
               QFileInfo(fileApp).size()==0){         /*  اذا كان العدد مختلف او القائمة غير موجودة                  */
          genirateAppFile();
          m_appCount=count;
          saveSettings();
          //EMimIcon::updateMimeAssociatedApplication();
          return true;
     }
     return false;
}

/****************************************************************************
 *                  -حمل عدد التطبيقات
 ****************************************************************************/
void MenuGen::loadSettings()
{
     QSettings setting(QApplication::organizationName(),"elokabsettings");
     setting.beginGroup("MenuAppCount");
     m_appCount=setting.value("Num",0).toInt();
     setting.endGroup();
}

/****************************************************************************
 *                  -حفظ عدد التطبيقات
 ****************************************************************************/
void MenuGen::saveSettings()
{
     QSettings setting(QApplication::organizationName(),"elokabsettings");
     setting.beginGroup("MenuAppCount");
     setting.setValue("Num",m_appCount);
     setting.endGroup();
     //     emit appDesktopChanged();
}

/****************************************************************************
 *                  عد المجلدات
 ****************************************************************************/
int MenuGen::countDesktopDir(QString dir)
{
//    if (!m_sWatcher->directories().contains(dir))
//         m_sWatcher->addPath(dir);

     int count=0;
     QDir curdir(dir);
    // qDebug()<<dir<<curdir.count();
     count+=curdir.count();

     QDirIterator it(dir,QDir::AllDirs |
                     QDir::NoDotAndDotDot |
                     QDir::NoSymLinks, QDirIterator::Subdirectories);

     while(it.hasNext())
     {
//         if(it.fileInfo().isDir()){
//             if (!m_sWatcher->directories().contains(dir))
//                  m_sWatcher->addPath(it.filePath());

//         }
          it.next();
          QString f=it.filePath();
          QDir curdir(f);
        //  qDebug()<<f<<curdir.count();
          count+=curdir.count();
     }
     return count;
}
//! [1-1] ------------------------------------------------  ---------------------------------------------- */

/****************************************************************************
 *                  عد الملفات
 ****************************************************************************/
//int MenuGen::countDesktopFile(QString   dir)
//{
//     if (!m_sWatcher->directories().contains(dir))
//          m_sWatcher->addPath(dir);
//     QString t(50-dir.length(),'-');
//     qDebug()<<"MenuGen >> m_sWatcher.addPath:"+t<<dir;
//     int count=0;
//     QString subfile;
//     QDir dirS(dir);
//     foreach ( subfile, dirS.entryList(QDir::AllEntries|  QDir::NoDotAndDotDot )){
//          QFileInfo fi(dir+"/"+subfile);
//          if(!fi.isDir()&&fi.completeSuffix()=="desktop"){
//               count++;
//          }
//     }
//     return count;
//}

/****************************************************************************
 *                  -انشاء ملف بالتطبيقات الموجودة
 ****************************************************************************/
void MenuGen::genirateAppFile()
{
   //  mtimer->stop();
     qDebug()<<"(MenuGen)<<====BEGIN====genirateAppFile======================================================";
//     m_sWatcher->blockSignals(true);
     //-------------------------------------------------------------
     QFile fileApp( Edir::configDir()+"/applictions");
     //   QFile fileMim( QDir::homePath()+"/.config/elokab/mimeApp");
     if (!fileApp.open(QFile::WriteOnly)){
          return ;
     }
     //   fileMim.open(QFile::WriteOnly);

     //--------------------------------------------------
     QTextStream out(&fileApp);
     // QTextStream outMim(&fileMim);
     out.setCodec(QTextCodec::codecForName("UTF-8"));
     //   outMim.setCodec(QTextCodec::codecForName("UTF-8"));

     //----------------------------------------------------


     //--------------------------------------------------
     QSettings setting(QApplication::organizationName(),"menuApps");
     setting.beginGroup("Menu");
     m_desktopList=setting.value("Exclud").toStringList();
     setting.endGroup();

     //----------------------------------------------------
//     QString dirUsers="/usr/share/applications";
//     QString dirLocal=  QDir::homePath()+"/.local/share/applications";
//     chargeDesktopDir( dirLocal);
//     chargeDesktopDir( dirUsers);
     foreach (QString s, Edir::applicationsDirs()) {
          chargeDesktopDir(s);
     }
     //-----------------------------------
     foreach (QString s, wordList) {
          out << s+"\n";
     }
     //     foreach (QString s, mimList) {
     //          outMim << s+"\n";
     //     }

     //------------------------------------------
     wordList.clear();
     // mimList.clear();
     m_desktopList.clear();
     fileApp.close();
     //fileMim.close();
   //  m_sWatcher->blockSignals(false);

     emit appDesktopChanged();
     qDebug()<<"(MenuGen)<<=====END=====genirateAppFile======================================================";
}

/****************************************************************************
 *                  -تحميل المجادات المحددة
 ****************************************************************************/
void MenuGen::chargeDesktopDir(QString dir)
{

     chargeDesktopFile(dir);


     QString subdir;
     QDir dirS(dir);
     foreach ( subdir, dirS.entryList(QDir::AllDirs| QDir::NoSymLinks| QDir::NoDotAndDotDot )){
          chargeDesktopFile(dir+"/"+subdir);

     }
     //return true;
}

/****************************************************************************
 *                  --تحميل الملفات المحددة
 ****************************************************************************/
void MenuGen::chargeDesktopFile(QString   dir)
{
     QString subfile;
     QDir dirS(dir);
//     if (!m_sWatcher->directories().contains(dir))
//          m_sWatcher->addPath(dir);

     foreach ( subfile, dirS.entryList(QDir::AllEntries|  QDir::NoDotAndDotDot )){
          QFileInfo fi(dir+"/"+subfile);
          if(!fi.isDir()&&fi.suffix()=="desktop"){
               if(!m_desktopList.contains(subfile)){
                    QString data=openDesktopEntry(dir+"/"+subfile);
                    if(!data.isEmpty()){
                         wordList.append(data);
                         m_desktopList.append(subfile);
                    }
               }
          }
     }
}

/****************************************************************************
 *                  -فتح الملفات لاستحراج المعلومات
 ****************************************************************************/


QString MenuGen::openDesktopEntry(QString file)
{

    DesktopFile xdg(file,"Desktop Entry",m_locale);
    QString oldname=xdg.value("Name").toString();
    QString   name=xdg.value("Name["+m_locale+"]",oldname).toString();

    QString   exec=xdg.value("Exec").toString();
    QString   icon=xdg.value("Icon").toString();
    QString   type=xdg.value("Type").toString();
    bool NoDisplay=xdg.value("NoDisplay",false).toBool();

    if(type!="Application")return "";
    if(name.isEmpty())return "";
    if(exec.isEmpty())return "";
    if(NoDisplay==true)return "";
    exec.remove("-caption");

  //  return name+"|"+exec.section("%",0,0)+"|"+category+"|"+icon+"|"+file+"|"+GenericName;
 return file +"|"+exec.section("%",0,0)+"|"+icon;
}
