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

#include "fileinformation.h"
#include "ui_fileinformation.h"
#include <EMimIcon>
#include "messages.h"
#include <QFileInfo>
#include <QDateTime>
#include <QMessageAuthenticationCode>
#include <QProcess>
//#ifdef DEBUG_APP
//#include <QDebug>
//#endif
/**************************************************************************************
 *                                  FILEINFORMATION
 **************************************************************************************/
FileInformation::FileInformation(QWidget *parent) :
     QWidget(parent),
     ui(new Ui::FileInformation)
{

#ifdef DEBUG_APP
     Messages::showMessage(Messages::TOP,"FileInformation::FileInformation()");
#endif


     ui->setupUi(this);
     setContextMenuPolicy(Qt::CustomContextMenu);
 canReadAudio=EMimIcon::findProgram("ffmpeg");
#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"FileInformation::FileInformation()");
#endif
}

/**************************************************************************************
 *                                  FILEINFORMATION
 **************************************************************************************/
FileInformation::~FileInformation()
{
     delete ui;
}

/**************************************************************************************
 *                                  FILEINFORMATION
 **************************************************************************************/
void FileInformation:: showEvent ( QShowEvent * /*event*/ )
{
     setFileName(mFile);
}


/**************************************************************************************
 *                                  FILEINFORMATION
 **************************************************************************************/
void FileInformation::setFileName(const QString &file)
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"FileInformation::setFileName()");
#endif
     mFile=file;
     if(!this->isVisible())
          return;



#ifdef DEBUG_APP
     Messages::showMessage(Messages::NORMALE,"FileInformation::setFileName()","file:"+file);
#endif



     if(mFile.startsWith("file://"))
     {
         mFile.remove("file://");
         QFileInfo fi(mFile);

         QString txt=fi.fileName();
         int pointSize=this->font().pointSize();
         int lent=txt.length();

         int width=200/pointSize;
         if(lent>width){
             int pos=0;
             while (pos<lent) {
                 int index=txt.indexOf(" ",pos);
                 pos=width+pos;
                 if(index==-1||(index-pos)>width)
                     txt.insert(pos,"\n")  ;
             }
         }

         ui->labelTitle->setText(txt);
//         if(fi.isDir())
//             setDirInformation(fi);
//         else
             setFileInformation(fi);



     }else{
          ui->labelTitle->setText("");
          ui->labelInfo->setText(file);
          ui->labelPixmap->setPixmap(EIcon::fromTheme("help-info").pixmap(128));
     }

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"FileInformation::setFileName()");
#endif
}


/**************************************************************************************
 *                                  FILEINFORMATION
 **************************************************************************************/
void FileInformation::setFileInformation(const QFileInfo &fi)
{


    QString mim=EMimIcon::mimeTyppe(fi);
    bool hasImage=false;
    bool hasAudio=false;
    QPixmap pix;
    int scal=128;

    if(mim.startsWith("video") || mim.endsWith("pdf"))
    {

        QMessageAuthenticationCode code(QCryptographicHash::Md5);
        code.addData(mFile.toUtf8());
        QString md5Name=code.result().toHex();
        QString fileThumbnail=Edir::thumbnaileCachDir()+"/"+md5Name;

        if(QFile::exists(fileThumbnail)){
            if(pix.load(fileThumbnail))
            pix=(QPixmap(fileThumbnail));
        }

    }else if(mim.startsWith("image")) {

        if(pix.load(mFile)){
            hasImage=true;
            int max=qMax(pix.width(),pix.height());
            if(max>=200) scal=200;
            else if(max<=200) scal=128;
            else scal=max;

        }

    }else if(canReadAudio && mim.startsWith("audio")){
        hasAudio=true;
    }

    if(pix.isNull())
        pix=EMimIcon::icon(fi,false).pixmap(128).scaled(128,128);
    //QIcon icon=EMimIcon::icon(fi,false);
    ui->labelPixmap->setPixmap(QPixmap(pix.scaled(QSize(scal,scal),Qt::KeepAspectRatio,Qt::SmoothTransformation)));

    QString infoStr;
    if(fi.isSymLink()) infoStr+=tr("Point To: %1 \n"). arg(fi.symLinkTarget());
    infoStr+= QString(tr("Type: %1 \n")).arg(EMimIcon::mimLang(mim));

    if(fi.isDir())
        infoStr+= QString(tr("Size: %1 \n")).arg(getDirSize(mFile));
    else
        infoStr+= QString(tr("Size: %1 \n")).arg(EMimIcon::formatSize(fi.size()));

    infoStr+= QString(tr("Modified: %1 \n")).arg(fi.lastModified().toString("dd.MM.yyyy hh:mm"));
    infoStr+= "\n";
    infoStr+= QString(tr("User Permission: %1  %2  %3\n"))
            . arg(fi.permission(QFile::ReadUser) ? "r" : "-")
            . arg(fi.permission(QFile::WriteUser) ? "w" : "-")
            . arg(fi.permission(QFile::ExeUser) ? "x" : "-");

    if(hasImage){
        infoStr+="\n";
        infoStr+= QString(tr("Width: %1 \n")).arg(pix.width());
        infoStr+= QString(tr("Height: %1 \n")).arg(pix.height());
    }

    if(hasAudio){
        QMap<QString, QString> map=audioInfo();
         infoStr+="\n";
//         if(!map["TITLE"].isEmpty())
//              infoStr+= QString(tr("Title: %1 \n")).arg(map["TITLE"]);
        if(!map["Duration"].isEmpty())
             infoStr+= QString(tr("Duration: %1 \n")).arg(map["Duration"].leftRef(8));
        if(!map["ARTIST"].isEmpty())
             infoStr+= QString(tr("Artist: %1 \n")).arg(map["ARTIST"]);
        if(!map["ALBUM"].isEmpty())
             infoStr+= QString(tr("Album: %1 \n")).arg(map["ALBUM"]);
        if(!map["GENRE"].isEmpty())
             infoStr+= QString(tr("Genre: %1 \n")).arg(map["GENRE"]);

        if(QFile::exists(fi.path()+"/.AlbumArt.png"))
             ui->labelPixmap->setPixmap(QPixmap(fi.path()+"/.AlbumArt.png"));
    }
    //-------------------------------------------------------------------
    ui->labelInfo->setText(infoStr);

}

/**************************************************************************************
 *                                 SIZEINFORMATION
 **************************************************************************************/
QString FileInformation::getDirSize(const QString &path)
{


     QDir dir(path);
     int folders=0,files=0;

     foreach (QString subfile, dir.entryList(QDir::AllEntries|  QDir::NoDotAndDotDot|QDir::Hidden))
     {
          QFileInfo fi(dir.absoluteFilePath(subfile));

          if(fi.isDir()) folders++;

          else files++;


     }

     return (QString::number(folders)+tr(" sub-folders ")+QString::number(files)+ tr(" files"));

     //return size;
}

QMap<QString, QString> FileInformation::audioInfo()
{
QFileInfo fi(mFile);
    QMap<QString, QString> map;
    QStringList list2;
    list2<<"-i"<<mFile;
    if(!QFile::exists(fi.path()+"/.AlbumArt.png"))
        list2 <<"-n"<<"-s"<<"128x128"<<fi.path()+"/.AlbumArt.png";

    QProcess p;
    //-------------------------------------------
    p.start("ffmpeg",list2);
    if (!p.waitForStarted()) {   return map ;  }

    if (!p.waitForFinished()){   return map;   }

    QString error=p.readAllStandardError();

    if(error.isEmpty())return map;

    QStringList list=error.split("\n");

    foreach (QString s, list) {
        s=s.trimmed();
        if(s.startsWith("Duration")){
            s=s.remove("Duration:");
            QString name=s.section(",",0,0).trimmed();

            map["Duration"]=name;

        }

        else if(s.startsWith("ARTIST")|| s.startsWith("artist")){

            QString name=s.section(":",1).trimmed();

            map["ARTIST"]=name;

        }

        else if(s.startsWith("ALBUM")|| s.startsWith("album")){

            QString name=s.section(":",1).trimmed();

            map["ALBUM"]=name;

        }

        else if(s.startsWith("GENRE" ) || s.startsWith("genre")){

            QString name=s.section(":",1).trimmed();

            map["GENRE"]=name;

        }

        else if(s.startsWith("TITLE" ) || s.startsWith("title")){

            QString name=s.section(":",1).trimmed();

            map["TITLE"]=name;

        }

    }


    return map;

}
