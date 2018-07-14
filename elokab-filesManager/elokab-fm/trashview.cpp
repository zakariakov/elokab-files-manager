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

#include "trashview.h"
#include "messages.h"
#include <QDir>
#include <EMimIcon>
#include <QMenu>
#include <QDebug>

#include <QSettings>
#include <QTextCodec>
#include <QApplication>
#include <QByteArray>
#include <QMessageBox>
#include  <QProcess>
#include  <QDateTime>

/*****************************************************************************************************
 *
 *****************************************************************************************************/
TrashView::TrashView(QWidget *parent) :
    QTreeWidget(parent),TrashFilesChanged(0)
{

#ifdef DEBUG_APP
    Messages::showMessage(Messages::TOP,"TrashView::TrashView()");
#endif
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(costumMenu(QPoint)));
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(itemSelectionChanged()));

    setSortingEnabled(true);

  //  mActions=action;

    setColumnCount(4);
    this->headerItem()->setText(0,tr("File"));
    this->headerItem()->setText(1,tr("Origin path"));
    this->headerItem()->setText(2,tr("Deletion date"));
    this->headerItem()->setText(3,tr("Deletion Time"));
    this->headerItem()->setText(4,tr("Type"));
    setIconSize(QSize(22,22));
    if( !TrashFilesChanged ){
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
        if( TrashPath.isEmpty() )
            qDebug()<< "Cant detect trash folder------------------------------------------------------" ;
        TrashPathInfo = TrashPath + "/info";
        TrashPathFiles = TrashPath + "/files";
        if( !QDir( TrashPathInfo ).exists() || !QDir( TrashPathFiles ).exists() )
            qDebug()<<  "Trash doesnt looks like FreeDesktop.org Trash specification----------------------" ;
        TrashFilesChanged = true;
    }
    fsWatcher=new QFileSystemWatcher;
    fsWatcher->addPath(TrashPathFiles);
    //   connect(fsWatcher,SIGNAL(directoryChanged(QString)),this,SLOT(directoryChanged(QString)));


#ifdef DEBUG_APP
    Messages::showMessage(Messages::NORMALE,"TrashView::TrashView()","TrashPath:"+TrashPath);
    Messages::showMessage(Messages::NORMALE,"TrashView::TrashView()","TrashPathInfo:"+TrashPathInfo);
    Messages::showMessage(Messages::NORMALE,"TrashView::TrashView()","TrashPathFiles:"+TrashPathFiles);
    Messages::showMessage(Messages::END,"TrashView::TrashView()");
#endif

}

/**************************************************************************************
 *
 **************************************************************************************/
QString localeType(const QFileInfo &info)
{
    QString mim=EMimIcon::mimeTyppe(info);

    QString mimLang=EMimIcon::mimLang(mim);

    return mimLang;
}

void TrashView::updateTrashFiles()
{
    //  if (TrashFilesChanged==false)return;
    this->clear();
    QDir trashDir(TrashPathFiles);

    foreach (QFileInfo file, trashDir.entryList
             (QDir::NoDotAndDotDot|
              QDir::AllEntries|QDir::Hidden ))
    {
        QString name=file.fileName();
        QString infoname=TrashPathInfo+"/"+name+".trashinfo";

        if(QFile::exists(infoname))
        {
            QStringList list=readDesktopFile(infoname);
            QTreeWidgetItem *item=new QTreeWidgetItem(this);
            item->setText(0,name);
            item->setText(1,list.at(0));
            item->setText(2,list.at(1).section("T",0,0));
            item->setText(3,list.at(1).section("T",1,1));
            QFileInfo fileinfo(TrashPathFiles+"/"+name);
            item->setText(4,localeType(fileinfo));
            item->setIcon(0,EMimIcon::icon(fileinfo));

        }

    }
    this->resizeColumnToContents(0);

    emit fileSelected(tr("%1 file(s) found")
                      .arg(QString::number(topLevelItemCount()))) ;
}


/**************************************************************************************
 *
 **************************************************************************************/
QStringList TrashView::readDesktopFile(const QString &fileName)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QStringList list;
    QSettings setting(fileName,QSettings::IniFormat);
    setting.setIniCodec(QTextCodec::codecForName("UTF-8"));
    setting.beginGroup("Trash Info");
    QString path=setting.value("Path","").toString();
    QString dat=setting.value("DeletionDate","").toString();
    setting.endGroup();


    QByteArray encodedString =QByteArray::fromPercentEncoding(path.toUtf8());

    QString string = codec->toUnicode(encodedString);
    list<<string<<dat;

    return list;

}

/**************************************************************************************
 *
 **************************************************************************************/
void TrashView::directoryChanged(QString)
{
    TrashFilesChanged=true;
    updateTrashFiles();
}

/**************************************************************************************
 *
 **************************************************************************************/
void TrashView::clearTrash()
{
    //fsWatcher->blockSignals(true);

    for (int i = 0; i < topLevelItemCount(); ++i) {

        QTreeWidgetItem *item=topLevelItem(i);
        if(item){
            QString name=item->text(0);
            QString fileName=TrashPathFiles+"/"+name;
            QString infoname=TrashPathInfo+"/"+name+".trashinfo";
            QFileInfo fi(fileName);
            if(fi.isDir())
            {
                QProcess prop;
                prop.start(QString("rm -r \"%1\"").arg(fileName));
                prop.waitForFinished();
                if(!QFile::exists(fileName)){
                    QFile::remove(infoname);
                    //  list.append(i);
                }
            }else{
                if(QFile::remove(fileName))
                {
                    QFile::remove(infoname);
#ifdef DEBUG_APP
                    Messages::showMessage(Messages::NORMALE,"TrashView::clearTrash()","remove:"+fileName);
                    Messages::showMessage(Messages::NORMALE,"TrashView::clearTrash()","infoname:"+infoname);


#endif
                    //   list.append(i);
                }
            }
        }
    }
    this->clear();
    //fsWatcher->blockSignals(false);
    //  TrashFilesChanged=true;
    // updateTrashFiles();
}

/**************************************************************************************
 *
 **************************************************************************************/
void TrashView::deleteFile()
{
    //fsWatcher->blockSignals(true);
    QList<QTreeWidgetItem *>list=this->selectedItems();

    foreach (QTreeWidgetItem *item, list)
    {
        if(item)
        {
            QString name=item->text(0);
            QString fileName=TrashPathFiles+"/"+name;
            QString infoname=TrashPathInfo+"/"+name+".trashinfo";
            QFileInfo fi(fileName);
            if(fi.isDir())
            {
                QProcess prop;
                prop.start(QString("rm -r \"%1\"").arg(fileName));
                qDebug()<<QString("rm -r \"%1\"").arg(fileName);
                prop.waitForFinished();
                if(!QFile::exists(fileName)){
                    QFile::remove(infoname);
                    int indx=indexOfTopLevelItem(item);
                    takeTopLevelItem(indx);
                }
            }else{
                if(QFile::remove(fileName))
                {
                    QFile::remove(infoname);
#ifdef DEBUG_APP
                    Messages::showMessage(Messages::NORMALE,"TrashView::clearTrash()","remove:"+fileName);
                    Messages::showMessage(Messages::NORMALE,"TrashView::clearTrash()","infoname:"+infoname);
#endif
                    int indx=indexOfTopLevelItem(item);
                    takeTopLevelItem(indx);
                }
            }


        }
    }


    //fsWatcher->blockSignals(false);

}
/**************************************************************************************
 *
 **************************************************************************************/
void TrashView::restoreFiles()
{
    // fsWatcher->blockSignals(true);
    QList<QTreeWidgetItem *>list=this->selectedItems();

    foreach (QTreeWidgetItem *item, list)
    {
        if(item)
        {
            QString name=item->text(0);
            QString fileName=TrashPathFiles+"/"+name;
            QString infoname=TrashPathInfo+"/"+name+".trashinfo";
            QString origeFile=item->text(1);

            /*
               if(fi.isDir())
               {
                    if(QFile::exists(origeFile)){
                         QMessageBox::information(0,"error",tr("A file named %1 already exists.")
                                                  .arg(origeFile));
                         return;
                   }
                                   QDir dir(origeFile);
                    dir.cdUp();
                    QString path=dir.absolutePath();
                    QProcess prop;
                    prop.start(QString("mv -v %1 %2").arg(fileName).arg(path));
                     prop.waitForFinished();
                     if(!QFile::exists(fileName)){
                           QFile::remove(infoname);
                           int indx=indexOfTopLevelItem(item);
                           takeTopLevelItem(indx);
                     }

                     qDebug()<<"restored"<<fileName<<path;
               }
               else if (QFile::copy(fileName,origeFile))
               {

                    if(QFile::remove(fileName)){
                    QFile::remove(infoname);
                    int indx=indexOfTopLevelItem(item);
                    takeTopLevelItem(indx);
                    }
               }
               */
            if(QFile::exists(origeFile)){
                QMessageBox::information(0,"error",tr("A file named %1 already exists.")
                                         .arg(origeFile));

            }else{
                QDir dir;
                if( !dir.rename(fileName, origeFile ) ){
                    QFileInfo fi(origeFile);
                    QString t=tr("restor to %1 failed \n please check if the path exist" ).arg(fi.absolutePath());

                    //Messages::showMessage(Messages::NORMALE,"TrashView::clearTrash()",t);

                    QMessageBox::information(0,"error",t);

                }else{
                    QFile::remove(infoname);
                    int indx=indexOfTopLevelItem(item);
                    takeTopLevelItem(indx);
                }
            }
        }
    }

    // fsWatcher->blockSignals(false);

}
/**************************************************************************************
 *
 **************************************************************************************/
void TrashView::costumMenu(QPoint)
{
    if(this->topLevelItemCount()==0)return;
    QMenu menu;
    menu.addAction(EIcon::fromTheme("user-trash",("emptytrash")),
                   tr("Empty trash"),this,SLOT(clearTrash()));

    QTreeWidgetItem *item=currentItem();
    if(item){
        menu.addAction(EIcon::fromTheme("edit-delete"),
                       tr("Delete"),this,SLOT(deleteFile()));
        menu.addAction(tr("Restor"),this,SLOT(restoreFiles()));
        // menu.addAction(tr("Move to trash"),this,SLOT(moveFileToTrash()));
    }
    menu.exec(QCursor::pos());
}

//______________________________________________________________________________________
void TrashView::moveFilesToTrash(const QStringList &list)
{

    foreach (QString file, list) {
        moveFileToTrash(file);
    }
}

//______________________________________________________________________________________
void TrashView::moveFileToTrash(const QString &file)
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

//______________________________________________________________________________________
void TrashView::itemSelectionChanged()
{
    QList< QTreeWidgetItem *>list=selectedItems();
    if(list.count()==0){
        emit fileSelected(tr("%1 file(s) found")
                          .arg(QString::number(topLevelItemCount()))) ;
    }else if(list.count()==1){
        QTreeWidgetItem *item=list.at(0);
        QString name=item->text(0);
        QString fileName=TrashPathFiles+"/"+name;
        emit fileSelected("file://"+fileName);
    }else{
        emit fileSelected(tr("%1 file(s) selected")
                          .arg(list.count())) ;
    }


}
