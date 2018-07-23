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
#include "confirmdlg.h"
#include <QFileInfo>
#include <QDirIterator>
#include <QDebug>
#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>
#include <QProcess>
#include <QFontMetrics>
#include <EDir>
//______________________________________________________________________________________
FilesActions::FilesActions(QObject *parent) :
     QObject(parent),
     mStop(0),
     mOverWrite(0),
     mSkyp(0),
     mApplyAll(0),
     mApplyAllFolder(0),
     mSkypFolder(0),
     mWriteIntoFolder(0)

{

     progress=new ProgressDlg;
     progress->setMaximum(0);
     progress->setValue(-1);
     progress->setInfoText("creatting list Of Files...");
     connect(progress,SIGNAL(rejected()),this,SLOT(slotCancel()));

}

//______________________________________________________________________________________
void FilesActions::removeFiles(const QStringList &files)
{
     mListDirs.clear();
     mListFiles.clear();
     progress->setWindowTitle(tr("Remove Files"));
     progress->show();
     qApp->processEvents();

     foreach (QString f, files) {
          QFileInfo fi(f);

          if(fi.isDir())
          {
               mListDirs.prepend(f);
               createListForRemove(f);
          }else{
               mListFiles.append(f);
          }

     }

     qApp->processEvents();

     progress->setMaximum(mListFiles.count()+mListDirs.count());
     int r=0;
     foreach (QString file, mListFiles) {
          r++;
          progress->setValue(r);
          QString txt=file;
          QFontMetrics fm(qApp->font());

        //  if (txt.length() > 50) txt = "/.../" + txt.split(QDir::separator()).last();

          int w=fm.width(txt);
          if(w>400)
              txt = "/.../" + txt.split(QDir::separator()).last();

          txt=fm.elidedText(txt,Qt::ElideRight,400);
          progress->setInfoText(txt);
          qApp->processEvents();

          QFile::remove(file);
     }


     foreach (QString path, mListDirs) {
          r++;
          progress->setValue(r);
          QString txt=path;
          if (txt.length() > 50) txt = "/.../" + txt.split(QDir::separator()).last();

          progress->setInfoText(txt);

          QDir dir;

          dir.rmdir(path);
     }

     progress->close();

}

//______________________________________________________________________________________
void FilesActions::createListForRemove(const QString &path)
{
     QDirIterator it(path,QDir::AllEntries |
                     QDir::System |
                     QDir::NoDotAndDotDot |
                     QDir::Hidden, QDirIterator::Subdirectories);

     while(it.hasNext())
     {
          if(mStop)return;

          it.next();

          QString filePath=it.filePath();

          if(it.fileInfo().isDir())
          {
               mListDirs.prepend(filePath);
          }
          else
          {

               mListFiles.append(filePath);
          }
     }
}

//______________________________________________________________________________________
void FilesActions::pastFiles(const QStringList &files,const QString &distDir)
{

     if(!QFileInfo(distDir).isWritable()){
          QMessageBox::critical(0,"",tr("Read only...cannot create file"));
          return;
     }

     m_hash.clear();
 m_hashSymLink.clear();

     progress->setWindowTitle(tr("Copy Files"));
     progress->show();
     qApp->processEvents();
     if(createList(files,distDir))
     {

          progress->setMaximum(m_hash.count());
          //----------------------------------------------------------------

          //-----------------------------------------------------------------
          int r=0;
          QHashIterator<QString, QString> i(m_hash);
          while (i.hasNext())
          {
              i.next();
              r++;
              progress->setValue(r);
              QString txt=i.key();
              if (txt.length() > 50) txt = "/.../" + txt.split(QDir::separator()).last();

              progress->setInfoText(txt);

              QFile fi(i.value());
              QFileInfo info(i.key());
              QString    destination=i.value();

              if(info.isSymLink()){

                  continue;
              }

              if(fi.exists())
              {

                  qDebug()<<"file exist"<<i.value();
                  if(!mApplyAll)
                  {
                      destination=confirmName( i.key(), i.value());
                      if(destination.isEmpty())
                          break;
                  }

                  if(mOverWrite)
                      QFile::remove(destination);
                  if(mSkyp)
                      continue;

              }


             // if this file > 10mb write bolk size
              if(info.size()<10000000){
                  progress->setCopyVisible(false);
                  QFile::copy(i.key(),destination);

              } else{
                  progress->setCopyVisible(true);
                  copyFile(i.key(),destination);

                  //  progress->setCopyValue(100);
              }


              qApp->processEvents();
              if(mStop)break;
          }

          creatSymsLinks();

     }

     progress->close();

}

void FilesActions::creatSymsLinks()
{
 // qDebug()<<"creatSymsLinks"<<m_hash;
  QHashIterator<QString, QString> i(m_hashSymLink);
  while (i.hasNext())
  {
          i.next();
       qDebug()<<"symlink"<<i.key()<< i.value();
      QFile fi(i.value());
      QFileInfo info(i.key());
      QString    destination=i.value();
      if(info.isSymLink()){



          if(fi.exists())
          {


              if(!mApplyAll)
              {
                  destination=confirmName( i.key(), i.value());
                  if(destination.isEmpty())
                      break;
              }

              if(mOverWrite)
                  QFile::remove(destination);
              if(mSkyp)
                  continue;

          }
          QString linkName=info.symLinkTarget();
          QFileInfo symfi(linkName);
          QString symFileName=symfi.fileName();
          QString symAbsoluteDir=info.absolutePath()+"/"+symFileName;

          qDebug()<<"destination :"<<destination;
          qDebug()<<"linkName :"<<linkName;
          qDebug()<<"symAbsoluteDir :"<<symAbsoluteDir;
          qDebug()<<"symFileName :"<<symFileName;

          if(QFile::exists(symAbsoluteDir)){
              QFile::link(symFileName,destination);
          }else{
              qDebug()<<"not exist :"<<linkName;
              QFile::link(linkName,destination);
          }
      }
  }
}

//______________________________________________________________________________________
void FilesActions::slotCancel()
{
     mStop=true;
}

//______________________________________________________________________________________
bool FilesActions::createList(const QStringList &files,const QString &dist)
{
     foreach (QString f, files)
     {
          QFileInfo fi(f);
          if(fi.isSymLink()){
              m_hashSymLink[f]= dist+QDir::separator()+fi.fileName();
          }else
          if(fi.isDir() )
          {
               QString dirName=fi.fileName();
               QString destination=dist+"/"+dirName;
               qDebug()<<f<<"destination<<<<<<<<<<<<<<<<<<<<<<"<<destination;
               if(QFile::exists(destination))
               {

                qDebug()<<"folder exist"<<destination;
                    if(!mApplyAllFolder)
                    {
                         destination=confirmName( f, destination);
                         if(destination.isEmpty())
                              return false;
                    }

                    if(mWriteIntoFolder){

                         createLisdOfDirs(f,destination);
                         continue;
                    }else if(mSkyp){
                         continue;
                    }


               }

               QDir dir(destination);

               dir.mkpath(destination);

               createLisdOfDirs(f,destination);

          }
          else
          {
               m_hash[f]= dist+QDir::separator()+fi.fileName();
               qDebug()<<"file<<<<"<<f<< dist+QDir::separator()+fi.fileName();

          }
     }

     return true;
}

//______________________________________________________________________________________
void FilesActions::createLisdOfDirs(const QString &path,const QString &dist)
{

     QDir dir(path);



     QDirIterator it(path,QDir::AllEntries |
                     QDir::NoDotAndDotDot |QDir::System|
                    QDir::Hidden, QDirIterator::Subdirectories);

     while(it.hasNext())
     {
          if(mStop)return;

          it.next();

          QString filePath=it.filePath();
          QString path=it.path();
          QString pathDest=filePath;
          pathDest.replace(path,dist);
          if(it.fileInfo().isSymLink()){
              m_hashSymLink[filePath]= pathDest;
              qDebug()<<"symLink<<<"<<filePath<< pathDest;
          }else
          if(it.fileInfo().isDir())
          {
               qDebug()<<"mkpath<<<"<<filePath<< pathDest;
               dir.mkpath(pathDest);
          }
          else
          {
               m_hash[filePath]= pathDest;
               qDebug()<<"filePath<<<"<<filePath<< pathDest;
          }
     }
}

//______________________________________________________________________________________
void  FilesActions::moveFiles(const QStringList &files, const QString &distDir)
{
     progress->setCopyVisible(false);
     progress->setWindowTitle(tr("Move Files"));
     progress->show();
     progress->setMaximum(files.count());

     int n=0;
     foreach (QString file, files) {
          n++;
          progress->setValue(n);
          QFileInfo original(file);
          QString fName = original.fileName();
          QString filepath=distDir+"/"+fName;

          if(QFileInfo( filepath ).exists() ){

               if(!mApplyAll)
               {
                    filepath=  confirmName(file,filepath,true);
                    if(filepath.isEmpty())
                         return ;
               }


               if(mSkyp)
                    continue;

          }

          QDir dir;
          if(!dir.rename(file,filepath))
               qDebug()<< tr( "move to %1 failed" ).arg(filepath);


     }
     progress->close();
}

//______________________________________________________________________________________
bool FilesActions::copyFile(const QString &source,const QString &dist)
{
     QFileInfo fi(dist);
     QString destination=dist;
     QFile sourceFile(source);

     if(fi.exists())
     {
          if(!mApplyAll)
          {
               destination=confirmName( source, dist);
               if(destination.isEmpty())
                    return false;
          }

          if(mOverWrite)
               QFile::remove(dist);
          if(mSkyp)
               return true;

     }


     //--------------------------------------------------------------------------

     QFile destFile(destination);

     // Open source and destination files
     sourceFile.open(QFile::ReadOnly);
     destFile.open(QFile::WriteOnly);

     // Determine buffer size, calculate size of file and number of steps
     char block[4096];
     qint64 totalSize = sourceFile.size();
     qDebug()<<totalSize;
     qint64 steps = totalSize >> 7; // shift right 7, same as divide 128, much faster
     qint64 interTotal = 0;
     qint64 mCurentSize=0;

     // Copy blocks
     while (!sourceFile.atEnd())
     {
          if(mStop)break; // cancelled

          qint64 inBytes = sourceFile.read(block, sizeof(block));
          destFile.write(block, inBytes);
          interTotal += inBytes;

          mCurentSize += inBytes;
          //   qDebug()<<"percent1:"<<interTotal * 100 / total;

          if (interTotal > steps)
          {
           //    qDebug()<<"percent:"<<mCurentSize * 100 / totalSize;
               qApp->processEvents();
               progress->setCopyValue(mCurentSize * 100 / totalSize);
               qApp->processEvents();
               interTotal = 0;
          }
     }
    // qDebug()<<"percent:"<<mCurentSize * 100 / totalSize;
     progress->setCopyValue(mCurentSize * 100 / totalSize);
     destFile.close();
     sourceFile.close();

     if (destFile.size() != totalSize) return false;

     return true;



}

//______________________________________________________________________________________
QString FilesActions::confirmName(const QString &source,const QString &dist,bool cut)
{
     QString destination=dist;

     ConfirmDlg *dlg=new ConfirmDlg(source,dist,cut);
     QFileInfo fi(source);
     if(dlg->exec()==QDialog::Accepted)
     {
          ConfirmDlg::Actions action=dlg->getAction();

          if(fi.isDir())
               mApplyAllFolder=dlg->isApplyToAll();
          else
               mApplyAll=dlg->isApplyToAll();

          switch (action) {
               case ConfirmDlg::ReName:
                    destination=dlg->getName();
                    mOverWrite=false;
                    mSkyp=false;
                    mApplyAll=false;
                    mSkypFolder=false;
                    mApplyAllFolder=false;
                    mWriteIntoFolder=false;
                    break;
               case ConfirmDlg::OverWrite:
                    mOverWrite=true;
                    mSkyp=false;
                    break;
               case ConfirmDlg::Skyp:
                    mSkyp=true;
                    mOverWrite=false;
                    break ;
               case ConfirmDlg::WriteIntoFolder:
                    mWriteIntoFolder=true;
                    mSkypFolder=false;
                    break;
               case ConfirmDlg::SkypFolder:
                    mSkypFolder=true;
                    mWriteIntoFolder=false;
                    break ;

               default:
                    break;
          }

     }else{
          destination="";
     }
     delete dlg;
     return destination;
}

//______________________________________________________________________________________
bool FilesActions::cutFile(const QString &source,const QString &dist)
{
     if(copyFile(source,dist)){
          QFile::remove(source);
          return true;
     }


     return false;

}

//______________________________________________________________________________________Trash
bool FilesActions::trashExists()
{
     QString TrashPath;
     QStringList paths;
     const char* xdg_data_home = getenv( "XDG_DATA_HOME" );
     if( xdg_data_home ){
          // qDebug() << "XDG_DATA_HOME not yet tested";
          QString xdgTrash( xdg_data_home );
          paths.append( xdgTrash + "/Trash" );
     }
     QString home =QDir::homePath();
     paths.append( home + "/.local/share/Trash" );
     paths.append( home + "/.trash" );
     foreach( QString path, paths ){
          if( TrashPath.isEmpty() ){
               QDir dir( path );
               if( dir.exists() ){
                    TrashPath = path;
               }
          }
     }
     if( TrashPath.isEmpty() ){

          qDebug()<< "Cant detect trash folder------------------------------------------------------" ;
          return false;
     }
     TrashPathInfo = TrashPath + "/info";
     TrashPathFiles = TrashPath + "/files";
     if( !QDir( TrashPathInfo ).exists() || !QDir( TrashPathFiles ).exists() ){
          qDebug()<<  "Trash doesnt looks like FreeDesktop.org Trash specification----------------------" ;
          return false;
     }
     return true;
}

//______________________________________________________________________________________Trash
void FilesActions::moveFilesToTrash(const QStringList &list)
{
    TrashPathInfo = Edir::trashInfo();
    TrashPathFiles = Edir::trashFiles();

     foreach (QString file, list) {
          moveFileToTrash(file);
     }
}

//______________________________________________________________________________________Trash
void FilesActions::moveFileToTrash(const QString &file)
{


     QFileInfo original(file );
     if( !original.exists() ){
          qDebug()<< tr( "File doesnt exists, cant move to trash" );
          return;
     }
     QString info;
     info += "[Trash Info]\nPath=";
     info += original.absoluteFilePath();
     info += "\nDeletionDate=";
     info += QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzzZ");
     info += "\n";
     QString trashname = original.fileName();
     QString infopath = TrashPathInfo + "/" + trashname + ".trashinfo";
     QString filepath = TrashPathFiles + "/" + trashname;
     int nr = 1;
     while( QFileInfo( infopath ).exists() || QFileInfo( filepath ).exists() ){
          nr++;
          trashname = original.baseName() + "." + QString::number( nr );
          if( !original.completeSuffix().isEmpty() ){
               trashname += QString( "." ) + original.completeSuffix();
          }
          infopath = TrashPathInfo + "/" + trashname + ".trashinfo";
          filepath = TrashPathFiles + "/" + trashname;
     }
     QDir dir;
     if( !dir.rename( original.absoluteFilePath(), filepath ) ){
          qDebug()<< tr( "move to trash failed" )<<original.absoluteFilePath();
     }else{
          qDebug()<< tr( "move to trash succeced" )<<filepath;
          QFile infofile(infopath);
          infofile.open(QIODevice::WriteOnly);
          QTextStream out(&infofile);
          out.setCodec("UTF-8");
          out <<info;


     }

     // infofile.s()reateUtf8( infopath, info );

}
