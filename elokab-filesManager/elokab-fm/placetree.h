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
          explicit PlaceTree(QWidget *parent = 0);
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
