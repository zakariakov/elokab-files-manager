#include "trash.h"
#include <QFileInfo>
#include <QProcess>
#include <QDebug>
#include <EDir>
#include <EMimIcon>
#include <QMessageBox>
#include <QDateTime>
//#include <QTextCodec>
//#include <QSettings>


Trash::Trash(QObject *parent) : QObject(parent)
{
    TrashPathInfo = Edir::trashInfo();
    TrashPathFiles = Edir::trashFiles();
    qDebug()<<  "Trash files"<<TrashPathFiles ;
    qDebug()<<  "Trash info"<<TrashPathInfo ;

    if( !QDir( TrashPathInfo ).exists() || !QDir( TrashPathFiles ).exists() )
        qDebug()<<  "Trash doesnt looks like FreeDesktop.org Trash specification----------------------" ;

}

//__________________________________________________________________________
void Trash::deleteFiles(const QStringList &list)
{
    foreach (QString file, list) {
        QFileInfo fi(file);
        QString fileName=file;
        QString infoname=TrashPathInfo+"/"+fi.fileName()+".trashinfo";
        if(fi.isDir())
        {
            QProcess prop;
            prop.start(QString("rm -r \"%1\"").arg(fileName));
            qDebug()<<QString("rm -r \"%1\"").arg(fileName);
            prop.waitForFinished();
            if(!QFile::exists(fileName)){
                QFile::remove(infoname);
               }
        }else{
            if(QFile::remove(fileName))
            {
                QFile::remove(infoname);
            }
        }

    }
}
//______________________________________________________________________________________
void Trash::restorFiles(const QStringList &list)
{
    foreach (QString fileName, list) {

        QFileInfo info(fileName);
         QString infoname=TrashPathInfo+"/"+info.fileName()+".trashinfo";
           QString origeFile=EMimIcon::trachInfo(fileName).value("path");
          //TODO replace list to hash
          //if(list.count()<1)return;

         //  QString origeFile=map.value("path");

           if(QFile::exists(origeFile)){
               QMessageBox::information(nullptr,"error",tr("A file named %1 already exists.")
                                        .arg(origeFile));

           }else{
               QDir dir;
               if( !dir.rename(fileName, origeFile ) ){
                   QFileInfo fi(origeFile);
                   QString t=tr("restor to %1 failed \n please check if the path exist" ).arg(fi.absolutePath());
                   QMessageBox::information(nullptr,"error",t);

               }else{
                   QFile::remove(infoname);

               }
           }
    }
}

void Trash::cleanTrash()
{
    qDebug()<<"trash clean"<<TrashPathFiles;
    QDir dir(TrashPathFiles);
    QStringList list=dir.entryList();
    qDebug()<<"trash clean"<<list;
    foreach (QString s, list) {

        QFileInfo fi(TrashPathFiles+"/"+s);
        if(fi.isDir())
        {
            QProcess prop;
            prop.start(QString("rm -r \"%1\"").arg(fi.filePath()));
            prop.waitForFinished();

        }else{
            QFile::remove(fi.filePath());

        }
    }

    QDir dirInfo(TrashPathInfo);
    QStringList listInfo=dirInfo.entryList();
    foreach (QString s, listInfo) {
        QFileInfo fi(TrashPathInfo+"/"+s);
        if(fi.isDir())
        {
            QProcess prop;
            prop.start(QString("rm -r \"%1\"").arg(fi.filePath()));
            prop.waitForFinished();

        }else{
            QFile::remove(fi.filePath());

        }
    }

}

//______________________________________________________________________________________
void Trash::moveFilesToTrash(const QStringList &list)
{

    foreach (QString file, list) {
        moveFileToTrash(file);
    }
}

//______________________________________________________________________________________
void Trash::moveFileToTrash(const QString &file)
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
    info += QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss");
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
        qDebug()<< tr( "move to trash failed" );
    }else{
        qDebug()<< tr( "move to trash succeced" );
        QFile infofile(infopath);
        infofile.open(QIODevice::WriteOnly);
        QTextStream out(&infofile);
        out.setCodec("UTF-8");
        out <<info;


    }

    // infofile.s()reateUtf8( infopath, info );

}


