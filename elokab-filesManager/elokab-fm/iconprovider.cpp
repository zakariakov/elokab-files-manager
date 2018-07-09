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

#include "iconprovider.h"
#include <EMimIcon>
//#include <EIcon>
#include <EDir>
//#ifdef DEBUG_APP
#include <QDebug>
//#include <QDateTime>
//#endif

#include<QDir>

#include<QApplication>

#include<QPainter>
#include<QTextStream>

/*****************************************************************************************************
 *
 *****************************************************************************************************/
QIcon hidenIcon(QIcon icon)
{
    QPixmap pix(128,128);
    pix.fill(Qt::transparent);
 QPixmap pixIcon=(icon.pixmap(128));
 QPainter painter(&pix);
  painter.setOpacity(0.6);
  painter.drawPixmap(pix.rect(),pixIcon,pixIcon.rect());

     return QIcon((pix));
}

IconProvider::IconProvider()
{
//largeDirectory(false);
     mimeSufix = new QHash<QString,QString>;
     mimeLanguage = new QHash<QString,QString>;
     iconCach=new QHash<QString,QIcon>;
   //  qDebug( )<<"langagee========"<<mLc;

//
     loadMimeTypes();
}
void IconProvider::setlocale(const QString &lc)
  {
    mLc=lc;
    QString path=Edir::cachDir();
    QFile fileIcons(path+"/"+QIcon::themeName()+"_Icons.cache");

//    fileIcons.open(QIODevice::ReadOnly);
//    QDataStream out(&fileIcons);
//    out >> *iconCach;
//    fileIcons.close();

    QFile fileLang(Edir::cachDir()+"/mimLang_"+mLc+".cache");

    fileLang.open(QIODevice::ReadOnly);
    QDataStream outL(&fileLang);
    outL >> *mimeLanguage;
    fileLang.close();

  }
/**************************************************************************************
 *
 **************************************************************************************/

QIcon IconProvider::icon(IconType type) const
{
   //   qDebug()<<type;
     //Computer, Desktop, Trashcan, Network, Drive, Folder, File,Image,Music,Text
     switch (type) {

          case Computer:return EIcon::fromTheme("computer");
          case Desktop:  return EIcon::fromTheme("application-x-desktop");
          case Trashcan:  return EIcon::fromTheme("trashcan");
          case Network:  return EIcon::fromTheme("network");
          case Folder:  return EIcon::fromTheme("folder");
          case File:  return EIcon::fromTheme("file");
          case Image:  return EIcon::fromTheme("image-x-generic");
          case Music:  return EIcon::fromTheme("audio-x-generic");
          case Text:  return EIcon::fromTheme("text-x-generic");
          case Video:  return EIcon::fromTheme("video-x-generic");
          case Exec:  return EIcon::fromTheme("application-x-executable");


          default:
               break;
     }
     return QIcon::fromTheme("unknon");
}

QIcon  IconProvider::iconStandard(const QFileInfo &info) const
{

    if(info.isDir()) return icon(Folder);


    QString suf=info.suffix().toLower();
    if(suf=="desktop") return icon(Desktop);

       QString mim=EMimIcon::mimeTyppe(info);

    if(mim=="application/x-executable") return icon(Exec);


    if(mim=="application/x-desktop") return icon(Desktop);


    if(mim.startsWith("text")) return icon(Text);


    if(mim.startsWith("image")) return icon(Image);


    if(mim.startsWith("audio")) return icon(Music);


    if(mim.startsWith("video"))  return icon(Video);

   return QIcon::fromTheme("unknon");
   return EMimIcon::icon(info,mPreview);

}



QIcon IconProvider::icon(const QFileInfo &info)const
{

    if(!QFile::exists(info.absoluteFilePath()))
        return QIcon();

    if(info.fileName()=="." || info.fileName()==".." || info.fileName().isEmpty())
        return QIcon();

  // ---------------iconCach---------------------
//    if(iconCach->contains(info.absoluteFilePath()))
//       return   iconCach->value(info.absoluteFilePath());

  //  if(mLargeIcon)
  qApp->processEvents();


    QIcon retIcon;
    QFileInfo minfo=info;



    if(info.isSymLink()){

        if(QFile::exists(minfo.symLinkTarget())){
            minfo.setFile(minfo.symLinkTarget());
        }else {
            return  EMimIcon::iconSymLink( EIcon::fromTheme("application-octet-stream","unknon"));
        }

    }


    if(!QFile::exists(minfo.absoluteFilePath()))
        return QIcon();



    if(minfo.isDir())
    {
        retIcon=  EMimIcon::iconFolder(minfo.absoluteFilePath());

    }else{
        QString suf=minfo.suffix().toLower();
        if(suf=="desktop"){

            return  EMimIcon::iconDesktopFile(minfo.absoluteFilePath()).pixmap(128).scaled(128,128);

        }
        //---------------------------------------
        //  QString mim= EMimIcon::mimeTyppe(minfo);
        //---------------------------------------
        QString mim;

        if(!suf.isEmpty() && mimeSufix->contains(suf))
                mim=mimeSufix->value(suf);


        if(mim.isEmpty())
             mim= EMimIcon:: getMimeTypeByFile(minfo.absoluteFilePath());
        //---------------------------------------



        if(mPreview && mim.startsWith("image"))
        {
           // qApp->processEvents();
            QPixmap pix;
            pix.loadFromData(EMimIcon::iconThumbnails(minfo.absoluteFilePath()));
            QIcon icon=QIcon(pix);
            if(!icon.isNull())
                retIcon=icon;
            else
                retIcon=EIcon::fromTheme("image-x-generic");

        }else{
            if(iconCach->contains(info.absoluteFilePath()))
                retIcon=iconCach->value(mim);
            else{
             retIcon=   EMimIcon::iconByMimType(mim,minfo.absoluteFilePath());
               iconCach->insert(mim,retIcon);
            }

        }

    }

   // qDebug()<<"emimicon"<<minfo.fileName();

    if(!QFile::exists(info.absoluteFilePath()))
        return QIcon();

//    if(info.isSymLink())
//        retIcon=  EMimIcon::iconSymLink(retIcon);



//    if(info.isHidden())
//          retIcon=hidenIcon(retIcon) ;


     // ---------------iconCach---------------------
    //iconCach->insert(info.absoluteFilePath(),retIcon);

    return retIcon;

}

//void IconProvider::updateIcons()
//{
//iconCach->clear();
//qDebug()<<"IconProvider clear cach >>>>>>>>>>";
//}
//void IconProvider::updatePath(const QString &path)
//{

//        QDir dir(path);
//        QStringList list=dir.entryList();
//        foreach (QString s, list) {
//            QString file=path+"/"+s;
//            qDebug()<<"IconProvider remove"<<file;
//            iconCach->remove(file);
//        }


//}



/*
QIcon IconProvider::iconF(const QFileInfo &info)const
{

    QFileInfo minfo=info;
    if(info.isSymLink()){
        if(QFile::exists(minfo.symLinkTarget()))
            minfo.setFile(minfo.symLinkTarget());
    }

    QString suf=minfo.suffix().toLower();

    if(suf=="desktop")
        return  EMimIcon::iconDesktopFile(minfo.absoluteFilePath());

    QIcon retIcon;

     QString mim= EMimIcon::mimeTyppe(minfo);

     if(mPreview && mim.startsWith("image"))
        {
         QPixmap pix;
         pix.loadFromData(EMimIcon::iconThumbnails(minfo.absoluteFilePath()));
         QIcon icon=QIcon(pix);

           // QIcon icon=EMimIcon::iconThumbnails(minfo.absoluteFilePath());
            if(!icon.isNull())
                retIcon=icon;
            else
                retIcon=EIcon::fromTheme("image-x-generic");

        }else{
              retIcon=   EMimIcon::iconByMimType(mim,minfo.absoluteFilePath());
        }

    return retIcon;

}
*/

/**************************************************************************************
 *
 **************************************************************************************/
QString IconProvider::type(const QFileInfo &info) const
{

     //  return QFileIconProvider::type(info);

     QString mim=EMimIcon::mimeTyppe(info);
      //return mim;


     QString mimLang=mimeLanguage->value(mim);
     if(!mimLang.isEmpty())
          return mimLang;

     mimLang=EMimIcon::mimLang(mim,mLc);

     if(mimLang.isEmpty()){
          return QFileIconProvider::type(info);
     }

        //qDebug()<<">>Cach add mimLang=========="<<mim<<mimLang;
     mimeLanguage->insert(mim, mimLang);

     return mimLang;

}

/**************************************************************************************
 *
 **************************************************************************************/
void IconProvider::loadMimeTypes() const
{

     // Open file with mime/suffix associations
     QFile mimeInfo("/usr/share/mime/globs");
     mimeInfo.open(QIODevice::ReadOnly);
     QTextStream out(&mimeInfo);

     do {
          QStringList line = out.readLine().split(":");
          if (line.count() == 2)
          {
               QString suffix = line.at(1);
               suffix.remove("*.");
               QString mimeName = line.at(0);
               //mimeName.replace("/","-");
               mimeSufix->insert(suffix, mimeName);
             // qDebug()<<suffix<< mimeName;
          }
     } while (!out.atEnd());
     mimeInfo.close();


}
void IconProvider::saveCacheIcons()
{
    QString path=Edir::cachDir();
    QFile file(path+"/"+QIcon::themeName()+"_Icons.cache");
    QDataStream out(&file);

//            file.open(QIODevice::WriteOnly);
//            out.setDevice(&file);
//            out << *iconCach;
//            file.close();

            QFile fileLang(Edir::cachDir()+"/mimLang_"+mLc+".cache");

            QDataStream outL(&fileLang);
              fileLang.open(QIODevice::WriteOnly);
            outL <<  *mimeLanguage;
            fileLang.close();

}
void IconProvider::clearCache()
{
    saveCacheIcons();
    iconCach->clear();
  //  setlocale(mLc);
}

