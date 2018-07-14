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

#ifndef PAGEWIDGET_H
#define PAGEWIDGET_H

#include <QWidget>
#include"actions.h"
#include "trashview.h"
#include "searchview.h"
#include "mylistview.h"
#include "mytreeview.h"
#include "itemdelegate.h"
#include "filesutils/propertiesdlg.h"
#include "filesutils/openwithdlg.h"
#include "settings.h"
#include <QListView>
#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QTreeView>
#include <QStack>
/**
 * @brief The PageWidget class
 */
class PageWidget : public QWidget
{
          Q_OBJECT
     public:
          /**
     * @brief PageWidget
     * @param model
     * @param obj
     * @param url
     * @param parent
     */
          explicit PageWidget(QFileSystemModel *model,
                              Settings *setting,
                              Actions *action,
                              const QString &url,                  
                              QWidget *parent = 0);

          //!
          ~PageWidget();


          //! return curent url path
          QString curentDir(){return m_dirPath;}

          //! return true if goback is enabled
          bool backEnabled(){return mbackEnabled;}

          //! return true if goForward is enabled
          bool forwardEnabled(){return mForwardEnabled;}

          void clearCurentPath(){ mItemDelegate->clearCurentPath(m_dirPath); }


     protected:

     signals:

          //! رسالة الى الحاوي بان المسار بحجم كبير
         // void isLargeDirectory(const QString &url);

          //! رسالة الى الاب بتغير المسار
          void urlChanged(const QString &url);

          //!
        //  void urlBeginChanged(const QString &url);
          //!
          void indexHasChanged(QModelIndex);

          //! رسالة بتوفر عناصر محددة
          void  selectedAvailabe(bool);

          //! رسالة لعرض معلومات عن الملفات المحددة الى النافذة الام
          void selectedFoldersFiles(QString);

          //! رسالة بتوفر التنقل للامام
          void historyForwardAvailable(bool);

          //! رسالة بتوفر التنقل للخلف
          void historyBackAvailable(bool);

     public slots:

          //!
          void closeAll();

          //!
          void setBackEnabled(bool enabled){mbackEnabled=enabled;}

          //!
          void setForwardEnabled(bool enabled){mForwardEnabled=enabled;}

          //!
          void setUrl(const QString &url);

          //!
          void setUrlChange(const QString &url);

          //!
          void viewChangged(int index);

          //!
          void updateSignals();

          //!
          void goForward();

          //!
          void goBack();

          //!
          void  setViewMode(int mode);

          //!
          QString dirPath(){return m_dirPath;}

          //!
          QString selectedFile();

          //!
          QStringList selectedFiles();

          //!
          QModelIndexList selectedIndex();

          //!
          void setZoomIn();
          //!
          void setZoomOut();
          //!
          void renameFiles();

          //!
          void moveFilesToTrash();

          //!
          void selectAll();

          //!
          void showProperties();

          //!
          void showOpenwithDlg(const QString &fileName);
 void iconUpdate(QModelIndex index);




//  void iconThumbUpdate(const QString &fileName);


     private slots:
          //!
          void selectionHasChanged(const QItemSelection &/*selected*/, const QItemSelection &);

          //!
          void slotItemActivated(QModelIndex index);

          //!
          void slotItemPressed(QModelIndex index);

          //!
          void customContextMenu(QPoint);

          //!
          int focusedWidget();


     private:

          //!
          Settings *mSettings;

          //!
          QFileSystemModel *myModel ;

          //!
          Actions *mActions;

          //!
          QItemSelectionModel *listSelectionModel;

          //!
          QStackedWidget *stackedWidget;

          //!
          MyListView *listView;

          //!
          MyTreeView *treeView;

          //!
          TrashView *trashView;

          //!
          SearchView *searchView;

          //!
         ItemDelegate *mItemDelegate;

         //!
         QStack< QString > mHistoryBack;

          //!
          QStack< QString > mHistoryForward;

          //!
          QString m_dirPath;


          //!
          bool mbackEnabled;

          //!
          bool mForwardEnabled;


          //!
          PropertiesDlg *propertiesDlg;

          //!
          OpenWithDlg *openWithDlg;

          //!
          enum ViewWidget{
               WListView,
               WTreeView,
               WTrashView,
               WSearchView
          };

          enum ViewMode{
               IconView,
               CompactView,
               DetailView
          };

          //!
          int mViewMode;

  //: TODO FIX RESTOR HIDEN FILES
bool restorHiden;

};

#endif // PAGEWIDGET_H
