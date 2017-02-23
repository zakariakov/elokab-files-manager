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
#include "emimicon.h"

#include <magic.h>
#include <sys/vfs.h>
#include <sys/stat.h>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QTextCodec>
#include <QProcess>
#include "desktopfile.h"
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QBuffer>
#include <QPushButton>
#include <QDir>
#include <QDirIterator>
#include <QPainter>
#include <QXmlStreamReader>
#include <QDateTime>
#include <QImageWriter>
#define THEM_BACK   "Elokab"
//______________________________________________________________________________________
QIcon EMimIcon::icon(const QFileInfo &info, bool previw)
{

    QFileInfo minfo=info;
    if(info.isSymLink()){
        if(QFile::exists(minfo.symLinkTarget())){
            minfo.setFile(minfo.symLinkTarget());
        }else {
            return iconSymLink( EIcon::fromTheme("application-octet-stream","unknon"));
        }
    }

    QIcon retIcon;
    if(minfo.isDir())
    {
        retIcon= iconFolder(minfo.absoluteFilePath());

    }else{

        if(minfo.suffix().toLower()=="desktop")
            return iconDesktopFile(minfo.absoluteFilePath()).pixmap(128);

        QString mim=mimeTyppe(minfo);

        if(previw && mim.startsWith("image"))
        {
            QPixmap pix;
            pix.loadFromData(iconThambnail(minfo.absoluteFilePath()));
            QIcon icon=QIcon(pix);
            if(!icon.isNull())
                retIcon=icon;

        }else
            retIcon=  iconByMimType(mim,minfo.absoluteFilePath());

    }

    if(info.isSymLink())
        retIcon= iconSymLink(retIcon);

    return retIcon;

}
QHash<QString,QIcon> EMimIcon::iconhash(const QFileInfo &info, bool previw)
{
    QHash<QString,QIcon>hash;
    QString key="unknow";
    QFileInfo minfo=info;
    if(info.isSymLink()){
        if(QFile::exists(minfo.symLinkTarget())){
            minfo.setFile(minfo.symLinkTarget());
        }else {
             hash["unknow"]=iconSymLink( EIcon::fromTheme("application-octet-stream","unknon"));
            return hash;
        }
    }

    QIcon retIcon;
    if(minfo.isDir())
    {
        retIcon= iconFolder(minfo.absoluteFilePath());

    }else{

        if(minfo.suffix().toLower()=="desktop"){
            hash["unknow"]=iconDesktopFile(minfo.absoluteFilePath()).pixmap(128);

            return hash;
        }
        QString mim=mimeTyppe(minfo);

        if(previw && mim.startsWith("image"))
        {
           QPixmap pix;
           pix.loadFromData(iconThambnail(minfo.absoluteFilePath()));
            QIcon icon=QIcon(pix);

           // QIcon icon=iconThambnail(minfo.absoluteFilePath());
            if(!icon.isNull())
                retIcon=icon;

        }else{
            retIcon=  iconByMimType(mim,minfo.absoluteFilePath());
            key=mim;
         }
    }

    if(info.isSymLink()){
        hash["unknow"]= iconSymLink(retIcon);
                return hash;
    }
    hash[key]= retIcon;
    return hash;

}
//______________________________________________________________________________________
QString EMimIcon::mimeTyppe(const QFileInfo &info)
{
    if (info.isDir())
        return "inode/directory"; /*getMimeTypeByFile(info.absoluteFilePath());*/
    QString mim;
    QString suf=info.suffix().toLower();
    if(!suf.isEmpty())
        mim= getMimeTypeBySufix(suf);

    if(!mim.isEmpty())return mim;

    return getMimeTypeByFile(info.absoluteFilePath());

    /*
     if(!suf.isEmpty()){

          return getMimeTypeByFile(info.absoluteFilePath());
     }else{

          QString mim= getMimeTypeBySufix(suf);
          if(mim.isEmpty())
               mim=getMimeTypeByFile(info.absoluteFilePath());

          return mim;
     }
     return "unknown";
     */
}

//______________________________________________________________________________________
QIcon EMimIcon::iconFolder(const QString &f)
{
     QDir dir(f);
     if(dir.path()==QDir::rootPath()){
         if(QIcon::hasThemeIcon("folder-red")){
             return EIcon::fromTheme("folder-red", ("folder"));
         }
         return  iconColorized(EIcon::fromTheme("folder"),QColor(255,0,0));

     }

     if(dir.path()==QDir::homePath()){

          return EIcon::fromTheme("user-home","folder");
     }
     //اذا كان المجلد مخصصا
     if(QFile::exists(f+"/.directory")){


          QHash<QString ,QString> hash=desktopFile(f+"/.directory");


          QString folderColor=hash.value("FolderColor");
          if(!folderColor.isEmpty()){
              QColor color(folderColor);
               if(color.isValid())
                  return iconColorized(EIcon::fromTheme("folder"),color);
          }
           QString iconXdg=hash.value("Icon");
          if(!iconXdg.isEmpty()){
              if(QFile::exists(iconXdg.replace("./",f+"/"))){
                  return QIcon(iconXdg);
              }

              return EIcon::fromTheme(iconXdg,"folder");
          }

     }

     //الايقونة الافتراضية للمجلد
     return EIcon::fromTheme("folder");
}

//______________________________________________________________________________________
QIcon EMimIcon::iconSymLink(QIcon icon)
{
     QPixmap pixSym=QIcon(EIcon::fromTheme("emblem-symbolic-link")).pixmap(48);
     QPixmap pix=icon.pixmap(QSize(128,128));
     QPainter p;
     p.begin(&pix);
     p.drawPixmap(QRect(80,80,48,48),pixSym,pixSym.rect());
     p.end();
     return QIcon(pix);
}

//______________________________________________________________________________________
QByteArray EMimIcon::iconThambnail(const QString &file)
{
    QFileInfo fi(file);

    QByteArray text=file.toUtf8();

    QString thumbnail=Edir::cachDir()+"/thambnail";

    QString fileThumbnail=thumbnail+"/"+text.toHex();

    QSettings setting("elokab","thambnail");
    QBuffer buffer;
    QImage pix;

    if(QFile::exists(fileThumbnail)){

        QString fModified=setting.value(file).toString();
        if(fModified== fi.lastModified().toString("dd MM yyyy hh:mm:ss"))
            pix.load(fileThumbnail);
        // return QIcon(fileThambnail);
    }else{

        //  QPixmap pixF(file);
        pix.load((file));
        //qDebug()<<pix.isNull()<<pix.size()<<file;
        if(pix.isNull()||pix.width()==0||pix.height()==0)
            return  buffer.buffer();

        if(pix.width()>128 && pix.height()>128){

            //  pix= QImage(128,128,QImage::Format_ARGB32);

            pix= pix.scaled(QSize(124,124),Qt::KeepAspectRatio);
            if(fi.absolutePath()!=thumbnail){
                QDir dir(thumbnail);
                dir.mkpath(thumbnail);
                setting.setValue(file,fi.lastModified().toString("dd MM yyyy hh:mm:ss"));
                pix.save(fileThumbnail,"png",50);
            }
        }
    }
    //    QPixmap pix(128,128);
    QImage pixRet;
    pixRet= QImage(128,128,QImage::Format_ARGB32);

    pixRet.fill(Qt::transparent);

    QSize size= pix.scaled(QSize(124,124),Qt::KeepAspectRatio).size();

    int left=(128-size.width())/2;
    int top=(128-size.height())/2;
    QPainter p(&pixRet);

    //    p.fillRect(0,0,259,259,QColor(229,239,255,50));

    p.drawImage(QRect(left+2,top+2,size.width()-4,size.height()-4),pix,pix.rect());

    p.setPen(Qt::lightGray);
    p.drawRect(0,0,126,126);
    p.setPen(Qt::gray);
    p.drawRect(-1,-1,127,127);
    p.setOpacity(0.3);
    p.drawRect(-1,-1,128,128);
    //   p.setPen(Qt::red);
    //  p.drawPixmap(QRect(left+1,top+1,size.width()-1,size.height()-1),pixF,pixF.rect());

    //p.end();






    QImageWriter writer(&buffer,"png");
    writer.setQuality(50);
    writer.write(pixRet);
    return buffer.buffer();

}

//______________________________________________________________________________________
QIcon EMimIcon::iconColorized(QIcon icon,QColor color)
{

     QPixmap pix=(icon.pixmap(128));

     QImage sourceImage(pix.toImage());
     QImage resultImage(sourceImage);
     QImage destinationImage(sourceImage);

     destinationImage.fill(color);

     QPainter painter(&resultImage);
     painter.setCompositionMode(QPainter::CompositionMode_Overlay);
     painter.drawImage(0, 0, destinationImage);
     painter.fillRect(sourceImage.rect(),color);
     painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
     painter.drawImage(0, 0, sourceImage);

     return QIcon(QPixmap::fromImage(resultImage ));
}
//______________________________________________________________________________________
QIcon EMimIcon::iconDesktopFile(const QString &f)
{

    QHash<QString ,QString> hash=desktopFile(f);
   // QString iconXdg=hash["Icon"];
    return EIcon::fromTheme(hash.value("Icon"),"application-x-desktop");
}

//______________________________________________________________________________________
QIcon EMimIcon::iconBySuffix(const QString &suf,const QString &f)
{
     // جلب اللواحق '/usr/share/mime/globs',
     QString mim = getMimeTypeBySufix(suf);

     return iconByMimType(mim,f);
}

//______________________________________________________________________________________
QIcon EMimIcon::iconByMimType(const QString &mim,const QString &f)
{

     QString icon=iconName(mim);
     if (icon.isEmpty()) {
          QString mim= getMimeTypeByFile(f);
          QString s=iconName(mim);
          if (!s.isEmpty()){
               return  EIcon::fromTheme(s,"unknown");

          }
     }
     return EIcon::fromTheme(icon,"unknown");
}

//______________________________________________________________________________________
QString EMimIcon::getMimeTypeByFile(QString fileName)
{

     //-----------MAGIC----------------------
     magic_t cookie = magic_open(MAGIC_MIME);
     magic_load(cookie, 0);
     QString temp = magic_file(cookie, fileName.toLocal8Bit());

     magic_close(cookie);
     QString rsl=temp.left(temp.indexOf(";"));
     //    QString mim=iconName(rsl);
     //      qDebug()<<"by mime"<<icon;
     if(rsl.isEmpty())
          return  "unknown";

     return  rsl;

     /*
    //--------------XDGMIME-------------------
    QProcess process;
    process.start(QString("xdg-mime query filetype %1").arg("\""+fileName+"\""));
    process.waitForFinished(80);
    QString result= process.readAll();

    result=result.section(";",0,0); // value "application/x-unknow"


    return (result);
    */
}

//______________________________________________________________________________________
QString EMimIcon::getMimeTypeBySufix(QString sufix)
{


     // mimeTypeالبحث عن اسم الايقونة حسب اللاحقة
     QFile file("/usr/share/mime/globs");
     if (!file.open(QFile::ReadOnly)){
          return "";

     }
     QTextStream textStream(&file);
     QString line;

     line= textStream.readLine();//
     while (!line.isNull()) {
          QString s= line.section(":",1,1);
          if( s==("*."+sufix)){
               //   qDebug()<<line;             //value "application/x-unknow:*.sufix"
               line=line.section(":",0,0);      // value "application/x-unknow"
               //                               line=line.replace("/","-"); // value "application-x-unknow"


               return /*iconName*/(line );
          }
          /************************************/
          line = textStream.readLine();

     }
     return "";
}

//______________________________________________________________________________________
QString EMimIcon::iconName(QString result )
{

    QString m_groupType;
    QString m_fileSubType;
//application/x-virtualbox-vdi

    m_groupType=result.section("/",0,0); // value "application"
    m_fileSubType=result.section("/",1,1); // value "x-unknow"

    //      qDebug()<<"process.readAll() : "<< QString("%1-%2").arg(m_groupType, m_fileSubType.trimmed());;
    QStringList names;
    names << QString("%1-%2").arg("application", m_fileSubType.trimmed());// value "application-unknow"

    names << QString("%1-%2").arg(m_groupType, m_fileSubType.trimmed());// value "application-unknow"

    names << QString("%1-x-%2").arg(m_groupType, m_fileSubType.trimmed());// value "application-x-unknow"

    names << QString("%1-x-generic").arg(m_groupType.trimmed());// value "application-x-generic"

    names << QString("%1-generic").arg(m_groupType.trimmed());// value "application-generic"

    names << QString("%1").arg( m_fileSubType.trimmed());// value "x-unknow"

    names << QString("%1").arg(m_groupType.trimmed());// value "application"
 // qDebug()<<names;

    foreach (QString s, names)
    {
         if (QIcon::hasThemeIcon(s))
              return s; 
    }

    QString s=getAssosiatedIcons(result);

    if (QIcon::hasThemeIcon(s))
         return s;


    return result.replace("/","-");
}


//______________________________________________________________________________________
QString EMimIcon::iconFillBack(QString mimeType)
{
     QString iconBase=mimeType.section("-",0,0);

     QStringList list;
     list<<"text"<<"video"<<"image"<<"audio";
     foreach (QString s, list) {
          if(iconBase==s)
               return iconBase;
     }

     return "unknown";
}

//______________________________________________________________________________________
QHash<QString ,QString> EMimIcon::desktopFile(const QString &filePath,const QString &lc)
{
     QHash<QString ,QString> hash;
    QFile files(filePath);
    QString name;
    QString nameLc;
    if(files.open( QFile::ReadOnly)){
        QTextStream textStream(&files);
        QString line = textStream.readLine();//premier line;

        while   (!line.isNull()) {
            line = textStream.readLine().trimmed();
            if(line.startsWith("Name=")){
                 name=line.section('=', 1).trimmed();

            }else if(line.startsWith("Name["+lc+"]")){

                nameLc=line.section('=', 1).trimmed();

            }else if(line.startsWith("Exec=")){

                hash ["Exec"]=line.section('=', 1).trimmed();

            }else if(line.startsWith("Icon=")){

                 hash ["Icon"]=line.section('=', 1).trimmed();

            }else if(line.startsWith("FolderColor=")){

                hash ["FolderColor"]=line.section('=', 1).trimmed();

           }


        }
    }

    if(nameLc.isEmpty())
        hash ["Name"]=name;
    else
       hash ["Name"]=nameLc;

    files.close();
      return hash;


}

//______________________________________________________________________________________
QString EMimIcon::desktopFilePath(const QString &file)
{


     foreach (QString s, Edir::applicationsDirs()) {


          QDirIterator it(s, QStringList("*.desktop"),
                          QDir::Files | QDir::NoDotAndDotDot,
                          QDirIterator::Subdirectories);
          while (it.hasNext()) {
               it.next();
               if(it.fileName()==file){


                    return it.filePath();
               }

          }
     }

     return "";
}

//______________________________________________________________________________________
void EMimIcon::launchApplication(const QString & fileName)
{
     QFileInfo fi(fileName);
     launchApp(fileName,mimeTyppe(fi)) ;
}

//______________________________________________________________________________________
void EMimIcon::launchAppFile(const QString & fileName,const QString & programe)
{
     QProcess process;
     QString   proc=replaceArgument(programe);
     process.startDetached(proc+" \""+fileName+"\"");
}

//______________________________________________________________________________________
bool EMimIcon::launchApp(const QString & fileName,const QString & mimetype)
{

    QProcess process;

    QHash<QString ,QString> hash;
qDebug()<<fileName;
    QString scheme = QUrl(fileName).scheme();
    if (    scheme == "http"   || scheme == "https" || scheme == "shttp"  ||
            scheme == "ftp"    || scheme == "ftps"  || scheme == "mailto" ||
            scheme == "pop"    || scheme == "pops"  || scheme == "nntp"   ||
            scheme == "imap"   || scheme == "imaps" || scheme == "irc"    ||
            scheme == "telnet" || scheme == "xmpp"  || scheme == "irc"    ||
            scheme == "nfs")

        /*-----------------------------------URL-------------------------------------------*/
        if(fileName.startsWith(scheme)){
            QStringList list=EMimIcon::associatedApplication("text/html");
            if(list.count()>0){  /*  has associatedApplication*/
                QString f=list.at(0);

                hash=  desktopFile( desktopFilePath(f));
                QString proc=hash ["Exec"];
                if(!proc.isEmpty()){

                    proc=replaceArgument(proc);

                    return process.startDetached(proc+" \""+fileName+"\"");

                }
            }
        }

    QFileInfo info(fileName);
    qDebug()<<"************************Begin-MimeTypeXdg::launchApp()***************************";
    qDebug()<<"*                                                                               *";
    qDebug()<<"* fileName :"<<fileName;
    qDebug()<<"* isDir:"<<info.isDir()<<" isFile:"<<info.isFile()<<" isExecutable:"<<info.isExecutable()<<" mimetype:"<<mimetype;
    qDebug()<<"*                                                                               *";
    qDebug()<<"*************************END-MimeTypeXdg::launchApp()****************************";

    /*-----------------------------------DIR-------------------------------------------*/
    if(info.isDir()){
        QSettings setting("elokab","elokabsettings");
        setting.beginGroup("DefaultBrowser");
        bool defaultBrowser=(setting.value("defaultBrowser",false).toBool());

        if(defaultBrowser==true)     {

            QDesktopServices::openUrl( QUrl::fromLocalFile(fileName) );

        }else{


            QString costumBrowser=(setting.value("BROWSER","elokab-fm").toString());
            QString prog=costumBrowser.section(" ",0,0);
            QStringList list;
            if(!costumBrowser.section(" ",1,1).trimmed().isEmpty())
                list<<costumBrowser.section(" ",1,1).trimmed();

            list<<fileName;
            QProcess process;
            process.startDetached(prog,list);

        }
        setting.endGroup();
        return true;
    }
    /*---------------------------------Desktop----------------------------------------------*/

    if(info.suffix()=="desktop"||mimetype=="application/x-desktop"){


        hash= desktopFile(fileName);

        QString proc=replaceArgument(hash ["Exec"]);
        qDebug()<<mimetype<<hash ["Exec"]<<proc;
        if(proc.contains("su-to-root")){
            proc=QString("ekbsudo %1 -i %2").arg(proc).arg(hash["Icon"]);
        }
        process.startDetached(QString("%1").arg(proc));

        return true;

    }

    /*--------------------------------Executable------------------------------------------*/
    //     if(info.isExecutable()){
    if(mimetype=="application/x-executable"){
        //            cde ffor text/x-shellscript

        process.startDetached("\""+fileName+"\"");

        return true;
    }

    /*------------------------------------File-------------------------------------------*/
    if(info.isFile()){
        if(info.isExecutable()){
            process.setWorkingDirectory(info.path());
            QDir::setCurrent(info.path());
            QString proc=replaceArgument(fileName);
            if(process.startDetached(proc)==true)
                return true;
        }
        QStringList list=associatedApplication(mimetype);

        if(list.count()>0){  /*  has associatedApplication*/
            QString f=list.at(0);

            hash= desktopFile(desktopFilePath(f));
            QString proc=hash ["Exec"];
            if(!proc.isEmpty()){
                //here replaceArgument
                proc=replaceArgument(proc);
                process.startDetached(proc+" \""+fileName+"\"");
                return true;
            }
        }
    }
    /*-------------------------------Programme-------------------------------------------*/

    process.setWorkingDirectory(info.path());
    QDir::setCurrent(info.path());
    QString proc=replaceArgument(fileName);
    return  process.startDetached(proc);

    return true;
}

//______________________________________________________________________________________
QString EMimIcon::replaceArgument(const QString & proc)
{
     /*freedesktop.org
    %f	 A single file name, even if multiple files are selected. The system reading the desktop entry should recognize that the program in question cannot handle multiple file arguments, and it should should probably spawn and execute multiple copies of a program for each selected file if the program is not able to handle additional file arguments. If files are not on the local file system (i.e. are on HTTP or FTP locations), the files will be copied to the local file system and %f will be expanded to point at the temporary file. Used for programs that do not understand the URL syntax.
    %F	 A list of files. Use for apps that can open several local files at once. Each file is passed as a separate argument to the executable program.
    %u	 A single URL. Local files may either be passed as file: URLs or as file path.
    %U	 A list of URLs. Each URL is passed as a separate argument to the executable program. Local files may either be passed as file: URLs or as file path.
    %d	 Deprecated.
    %D	 Deprecated.
    %n	 Deprecated.
    %N	 Deprecated.
    %i	 The Icon key of the desktop entry expanded as two arguments, first --icon and then the value of the Icon key. Should not expand to any arguments if the Icon key is empty or missing.
    %c	 The translated name of the application as listed in the appropriate Name key in the desktop entry.
    %k	 The location of the desktop file as either a URI (if for example gotten from the vfolder system) or a local filename or empty if no location is known.
    %v	 Deprecated.
    %m	 Deprecated.
    */
     QString prog=proc;
     //     QStringList listArg;
     //     listArg<<"%f"<<"%F"<<"%u"<<"%U"<<"%d"<<"%D"<<"%n"<<"%i"<<"%c"<<"%k"<<"%v"<<"%m";
     //    foreach (QString arg, listArg) {
     //        if(prog.contains(arg)){
     //        prog.replace(arg,"");
     //        }
     //    }

     QRegExp ex("\\%[A-Z]");
     QRegExp ex2("\\%[a-z]");
     prog.remove(ex);
     prog.remove(ex2);
     prog.remove("-caption");
     return prog.trimmed();
}

//______________________________________________________________________________________
QString EMimIcon::formatSize(qint64 num)
{
     QString total;
     const qint64 kb = 1024;
     const qint64 mb = 1024 * kb;
     const qint64 gb = 1024 * mb;
     const qint64 tb = 1024 * gb;

     if (num >= tb) total = QString(QObject::tr("%1 TB")).arg(QString::number(qreal(num) / tb, 'f', 2));
     else if(num >= gb) total = QString(QObject::tr("%1 GB")).arg(QString::number(qreal(num) / gb, 'f', 2));
     else if(num >= mb) total = QString(QObject::tr("%1 MB")).arg(QString::number(qreal(num) / mb, 'f', 1));
     else if(num >= kb) total = QString(QObject::tr("%1 KB")).arg(QString::number(qreal(num) / kb,'f', 1));
     else total = QString(QObject::tr("%1 bytes")).arg(num);

     return total;
}

//______________________________________________________________________________________
QHash<QString ,qint64> EMimIcon::getDriveInfo(QString path)
{

     struct statfs info;
     statfs(path.toLocal8Bit(), &info);
     QHash<QString ,qint64>hash;
     if(info.f_blocks == 0) return hash;

     hash["Perc"]= (qint64) (info.f_blocks - info.f_bavail)*100/info.f_blocks;
     hash["Used"]= (qint64) (info.f_blocks - info.f_bavail)*info.f_bsize;
     hash["Total"]= (qint64) info.f_blocks*info.f_bsize;
     return hash;

}

//______________________________________________________________________________________
void EMimIcon::updateMimeAssociatedApplication()
{

     QString s="/usr/share/applications/";
     //mime  desktop
     QHash<QString ,QStringList> hash;

  //   qDebug()<<"MimeTypeXdg::updateMimeAssociatedApplication()";

     QDirIterator it(s, QStringList("*.desktop"),
                     QDir::Files | QDir::NoDotAndDotDot,
                     QDirIterator::Subdirectories);
     while (it.hasNext()) {
          it.next();

          // QStringList listmim= getMimeDesktop(it.filePath());
          foreach (QString mim, getMimeDesktop(it.filePath())) {
               if(mim.isEmpty())
                    continue;
               QStringList list;
               if(!hash.contains(mim)){

                    list<<it.fileName();
                    hash[mim]=list;

               }else{
                    list=hash[mim];
                    list.append(it.fileName());
                    hash[mim]=list;
               }
          }//listmim


     }//while

     QSettings settings("elokab","mimeappslist");
     //   settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
     settings.beginGroup("Default-Applications");
     QHashIterator<QString, QStringList> i(hash);
     while (i.hasNext()) {
          i.next();
          //  qDebug() << i.key() << ": " << i.value();
          QStringList list=settings.value(i.key() ).toStringList();
          if(list.isEmpty()){
               list=i.value();
             //  qDebug()<<"new list"<<i.value();
               settings.setValue(i.key(),list);
          }else{
               foreach (QString s, i.value()) {
                    if(!list.contains(s)){
                         list.append(s);
                        // qDebug()<<"new desktop prog"<<s;
                         //qDebug()<<"new list prog"<<list;
                         settings.setValue(i.key(),list);
                    }
               }
          }



     }
     settings.endGroup();


}

//______________________________________________________________________________________
QStringList EMimIcon::getMimeDesktop(const QString &fileName )
{
     QStringList list;
     QFile files(fileName);
     if(files.open( QFile::ReadOnly)){
          QTextStream textStream(&files);
          QString line = textStream.readLine();//premier line;

          while   (!line.isNull()) {

               line = textStream.readLine();

               if(line.contains("MimeType=")){
                    QString mimeType=line.remove("MimeType=");
                    list=mimeType.split(";");
               }
          }
     }
     files.close();
     return list;
}

//______________________________________________________________________________________
QStringList mimAliasType(const QString &mim)
{


     QStringList list;
     QFile file("/usr/share/mime/"+mim+".xml");

     if (!file.open(QIODevice::ReadOnly))
          return list;


     QXmlStreamReader xml;
     xml.setDevice(&file);

     while (!xml.atEnd()) {

          xml.readNext();

          if (xml.name() == "alias")
          {

               list<< xml.attributes().value("type").toString();
          }

     }
     xml.clear();
     file.close();

     return list;
}
//______________________________________________________________________________________
QString EMimIcon::getAssosiatedIcons(const QString &mim)
{


     QString iconName;
     QFile file("/usr/share/mime/"+mim+".xml");

     if (!file.open(QIODevice::ReadOnly))
          return mim;


     QXmlStreamReader xml;
     xml.setDevice(&file);

     while (!xml.atEnd()) {

          xml.readNext();

          if (xml.name() == "generic-icon"||xml.name() == "icon")
          {

               iconName= xml.attributes().value("name").toString();
               break;
          }

     }
     xml.clear();
     file.close();

     return iconName;
}
//______________________________________________________________________________________
QStringList EMimIcon::associatedApplication(const QString &mim)
{
     QStringList listApp;

     QSettings settings("elokab","mimeappslist");
     if(!QFile::exists(settings.fileName())||QFileInfo(settings.fileName()).size()==0)
          updateMimeAssociatedApplication();

     settings.beginGroup("Default-Applications");
     listApp=   settings.value(mim).toStringList();


     if(listApp.isEmpty()){

          foreach (QString aliastype, mimAliasType(mim)) {
               listApp.append(  settings.value(aliastype).toStringList());
          }

     }


     if(listApp.isEmpty()){
          if(mim.startsWith("text"))
               listApp=   settings.value("text/plain","").toStringList();
          else if(mim.startsWith("image"))
               listApp=   settings.value("image/png","").toStringList();
          else if(mim.startsWith("video"))
               listApp=   settings.value("video/x-ogg","").toStringList();
          else if(mim.startsWith("audio"))
               listApp=   settings.value("audio/x-ogg","").toStringList();

        //  qDebug()<<"associatedApplication "<<listApp;
     }


     settings.endGroup();

     return listApp;
}

//______________________________________________________________________________________
void EMimIcon::setMimeAssociatedApplication(const QString &mimType,const QStringList &listAppDesktop  )
{
     QSettings settings("elokab","mimeappslist");

     settings.beginGroup("Default-Applications");

     settings.setValue(mimType,listAppDesktop);


     settings.endGroup();
}

//______________________________________________________________________________________
void EMimIcon::AddMimeAssociatedApplication(const QString &mimType,const QString &appDesktop  )
{


     QSettings settings("elokab","mimeappslist");

     settings.beginGroup("Default-Applications");
     //get list appAssociated if exist
     QStringList list=settings.value(mimType).toStringList();
     list.insert(0,appDesktop);
     //save the new list
     settings.setValue(mimType,list);

     settings.endGroup();
}

//______________________________________________________________________________________
QString EMimIcon::mimLang(const QString &mim,const QString &local)
{
     QString mimType=mim;
     if(mimType=="text/x-shellscript")
          mimType="application/x-shellscript";

     QFile file("/usr/share/mime/"+mimType+".xml");

     if (!file.open(QIODevice::ReadOnly))
          return mimType;

     QString ret;
     QString retDefault;
     QString retEn=mimType;
     QXmlStreamReader xml;
     xml.setDevice(&file);

     while (!xml.atEnd()) {

          xml.readNext();

          if (xml.name() == "comment")
          {

               QString lang= xml.attributes().value("xml:lang").toString();
               if(lang.isNull())
               {
                    QString  def= xml.readElementText();
                    if(!def.isEmpty())
                         retDefault=def;
 continue;
               }else if(lang.startsWith("en")){

                    retEn=xml.readElementText();
 continue;

               }else if(lang==local){

                    ret= xml.readElementText();
                    break;

               }




          }

     }
     xml.clear();
     file.close();
     if(ret.isNull()||ret.isEmpty())
          ret=retDefault;
     if(ret.isNull()||ret.isEmpty())
          ret=retEn;
     return ret.trimmed();
}

QString EMimIcon::createDescktopFile(const QString &exec)
{
    QFileInfo fi(exec);
    QString name=fi.baseName();
    QString path=Edir::applicationsHomeDir()+"/";
    QString fileName=path+name+".desktop";

    if(QFile::exists(fileName)){
        int i=0;
        while (QFile::exists(fileName)) {
            i++;
            QString mName=name+QString::number(i);
            fileName=path+mName+".desktop";
        }
    }
    /*
    QSettings settings(fileName,QSettings::IniFormat);

   settings.setIniCodec(QTextCodec::codecForLocale());

    settings.beginGroup("Desktop Entry");
    settings.setValue("Type","Application");
    settings.setValue("Name",fi.baseName());
    settings.setValue("Icon","application-x-desktop");
    settings.setValue("Exec",exec);
    settings.setValue("NoDisplay",true);

settings.endGroup();
*/

    QFile fileS(fileName);

    if (! fileS.open(QIODevice::WriteOnly ))
        return name+".desktop";

    QTextStream out(&fileS);
    out.setCodec(QTextCodec::codecForName("UTF-8"));
    out<<"[Desktop Entry]"<< endl;;
    out<<"Type=Application"<< endl;
    out<<"Name="<<fi.baseName()<< endl;
    out<<"Icon=application-x-desktop"<< endl;
    out<<"Exec="<<exec<< endl;
    out<<"NoDisplay=true\n"<< endl;
    fileS.close();

    return QFileInfo(fileName).fileName();
}
