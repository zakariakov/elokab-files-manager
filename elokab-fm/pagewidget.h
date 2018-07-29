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
#include"thumbnails.h"
//#include "trashview.h"
#include "trash.h"
#include "searchview.h"
#include "myfilesystemmodel.h"
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
    explicit PageWidget(MyFileSystemModel *model,
                        Settings *setting,
                        Actions *action,
                        const QString &url,
                        QWidget *parent = nullptr);

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
    //! اشارة بتغير العنصر لتغير المسار
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

    //! تفعيل زر الذهاب للخلف
    void setBackEnabled(bool enabled){mbackEnabled=enabled;}

    //! تفعيل زر الذهاب للامام
    void setForwardEnabled(bool enabled){mForwardEnabled=enabled;}

    //! الذهاب الى المسار
    void setUrl(const QString &url);

    //! غير المسار
    void setUrlChange(const QString &url);

    //! تغير المنظر
    void viewChangged(int index);

    //! تحديق الاشارات
    void updateSignals();

    //! الذهاب للامام
    void goForward();

    //! الذهاب للخلف
    void goBack();

    //! طريقة العرض ايقونات او قياسي او تفصيلي
    void  setViewMode(int mode);

    //! يرجع المسار الحالي
    QString dirPath(){return m_dirPath;}

    //! مسار الملف المحدد
    QString selectedFile();

    //! قائمة بمسارات الملفات المحددة
    QStringList selectedFiles();

    //! قائمة باعناصر المحددة
    QModelIndexList selectedIndex();

    //! تكبير
    void setZoomIn();
    //! تصغير
    void setZoomOut();
    //! تغيير الاسم
    void renameFiles();

    //!
    //  void moveFilesToTrash();

    //! تحديد الكل
    void selectAll();

    //! نافذة الخصائص
    void showProperties();

    //! نلفذة فتح باستخدام
    void showOpenwithDlg(const QString &fileName);

    //! تحديق الايقونات
    void iconUpdate(QModelIndex index);

    //! تحديث المصغرات
    void iconThumbUpdate(const QString &file);

    //! تحديد العنصر بواسطة المسار
    void selectIndex(const QString &file);

    //! حذف الملفات المحددة
    void trashDeleteFiles();


private slots:

    //! تفعيل /تعطيل النقر المزدوج لاعادة التسمية
    void setDoubleClickEdit(bool arg);

    //! عند تغير التحديد
    void selectionHasChanged(const QItemSelection &/*selected*/, const QItemSelection &);

    //! عند تفعيل العنصر
    void slotItemActivated(QModelIndex index);

    //! عند النقر على العنصر
    void slotItemPressed(QModelIndex index);

    //! القائمة الفرعية المنسدلة
    void customContextMenu(QPoint);

    //! الودجت المفعلة
    int focusedWidget();

     //! استرجاع ملفات من سلة المحذوفات
    void trashRestoreFiles();


private:

    Thumbnails *mThumbnails;
    //!
    Settings *mSettings;

    //!
    MyFileSystemModel *myModel ;

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
    // TrashView *trashView;
    //!
    Trash *mTrash;

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

    bool mDoubleClickEdit;

    //!
    PropertiesDlg *propertiesDlg;

    //!
    OpenWithDlg *openWithDlg;

    //!
    enum ViewWidget{
        WListView,
        WTreeView,
        WSearchView,
        WTrashView
    };

    enum ViewMode{
        IconView,
        CompactView,
        DetailView
    };

    //!
    int mViewMode;



};

#endif // PAGEWIDGET_H
