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

#ifndef PLACETREE_H
#define PLACETREE_H

//#include "mycomputer.h"
#include <QTreeWidget>
#include "udiskdevice.h"
/**
 * @brief The PlaceTree class
 */
class PlaceTree : public QTreeWidget
{
          Q_OBJECT
     public:

          /**
          * @brief PlaceTree
          * @param parent
          */
          explicit PlaceTree(QWidget *parent = nullptr);
~PlaceTree();
     signals:

          /**
           * @brief urlPlacesChanged اشارة الى تغير المسار
           */
          void  urlPlacesChanged(const QString &);

          //!
          void slotOpenNewTab(const QString &);

     public slots:
void refreshIcons();
          /**
          * @brief setCurentUrl جلب المسار الحالي
          * @param url المسار الحالي
          */
          void setCurentUrl(const QString &url);

          /**
           * @brief addNewBookmark اضافة المسار الحالي الى المفضلة
          */
          void addNewBookmark();

private:

          /*!< cmment */
          QTreeWidgetItem *mPlacesItem;

           QTreeWidgetItem  *itemRoot;

           QTreeWidgetItem  *itemTrashFolder;

           QTreeWidgetItem  *itemDesktopFolder;

            QTreeWidgetItem  *itemHomeFolder;

          /*!< cmment */
          QTreeWidgetItem *mBookmarksItem;

          /*!< cmment */
          QTreeWidgetItem *mDivecesItem;

          /*!< cmment */
          QString m_dirPath;

          /*!< cmment */
//          MyComputer *myComputer;

          //!
          QHash<QString,QTreeWidgetItem *>mHashItems;

          QList<QTreeWidgetItem *>mListMountedItems;

          //!new=================
           QList<Device*> mListDevice;

           UdiskDevice *mUdiskDevice;

private slots:
           //!new=================


          //!
          void  addBookmark(const QString &path);

          //! chargeFoldersPlaces تحميل مجلدات المنزل
          void  chargeFoldersPlaces();

          //!
          void chargeBookmarks();

          //!
          void mountDisck();

           //!New
          void updatDiskItems();
          //!
          void  addDiskItem(Device *D);

          //!
          void removeDiskItem(QString s);

          //!
          void updateHashItem(QTreeWidgetItem* item);

          //! القائمة المنسدلة للاوامر
          void customContextMenu(QPoint);

          //!
          void showProperties();

          /**
          * @brief itemPlacesClicked عند تغعيل عنصر الشجرة سيتم ارسال اشارة الى فتح المجلد
          * @param item العنصر المحدد
          */
          void itemPlacesClicked(QTreeWidgetItem* item,int);
void mountItem(QTreeWidgetItem* item,const QString &url);
          //!
          QString openDeviceUrl(QTreeWidgetItem* item);

          /*!< حذف المفصلة الحالية */
          void removeCurentBookmark();

          /*!< تحرير المفضلة الحالية */
          void editCurentBookmark();

          /*!< الفتح في لسان جديد */
          void openInNewTab();

          //!
          void saveBookmarks();



};

#endif // PLACETREE_H
