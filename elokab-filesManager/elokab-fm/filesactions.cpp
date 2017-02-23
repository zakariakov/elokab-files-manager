#include "filesactions.h"
#include "confirmdlg.h"
#include <QFileInfo>
#include <QDirIterator>
#include <QDebug>
#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>

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
          if (txt.length() > 50) txt = "/.../" + txt.split(QDir::separator()).last();

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
                     QDir::NoSymLinks |
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
     progress->setCopyVisible(true);
     progress->setWindowTitle(tr("Copy Files"));
     progress->show();
     qApp->processEvents();
     if(createList(files,distDir))
     {

          progress->setMaximum(m_hash.count());

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

               if(copyFile(i.key(),i.value())==false)
                    break;

               qApp->processEvents();
               if(mStop)break;
          }


     }

     progress->close();

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

          if(fi.isDir())
          {
               QString dirName=fi.fileName();
               QString destination=dist+"/"+dirName;
               qDebug()<<f<<"destination<<<<<<<<<<<<<<<<<<<<<<"<<destination;
               if(QFile::exists(destination))
               {
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
          }
     }

     return true;
}

//______________________________________________________________________________________
void FilesActions::createLisdOfDirs(const QString &path,const QString &dist)
{

     QDir dir(path);



     QDirIterator it(path,QDir::AllEntries |
                     QDir::System |
                     QDir::NoDotAndDotDot |
                     QDir::NoSymLinks |
                     QDir::Hidden, QDirIterator::Subdirectories);

     while(it.hasNext())
     {
          if(mStop)return;

          it.next();

          QString filePath=it.filePath();
          QString path=it.path();
          QString pathDest=filePath;
          pathDest.replace(path,dist);

          if(it.fileInfo().isDir())
          {
               dir.mkpath(pathDest);
          }
          else
          {
               m_hash[filePath]= pathDest;
               qDebug()<<"filePath"<<filePath<< pathDest;
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
               qDebug()<<"percent:"<<mCurentSize * 100 / totalSize;
               qApp->processEvents();
               progress->setCopyValue(mCurentSize * 100 / totalSize);
               qApp->processEvents();
               interTotal = 0;
          }
     }
     qDebug()<<"percent:"<<mCurentSize * 100 / totalSize;
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

