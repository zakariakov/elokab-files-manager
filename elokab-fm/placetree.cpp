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

#include "placetree.h"
#include "messages.h"
#include "defines.h"

#include <EMimIcon>
#include "filesutils/propertiesdlg.h"
//#include <EIcon>
#include  <QApplication>
#include  <QMenu>
#include  <QDir>
#include  <QDebug>
#include  <QSettings>
#include  <QTimer>

/*****************************************************************************************************
 *
 *****************************************************************************************************/
PlaceTree::PlaceTree(QWidget *parent) :
     QTreeWidget(parent)
{
    Messages::debugMe(0,__LINE__,"PlaceTree",__FUNCTION__);

     setObjectName("PlaceTree");
     setHeaderHidden(true);
     setIconSize(QSize(16,16));
         setContextMenuPolicy(Qt::CustomContextMenu);
     setStyleSheet("QTreeView{"
                   "background-color: transparent;"
                   "border-color: palette(Window);"
                   "}");
     setFrameShape(QFrame::NoFrame);
     setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
//     myComputer=new MyComputer;
///New===============================
     mUdiskDevice=new UdiskDevice;


     connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));
     connect(this,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(itemPlacesClicked(QTreeWidgetItem*,int)));
     connect(this,SIGNAL(itemActivated(QTreeWidgetItem*,int)),this,SLOT(itemPlacesClicked(QTreeWidgetItem*,int)));

//     connect(myComputer,SIGNAL(slotAddItemDisk(QTreeWidgetItem*)),this,SLOT(addDiskItem(QTreeWidgetItem*)));
//     connect(myComputer,SIGNAL(slotRemoveItemDisk(QTreeWidgetItem*)),this,SLOT(removeDiskItem(QTreeWidgetItem*)));
//     connect(myComputer,SIGNAL(updateDeviceItem(QTreeWidgetItem*)),this,SLOT(updateHashItem(QTreeWidgetItem*)));
//     connect(myComputer,SIGNAL(deviceMounted(QTreeWidgetItem*,QString)),this,SLOT(mountItem(QTreeWidgetItem*,QString)));
     connect(mUdiskDevice,SIGNAL(deviceAdded(Device*)),this,SLOT(addDiskItem(Device*)));
      connect(mUdiskDevice,SIGNAL(deviceRemoved(QString)),this,SLOT(removeDiskItem(QString)));
     //--------------------
     mPlacesItem=new QTreeWidgetItem(this);
     mBookmarksItem=new QTreeWidgetItem(this);
     mDivecesItem=new QTreeWidgetItem(this);
     //-------------------
     QFont fontB;
     fontB.setBold(true);
     //-----------------Places--------------------------
     mPlacesItem->setText(0,tr("Places"));
     mPlacesItem->setIcon(0,EIcon::fromTheme("folder"));
     mPlacesItem->setFont(0,fontB);
     mPlacesItem->setData(0,Qt::UserRole,"Places");
     //-----------------Bookmarks--------------------------
     mBookmarksItem->setText(0,tr("Bookmarks"));
     mBookmarksItem->setIcon(0,EIcon::fromTheme("folder-bookmarks",("bookmarks")));
     mBookmarksItem->setFont(0,fontB);
     mBookmarksItem->setData(0,Qt::UserRole,"Bookmarks");
     //-----------------Devices--------------------------
     mDivecesItem->setText(0,tr("Devices"));
     mDivecesItem->setIcon(0,EIcon::fromTheme("drive-harddisk",("computer")));
     mDivecesItem->setFont(0,fontB);
     mDivecesItem->setData(0,Qt::UserRole,"Devices");
     //-----------------chargements----------------------
     chargeFoldersPlaces();
    // myComputer->chargeDevices();
mUdiskDevice->UpdateDevices();
//-----------------Expending All--------------------

     expandAll();
  Messages::debugMe(0,__LINE__,"PlaceTree",__FUNCTION__,"end");
}
PlaceTree::~PlaceTree()
{

  //  myComputer->blockSignals(true);
  delete mUdiskDevice;
  delete  mDivecesItem;
}

/**************************************************************************************
 *                                  FOLDERS
 **************************************************************************************/
void PlaceTree::chargeFoldersPlaces()
{
      Messages::debugMe(0,__LINE__,"PlaceTree",__FUNCTION__);

    QString homeDir=QDir::homePath();
    QString DesktopDir=Edir::desktopDir();
    QString MusicDir=Edir::musicDir();
    QString PicturesDir=Edir::picturesDir();
    QString DocumentsDir=Edir::documentsDir();
    QString MoviesDir=Edir::moviesDir();

   // qDebug()<<"MoviesDir>>>>>===========>>>>"<<MoviesDir;

    QDir dir;
    //HOME
    dir.setPath(homeDir);
    itemHomeFolder=new QTreeWidgetItem(mPlacesItem);
    itemHomeFolder->setText(0,tr("Home"));
    itemHomeFolder->setData(0,Qt::ToolTipRole,homeDir);
    itemHomeFolder->setIcon(0,EIcon::fromTheme("folder-home",("user-home")));
    mHashItems[homeDir]=itemHomeFolder;
     Messages::debugMe(2,__LINE__,"found",dir.dirName());
    //مجلد سطح المكتب
    dir.setPath(DesktopDir);
    if(dir.exists()&& !DesktopDir.isEmpty() && DesktopDir!=homeDir){
        itemDesktopFolder=new QTreeWidgetItem(mPlacesItem);
        itemDesktopFolder->setText(0,dir.dirName());
        itemDesktopFolder->setData(0,Qt::ToolTipRole,DesktopDir);
        itemDesktopFolder->setIcon(0,EIcon::fromTheme("folder-desktop",("user-desktop")));
        mHashItems[DesktopDir]=itemDesktopFolder;
      Messages::debugMe(2,__LINE__,"found",dir.dirName());
    }

    //مجلد الصوت
    dir.setPath(MusicDir);
    if(dir.exists()&& !MusicDir.isEmpty() && MusicDir!=homeDir){
        QTreeWidgetItem  *itemMusicFolder=new QTreeWidgetItem(mPlacesItem);
        itemMusicFolder->setText(0,dir.dirName());
        itemMusicFolder->setData(0,Qt::ToolTipRole,MusicDir);
        itemMusicFolder->setIcon(0,QIcon::fromTheme("folder-music",QIcon::fromTheme("folder-sound")));
        mHashItems[MusicDir]=itemMusicFolder;
        Messages::debugMe(2,__LINE__,"found",dir.dirName());
    }
    //مجلد الصور
    dir.setPath(PicturesDir);
    if(dir.exists()&& !PicturesDir.isEmpty() && PicturesDir!=homeDir){
        QTreeWidgetItem  *itemPicturesFolder=new QTreeWidgetItem(mPlacesItem);
        itemPicturesFolder->setText(0,dir.dirName());
        itemPicturesFolder->setData(0,Qt::ToolTipRole,PicturesDir);
        itemPicturesFolder->setIcon(0,QIcon::fromTheme("folder-images",QIcon::fromTheme("folder-image")));
        mHashItems[PicturesDir]=itemPicturesFolder;
        Messages::debugMe(2,__LINE__,"found",dir.dirName());
    }

    //مجلد المستندات
     dir.setPath(DocumentsDir);
    if(dir.exists()&& !DocumentsDir.isEmpty() && DocumentsDir!=homeDir){

        QTreeWidgetItem  *itemDocumentsFolder=new QTreeWidgetItem(mPlacesItem);
        itemDocumentsFolder->setText(0,dir.dirName());
        itemDocumentsFolder->setData(0,Qt::ToolTipRole,DocumentsDir);
        itemDocumentsFolder->setIcon(0,QIcon::fromTheme("folder-documents",QIcon::fromTheme("folder_documents")));
        mHashItems[DocumentsDir]=itemDocumentsFolder;
        Messages::debugMe(2,__LINE__,"found",dir.dirName());

    }

    //    مجلد الفيديو
    dir.setPath(MoviesDir);
    if(dir.exists()&& !MoviesDir.isEmpty() && MoviesDir!=homeDir){
        QTreeWidgetItem  *itemMoviesFolder=new QTreeWidgetItem(mPlacesItem);
        itemMoviesFolder->setText(0,dir.dirName());
        itemMoviesFolder->setData(0,Qt::ToolTipRole,MoviesDir);
        itemMoviesFolder->setIcon(0,QIcon::fromTheme("folder-video",QIcon::fromTheme("folder-videos")));
        mHashItems[MoviesDir]=itemMoviesFolder;
        Messages::debugMe(2,__LINE__,"found",dir.dirName());
    }

    //ROOT
    dir.setPath(QDir::rootPath());
    itemRoot=new QTreeWidgetItem(mPlacesItem);
    itemRoot->setText(0,tr("Root"));
    itemRoot->setData(0,Qt::ToolTipRole,QDir::rootPath());
    //   QIcon icon=   EMimIcon::iconColorized(EIcon::fromTheme("folder"),QColor(255,0,0,150));
    itemRoot->setIcon(0,EMimIcon::iconFolder(QDir::rootPath()));
    mHashItems[QDir::rootPath()]=itemRoot;   //QDir::rootPath()

    //TRASH
    itemTrashFolder=new QTreeWidgetItem(mPlacesItem);
    itemTrashFolder->setText(0,tr("Trash"));
    itemTrashFolder->setData(0,Qt::ToolTipRole,D_TRASH);
    itemTrashFolder->setIcon(0,EIcon::fromTheme("user-trash","emptytrash"));
    mHashItems[D_TRASH]=itemTrashFolder;
    //--------------------------------------------------------------------------------------
    QSettings setting(QApplication::organizationName(),QApplication::applicationName());

    int count = setting.beginReadArray("Boukmarks");
    if(count==0){
        //        addBookmark(MusicDir);
        //        addBookmark(PicturesDir);
        //        addBookmark(DocumentsDir);
        //       return;
    }

    for (int i = 0; i < count; ++i){

        setting.setArrayIndex(i);

        QString   file = setting.value("file", "").toString();

        if (! file.isEmpty()){
            addBookmark(file);
        }

    } // for

    setting.endArray();

  Messages::debugMe(0,__LINE__,"PlaceTree",__FUNCTION__,"end");
}
void PlaceTree::refreshIcons()
{
    mPlacesItem->setIcon(0,EIcon::fromTheme("folder"));

    //   QIcon icon=   EMimIcon::iconColorized(EIcon::fromTheme("folder"),QColor(255,0,0,150));

    itemRoot->setIcon(0,EMimIcon::iconFolder(QDir::rootPath()));

    itemTrashFolder->setIcon(0,EIcon::fromTheme("user-trash","emptytrash"));

    itemDesktopFolder->setIcon(0,EIcon::fromTheme("user-desktop","folder-desktop"));

    itemHomeFolder->setIcon(0,EIcon::fromTheme("user-home","folder-home"));

    mBookmarksItem->setIcon(0,EIcon::fromTheme("bookmarks","folder-bookmarks"));

    mDivecesItem->setIcon(0,EIcon::fromTheme("drive-harddisk",("computer")));

    for (int i = 0; i < mBookmarksItem->childCount(); ++i) {
        QTreeWidgetItem  *itemBookMark=mBookmarksItem->child(i);

        QString  path = itemBookMark->data(0,Qt::ToolTipRole).toString();
        itemBookMark->setIcon(0,EMimIcon::iconFolder(path));
    }

}

/**************************************************************************************
 *                                  DEVICE
 **************************************************************************************/
void PlaceTree::updatDiskItems()
{
/*
//    foreach (Device *D, mListDevice) {
//        delete  D;
//    }
mUdiskDevice->blockSignals(true);
    mListDevice.clear();
    for (int i = 0; i < mDivecesItem->childCount(); ++i) {
        mDivecesItem->takeChild(i);
    }
mHashItems.clear();
mListMountedItems.clear();

        if (mUdiskDevice->UpdateDevices()){
               qDebug()<<"HERE=========================2===========================;";
             mListDevice=mUdiskDevice->listDevice();

         foreach (Device *D, mListDevice) {

                      addDiskItem(D);
         }

      }
     mUdiskDevice->blockSignals(false);*/
}


/**************************************************************************************
 *                                  DEVICE
 **************************************************************************************/
void PlaceTree::addDiskItem(Device *D)
{
    Messages::debugMe(0,__LINE__,"PlaceTree",__FUNCTION__,D->devPath());
    QTreeWidgetItem *item=new QTreeWidgetItem();
    item->setText(0,D->label());
    item->setData(0,Qt::ToolTipRole,D->mountPath());
    item->setData(0,Qt::UserRole,D->devPath());
   // item->setData(1,Qt::UserRole,D->mountPath());

    item->setIcon(0,QIcon::fromTheme(D->iconName(),QIcon("drive-harddisk")));

     mDivecesItem->addChild(item);

     mHashItems[item->data(0,Qt::ToolTipRole).toString()]=item;

}

/**************************************************************************************
 *                                  DEVICE
 **************************************************************************************/
void PlaceTree::removeDiskItem(QString s)
{
      Messages::debugMe(0,__LINE__,"PlaceTree",__FUNCTION__);

     QTreeWidgetItem *itemDev=nullptr;
    for (int i = 0; i < mDivecesItem->childCount(); ++i) {
        QTreeWidgetItem *item=mDivecesItem->child(i);
        QString devStr=item->data(0,Qt::UserRole).toString();
        if (devStr==s){
            itemDev=item;
            break;
        }
    }

     int indx=mDivecesItem->indexOfChild(itemDev);
     if(indx!=-1){
          mHashItems.remove(itemDev->data(0,Qt::ToolTipRole).toString());
          mDivecesItem->takeChild(indx);
     }


}

void PlaceTree::updateHashItem(QTreeWidgetItem* item)
{
     QString key=   mHashItems.key(item);
     mHashItems.remove(key);
     if(item->data(0,Qt::ToolTipRole).toString()!="/")
          mHashItems[item->data(0,Qt::ToolTipRole).toString()]=item;

}

/**************************************************************************************
 *                                  BOOKMARKS
 **************************************************************************************/
void PlaceTree::chargeBookmarks()
{

     //add her save settings bookmarks
}

/**************************************************************************************
 *                                  BOOKMARKS
 **************************************************************************************/
void PlaceTree::addNewBookmark()
{
      Messages::debugMe(0,__LINE__,"PlaceTree",__FUNCTION__,m_dirPath);
     addBookmark(m_dirPath);

     saveBookmarks();

}

/**************************************************************************************
 *                                  BOOKMARKS
 **************************************************************************************/
void PlaceTree::addBookmark(const QString &path)
{

     QDir dir(path);
     QString name=dir.dirName();
     if(name.isEmpty())name="/";
     QTreeWidgetItem  *itemBookMark=new QTreeWidgetItem(mBookmarksItem);
     itemBookMark->setText(0,name);
     itemBookMark->setIcon(0,EMimIcon::iconFolder(path));
     itemBookMark->setData(0,Qt::ToolTipRole,path);
     mHashItems[path]=itemBookMark;


}

/**************************************************************************************
 *                                  BOOKMARKS
 **************************************************************************************/
void PlaceTree::editCurentBookmark()
{
     QTreeWidgetItem *item=currentItem();
     if(item)
          qDebug()<<"edit bookmark"<<item->data(0,Qt::ToolTipRole).toString();
     //add her save settings bookmarks
}

/**************************************************************************************
 *                                  BOOKMARKS
 **************************************************************************************/
void PlaceTree::removeCurentBookmark()
{
     QTreeWidgetItem *item=currentItem();
     if(item){
          if(item->parent()){
               int index=item->parent()->indexOfChild(item);

               mHashItems.remove(item->data(0,Qt::ToolTipRole).toString());
               item->parent()->takeChild(index);
               saveBookmarks();
          }


     }

}

/**************************************************************************************
 *                                  BOOKMARKS
 **************************************************************************************/
void PlaceTree::saveBookmarks()
{

     QSettings settings(QApplication::organizationName(),QApplication::applicationName());
     settings.remove("Boukmarks");
     settings.beginWriteArray("Boukmarks");

     for (int i = 0; i < mBookmarksItem->childCount(); ++i) {
          settings.setArrayIndex(i);
          QTreeWidgetItem  *itemBookMark=mBookmarksItem->child(i);
          settings.setValue("file",itemBookMark->data(0,Qt::ToolTipRole));
     }

     settings.endArray();
}

/**************************************************************************************
 *                                  NAVIGATION
 **************************************************************************************/
void PlaceTree::itemPlacesClicked(QTreeWidgetItem* item,int)
{
     if(!item->parent())return;

     QString url=item->data(0,Qt::ToolTipRole).toString();

     //اذا كان العنصر عبارة عن جهاز قابل للضم والفصل سيتم صمه اولا
     if(item->parent()->data(0,Qt::UserRole)=="Devices")
     {
          if(url.isEmpty()||!QFile::exists(url)){
               url=  openDeviceUrl( item);
          }
          if(item->data(0,Qt::ToolTipRole).toString()!="/")
               mHashItems[url]=item;
     }

      if(QFile::exists(url)||url==D_TRASH)
          emit urlPlacesChanged(url);

}
void PlaceTree::mountItem(QTreeWidgetItem* item,const QString &url)
{
    if(mListMountedItems.contains(item)){
        if(QFile::exists(url))
          emit urlPlacesChanged(url);
        mListMountedItems.removeOne(item);
    }
}

/**************************************************************************************
 *                                  NAVIGATION
 **************************************************************************************/
QString PlaceTree::openDeviceUrl(QTreeWidgetItem* item)
{

    QApplication::setOverrideCursor(Qt::WaitCursor) ;
    QString url=item->data(0,Qt::ToolTipRole).toString();

    ///New============================
    QString  devpath=item->data(0,Qt::UserRole).toString();
      bool ism=mUdiskDevice->isMounted(devpath);


    if(!ism){
         if (mUdiskDevice->mount(devpath))
              qApp->processEvents();
              url=mUdiskDevice->mountPoint(devpath);
            // item->setData(1,Qt::UserRole,url);
             item->setData(0,Qt::ToolTipRole,url);
             QApplication::restoreOverrideCursor();
  mListMountedItems.append(item);
     }


//    if  (myComputer->mountDevices(item))
//    {
//
//       // qApp->processEvents();
//        url=myComputer->getDevicePath(item);
//        QApplication::restoreOverrideCursor();


//    }


    m_dirPath=url;
    emit urlPlacesChanged(url);

     Messages::debugMe(0,__LINE__,"PlaceTree",__FUNCTION__,url);
    QApplication::restoreOverrideCursor();
    return url;

}



/**************************************************************************************
*                                  MENUS
***************************************************************************************/
void PlaceTree::customContextMenu(QPoint)
{
     QTreeWidgetItem *item=currentItem();
     QMenu menu;

     menu.addAction(EIcon::fromTheme("list-add"),tr("Add Bookmark"),this,SLOT(addNewBookmark()));

     if(item)
     {
          if(item->parent())
          {
               if(item->parent()->data(0,Qt::UserRole)=="Bookmarks")
               {
                    //  menu.addAction(QIcon::fromTheme("document-edit"),tr("Edit Bookmark"),this,SLOT(editCurentBookmark()));
                    menu.addSeparator();
                    menu.addAction(EIcon::fromTheme("list-remove"),tr("Remove Bookmark"),this,SLOT(removeCurentBookmark()));

               }

               if(item->parent()->data(0,Qt::UserRole)=="Devices")
               {
                    menu.addSeparator();
                    bool ismouted=mUdiskDevice->isMounted(item->data(0,Qt::UserRole).toString());
                    QAction *a;

                    a=    menu.addAction(item->icon(0),tr("Mount"),this,SLOT(mountDisck()));

                    a->setEnabled(!ismouted);
                    a=    menu.addAction(EIcon::fromTheme("media-eject" )
                                         ,tr("UnMount"),this,SLOT(mountDisck()));

                    a->setEnabled(ismouted);
                    menu.addSeparator();
               }

               menu.addAction(EIcon::fromTheme("tab-new"),tr("open in new tab"),this,SLOT(openInNewTab()));
               menu.addSeparator();

               QIcon icon=(EIcon::fromTheme("configure"));

               menu.addAction(icon,tr("Properties"),this,SLOT(showProperties()));

          }
     }
     menu.exec(QCursor::pos());

}

/**************************************************************************************
 *
 **************************************************************************************/
void PlaceTree::showProperties()
{
     QStringList list;
     QTreeWidgetItem *item=currentItem();
     if(!item)
          return;

     list<<item->data(0,Qt::ToolTipRole).toString();

     PropertiesDlg *propertiesDlg=new PropertiesDlg(list,this);

     propertiesDlg->show();

}

void PlaceTree::mountDisck()
{

     QTreeWidgetItem *item=currentItem();
     if(!item)return;
     QString device=item->data(0,Qt::UserRole).toString();
     if(!mUdiskDevice->isMounted(device))
         openDeviceUrl(item);
     else{
          mUdiskDevice->unmount(device);
          m_dirPath=QDir::homePath();
          emit urlPlacesChanged(QDir::homePath());
     }
}

void PlaceTree::openInNewTab()
{
     QTreeWidgetItem *item=currentItem();
     if(!item)return;
     emit slotOpenNewTab(item->data(0,Qt::ToolTipRole).toString());
}

void PlaceTree::setCurentUrl(const QString &url)
{
     m_dirPath=url;

     QDir dir(url);
     do{
          QString path=dir.absolutePath();
          if(mHashItems.contains(path)){
             setCurrentItem(mHashItems[path]);
               break;
          }
     }while (dir.cdUp());

}
