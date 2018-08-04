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
#include <QImageReader>
#include <QStandardPaths>
#include <QApplication>
#include <QMessageAuthenticationCode>
#include <QMessageBox>

Q_GLOBAL_STATIC(EMimIcon, EMimIconInstance)
EMimIcon *EMimIcon::instance()
{
    return EMimIconInstance();
}

void EMimIcon::setlocale(QString lc){instance()->mLocal=lc;}
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
            pix.loadFromData(iconThumbnails(minfo.absoluteFilePath()));
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
           pix.loadFromData(iconThumbnails(minfo.absoluteFilePath()));
            QIcon icon=QIcon(pix);

           // QIcon icon=Thumbnails(minfo.absoluteFilePath());
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
        return "inode/directory";

    QString mim;
    QString suf=info.suffix().toLower();
    if(!suf.isEmpty()) mim= getMimeTypeBySufix(suf);

    if(!mim.isEmpty())return mim;

    return getMimeTypeByFile(info.absoluteFilePath());

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


          QHash<QString ,QVariant> hash=desktopFile(f+"/.directory");


          QString folderColor=hash.value("FolderColor").toString();
          if(!folderColor.isEmpty()){
              QColor color(folderColor);
               if(color.isValid())
                  return iconColorized(EIcon::fromTheme("folder"),color);
          }
           QString iconXdg=hash.value("Icon").toString();
          if(!iconXdg.isEmpty()){
              if(QFile::exists(iconXdg.replace("./",f+"/"))){
                  return QIcon(iconXdg);
              }

              return EIcon::fromTheme(iconXdg,"folder");
          }

     }

#if QT_VERSION >= 0x050000
    if(dir.path()==QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))
         return EIcon::fromTheme("folder-documents","folder");
    if(dir.path()==QStandardPaths::writableLocation(QStandardPaths::MusicLocation))
         return EIcon::fromTheme("folder-music","folder");
    if(dir.path()==QStandardPaths::writableLocation(QStandardPaths::MoviesLocation))
         return EIcon::fromTheme("folder-video","folder");
    if(dir.path()==QStandardPaths::writableLocation(QStandardPaths::PicturesLocation))
         return EIcon::fromTheme("folder-pictures","folder");
    if(dir.path()==QStandardPaths::writableLocation(QStandardPaths::DownloadLocation))
         return EIcon::fromTheme("folder-download","folder");
    if(dir.path()==QStandardPaths::writableLocation(QStandardPaths::DesktopLocation))
         return EIcon::fromTheme("user-desktop","folder");

#else

     if(dir.path()==QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation))
          return EIcon::fromTheme("folder-documents","folder");
     if(dir.path()==QDesktopServices::storageLocation(QDesktopServices::MusicLocation))
          return EIcon::fromTheme("folder-music","folder");
     if(dir.path()==QDesktopServices::storageLocation(QDesktopServices::MoviesLocation))
          return EIcon::fromTheme("folder-video","folder");
     if(dir.path()==QDesktopServices::storageLocation(QDesktopServices::PicturesLocation))
          return EIcon::fromTheme("folder-pictures","folder");
     if(dir.path()==QDesktopServices::storageLocation(QDesktopServices::DesktopLocation))
          return EIcon::fromTheme("user-desktop","folder");
 #endif

     //الايقونة الافتراضية للمجلد
     return EIcon::fromTheme("folder");
}

//______________________________________________________________________________________
QIcon EMimIcon::iconSymLink(QIcon icon)
{
     QPixmap pixSym=QIcon(EIcon::fromTheme("emblem-symbolic-link")).pixmap(64);
     QPixmap pix=icon.pixmap(QSize(128,128));
     QPainter p;
     p.begin(&pix);
     p.drawPixmap(QRect(0,0,64,64),pixSym,pixSym.rect());
     p.end();
     return QIcon(pix);
}

//______________________________________________________________________________________
QByteArray EMimIcon::iconThumbnails(const QString &file)
{
    QFileInfo fi(file);

    QMessageAuthenticationCode code(QCryptographicHash::Md5);
    code.addData(file.toLatin1());

   // QByteArray text=file.toUtf8();

    QString thumbnail=Edir::cachDir()+"/thumbnails";

    QString fileThumbnail=thumbnail+"/"+code.result().toHex();

    QSettings setting(QApplication::organizationName(),"thumbnails");
    QBuffer buffer;
    QImage pix;
    bool hasThumb=false;

    if(QFile::exists(fileThumbnail)){
        QImageReader reader(fileThumbnail);
        QString fModified=reader.text("DATETIME");
        if(fModified== fi.lastModified().toString("dd MM yyyy hh:mm:ss")){
            pix.load(fileThumbnail);
            hasThumb=true;
        }
        // return QIcon(fileThumbnails);
    }


   if(!hasThumb){

      QImageReader reader(file);

     //  pix.load((file));
       if(qMax(reader.size().width(),reader.size().height())<=128){
           pix.load((file));
       }
   }



    QImageWriter writer(&buffer,"png");
    writer.setQuality(50);
    writer.write(pix);
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

    QHash<QString ,QVariant> hash=desktopFile(f);
   // QString iconXdg=hash["Icon"];
    QString icon=hash.value("Icon").toString();
    if(icon.isEmpty() )
      return  EIcon::fromTheme("application-x-desktop");

    return EIcon::fromTheme(icon,"application-x-desktop");
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
    // magic_t cookie = magic_open(MAGIC_MIME);
     magic_t cookie = magic_open(MAGIC_MIME_TYPE);

     magic_load(cookie, nullptr);
     QString temp = magic_file(cookie, fileName.toLocal8Bit());

     magic_close(cookie);
     //qDebug()<<"magic"<<temp;
    // QString rsl=temp.left(temp.indexOf(";"));
     QString rsl=temp;

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

     //value "application/x-unknow:*.sufix"
    QString line= textStream.readLine();//
     while (!line.isNull()) {
          QString s= line.section(":",1,1);
          if( s==("*."+sufix)){
               return line.section(":",0,0); // value "application/x-unknow"
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
QHash<QString ,QVariant> EMimIcon::desktopFile(const QString &filePath,const QString &lc)
{
     QHash<QString ,QVariant> hash;
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

            }else if(line.startsWith("Terminal=")){

                 hash ["Terminal"]=line.section('=', 1).trimmed();

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

    QHash<QString ,QVariant> hash;
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
                QString proc=hash ["Exec"].toString();
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
        QSettings setting(QApplication::organizationName(),"elokabsettings");
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

        QString proc=replaceArgument(hash ["Exec"].toString());
        qDebug()<<mimetype<<hash ["Exec"]<<proc;
        if(proc.contains("su-to-root")){
            proc=QString("ekbsudo %1 -i %2").arg(proc).arg(hash["Icon"].toString());
        }
         QString term=defaultTerminal();
        if(hash.value("Terminal",false).toBool()==true)
            process.startDetached(QString("%1 -e %2").arg(term).arg(proc.trimmed()));//Todo remplace xfce4-terminal
        else
        process.startDetached(QString("%1").arg(proc));

        return true;

    }

    /*--------------------------------Executable------------------------------------------*/
    //     if(info.isExecutable()){
    if(mimetype=="application/x-executable"){
        //            cde ffor text/x-shellscript
        qDebug()<<"lanchapp"<<"application/x-executable";

        process.startDetached("\""+fileName+"\"");

        return true;
    }

    /*------------------------------------File-------------------------------------------*/
    if(info.isFile() && info.isExecutable()){
        if( mimetype.startsWith("text") || mimetype.contains("application/x-shellscript"))
               {
            qDebug()<<"lanchapp"<<info.isExecutable()<<mimetype.startsWith("text");
            QMessageBox msgBox;
             msgBox.setText("The document has been modified.");
           //  msgBox.setInformativeText("Do you want to save your changes?");
             msgBox.setStandardButtons(QMessageBox::Cancel );
             msgBox.setDefaultButton(QMessageBox::Cancel);
             QPushButton *execButton = msgBox.addButton(QObject::tr("Execute"), QMessageBox::ActionRole);
             QPushButton *execTerminalButton = msgBox.addButton(QObject::tr("Execute in terminal"), QMessageBox::ActionRole);
             QPushButton *editButton = msgBox.addButton(QObject::tr("Edit"), QMessageBox::ActionRole);


             msgBox.exec();

              if (msgBox.clickedButton() == execButton) {
                  process.setWorkingDirectory(info.path());
                  QDir::setCurrent(info.path());
                  QString proc=replaceArgument(fileName);
                  if(process.startDetached(proc)==true)
                      return true;

              } else if (msgBox.clickedButton() == execTerminalButton) {
                    qDebug()<<"lanchapp"<<"execTerminalButton";
                  QString proc=replaceArgument(fileName);
                 QString terminal=defaultTerminal();
                 QStringList list;
                 list<<"-e"<<proc;

                   process.setWorkingDirectory(info.path());
                   QDir::setCurrent(info.path());

                   if(process.startDetached(terminal,list,info.path())==true)
                       return true;

              } else if (msgBox.clickedButton() == editButton) {
                    qDebug()<<"lanchapp"<<"abortButton";

              } else{
                   qDebug()<<"lanchapp"<<"Cancel";
                  return true;
              }



        }
 }

    if(info.isFile()){
        QStringList list=associatedApplication(mimetype);

        if(list.count()>0){  /*  has associatedApplication*/
            QString f=list.at(0);

            hash= desktopFile(desktopFilePath(f));
            QString proc=hash ["Exec"].toString();
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

   // return true;
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

     QSettings settings(QApplication::organizationName(),"mimeappslist");
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

     QSettings settings(QApplication::organizationName(),"mimeappslist");
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
     QSettings settings(QApplication::organizationName(),"mimeappslist");

     settings.beginGroup("Default-Applications");

     settings.setValue(mimType,listAppDesktop);


     settings.endGroup();
}

//______________________________________________________________________________________
void EMimIcon::AddMimeAssociatedApplication(const QString &mimType,const QString &appDesktop  )
{


     QSettings settings(QApplication::organizationName(),"mimeappslist");

     settings.beginGroup("Default-Applications");
     //get list appAssociated if exist
     QStringList list=settings.value(mimType).toStringList();
     list.insert(0,appDesktop);
     //save the new list
     settings.setValue(mimType,list);

     settings.endGroup();

}

//______________________________________________________________________________________
QString EMimIcon::mimLang(const QString &mim)
{
    if(instance()->hashMimLang.contains(mim)){
     //  qDebug()<<"mimLang"<<mim<<instance()->hashMimLang.value(mim);
        return instance()->hashMimLang.value(mim);

    }

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

               }else if(lang==instance()->mLocal){

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

     instance()->hashMimLang.insert(mim,ret.trimmed());

     if(ret.isNull()||ret.isEmpty())
          ret=mim;
//  qDebug()<<"inser"<<mim<<ret;
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

QString EMimIcon::defaultTerminal()
{
    QByteArray sS=qgetenv("DESKTOP_SESSION");
    qDebug()<<"envirenment"<<sS;
    QString terminal;

    QSettings setting(QApplication::organizationName(),QApplication::applicationName());
    setting.beginGroup("Main");
    terminal=setting.value("Terminal").toString();
    setting.endGroup();
    if(findProgram(terminal))
        return terminal;

    //search in enverenment
    if(sS=="xfce")
        terminal="xfce4-terminal";
    else if(sS=="Enlightenment"||sS.contains("enlightenment"))
        terminal="terminology";
    else if(sS==" plasma-wayland-session"||sS==" plasma-session"||sS.contains("plasma"))
        terminal="konsole";
    else if(sS=="gnome-session"||sS.contains("gnome"))
        terminal="gnome-terminal";
    else if(sS=="cinnamon-session")
        terminal="gnome-terminal";
    else if(sS.contains("deepin"))
        terminal="gnome-terminal";
    else if(sS==("lxsession"))
        terminal="lxterminal";
    //search list of terminal

    if(terminal.isEmpty()){
        //NOTE تغيير هذه القائمة بعد مدة
        QStringList list;
        list<<"elokab-terminal"<<"gnome-terminal"<<"konsole"<<"termite"
           <<"deepin-terminal"<<"terminology"<<"xfce4-terminal"
          << "lxterminal"<<"qterminal"<<"mate-terminal"<<"pantheon-terminal"
          <<"terminator"<<"theterminal"<<"aterm"
         <<"urxvt"<<"eterm"<<"mlterm"<<" tilda";

        QStringList dirs = QString(getenv("PATH")).split(":");

        foreach(QString dir,dirs)
        {
            foreach (QString term, list) {
                if(QFile::exists(dir+"/"+term)){
                    terminal=term;
                    qDebug()<<"terminal exist"<<dir+"/"+term;
                    break;
                }
            }
            if(!terminal.isEmpty())break;
        }
    }

    if(findProgram(terminal))
    { return terminal;}

        terminal="xterm";

    qDebug()<<"defaultTerminal"<<terminal;
    return terminal;

}

/*********************************************************************
 *
 *********************************************************************/
bool EMimIcon::findProgram(const QString &program)
{

    if(program.isEmpty()) return false;

     QFileInfo fi(program);
     if (fi.isExecutable())
          return true;

     QString path = qgetenv("PATH");
     foreach(QString dir, path.split(":"))
     {
          QFileInfo fi= QFileInfo(dir + QDir::separator() + program);
          //  qDebug()<<fi.filePath();

          if (fi.isExecutable() )
               return true;
     }
     return false;
}

QMap<QString, QString> EMimIcon::trachInfo(const QString &fileName)
{

    QFileInfo info(fileName);
    QString name=  info.fileName();
    QString infoname=Edir::trashInfo()+"/"+name+".trashinfo";


    QTextCodec *codec = QTextCodec::codecForName("UTF-8");

    QSettings setting(infoname,QSettings::IniFormat);
    setting.setIniCodec(QTextCodec::codecForName("UTF-8"));
    setting.beginGroup("Trash Info");
    QString path=setting.value("Path","").toString();
    QString dat=setting.value("DeletionDate","").toString();
    setting.endGroup();

    QByteArray encodedString =QByteArray::fromPercentEncoding(path.toUtf8());

    QString string = codec->toUnicode(encodedString);
    QMap <QString,QString>map;
    map["path"]=string;
    map["date"]=dat;

    return map;

}

bool EMimIcon::isArchive(const QString &mim)
{
    QStringList listArchive;
    listArchive<<"application/x-7z-compressed"<<"application/x-7z-compressed-tar"
              <<"application/x-ace"<<"application/x-alz"<<"application/x-ar"
             <<"application/x-arj"<<"application/x-bzip"<<"application/x-bzip-compressed-tar"
            <<"application/x-bzip1"<<"application/x-bzip1-compressed-tar"
           <<"application/x-cabinet"<<"application/x-cd-image"<<"application/x-compress"
          <<"application/x-compressed-tar"<<"application/x-cpio"<<"application/x-deb"
         <<"application/x-ear"<<"application/x-ms-dos-executable"<<"application/x-gtar"
        <<"application/x-gzip"<<"application/x-gzpostscript"<<"application/x-java-archive"
       <<"application/x-lha"<<"application/x-lhz"<<"application/x-lrzip"
      <<"application/x-lrzip-compressed-tar"<<"application/x-lzma"
      <<"application/x-lz4"<<"application/x-lzip"<<"application/x-lzip-compressed-tar"
     <<"application/x-lzma-compressed-tar"<<"application/x-lzop"<<"application/x-lz4-compressed-tar"
    <<"application/x-lzop-compressed-tar"<<"application/x-ms-wim"
    <<"application/x-rar"<<"application/x-rar-compressed"
    <<"application/x-rpm"<<"application/x-source-rpm"<<"application/x-rzip"
    <<"application/x-rzip-compressed-tar"<<"application/x-tar"
    <<"application/x-tarz"<<"application/x-stuffit"<<"application/x-war"
    <<"application/x-xz"<<"application/x-xz-compressed-tar"
    <<"application/x-zip"<<"application/x-zip-compressed"
    <<"application/x-zoo"<<"application/zip"<<"application/x-archive"
    <<"application/vnd.ms-cab-compressed"<<"application/vnd.debian.binary-package"
    <<"application/gzip"<<"application/vnd.rar";

    if(listArchive.contains(mim))
        return true;
    return false;
}
