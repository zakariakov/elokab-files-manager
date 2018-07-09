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

#ifndef TAB_H
#define TAB_H

#include "pagewidget.h"
#include <QWidget>
#include "iconprovider.h"
//#include "iconproviderdefault.h"
#include "settings.h"
#include <QTabWidget>
#include <QMimeData>
#include <QUrl>
#include <QDebug>
#include <QApplication>
#include <QFileSystemWatcher>
#include <QFutureWatcher>
/*

class Thumbnail : public QObject
{
    Q_OBJECT
public:
    Thumbnail(QFileInfo fi,bool preview,QModelIndex index, QObject *parent = 0);
 ~Thumbnail();
    QIcon icon(){return m_icon;}

//    QRectF boundingRect() const{}
//    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*){}

signals:
    void imageReady(QIcon icon);
    void imageFiniched(QModelIndex index);
protected slots:

    void on_imageReady(QIcon icon);
    void on_render_finished();
public slots:

    void startRender();
    void cancelRender();

private:
     QFutureWatcher< void >* m_render;


QModelIndex m_index;
QFileInfo m_info;
QIcon m_icon;
bool mPreview;

        void render();

};

*/

class MyFileSystemModel :public QFileSystemModel
{
 Q_OBJECT
public:
    explicit MyFileSystemModel(IconProvider *iconProvider,QObject *parent = 0);
  //  QIcon iconSymLink(QIcon icon,QSize size);
//void loadIcons(QModelIndexList indexes);
//void loadImage(QString path) const;
//void loadIcon(QFileInfo fi, QString mim) const;
//void geticon(QFileInfo fi)const;
 //void clearCache();
 //QIcon imageThumbnail(QModelIndex index)const;

//void refreshIcons(const QString &dir);

void setPreview(bool preview){mPreview=preview;}
void clearCache(const QString &path);


signals:
 //void imageLoaded(QFileInfo minfo);
 void dragDropFiles(bool copy,QString path, QStringList list);
 //void iconUpdate(const QModelIndex index)const;

protected:

   QVariant data(const QModelIndex &index, int role) const ;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

 //   int columnCount(const QModelIndex &parent) const;
    bool dropMimeData(const QMimeData * data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      const QModelIndex & parent );

public slots:

//    void updateIcons(const QString &file){
//        if(index(file).isValid())
//          iconUpdate(index(file));
//    }
private:

    IconProvider *mIconProvider;
   bool mPreview;
//  QHash<QString,QModelIndex> *hashIndex;
   //QHash< QModelIndex,Thumbnail* > *mThumbnails;
     QHash<QString,QString>*mimcach;
   //   QHash<QString,QIcon>*iconmimCach;
   //      QHash<QString,QIcon>*iconDesktopCach;
   //   QHash<QString,QByteArray> *hashImages;


};


/**
 * @brief The Tab class السنة البرنامج
 */
class Tab : public QTabWidget
{
          Q_OBJECT
     public:

          /**
          * @brief Tab
          * @param iconProvider
          * @param actions
          * @param parent
          */
          explicit Tab(Settings *setting,IconProvider *iconProvider,
                       Actions *actions, QWidget *parent = 0);
          ~Tab();

     signals:
  void  largeDirectoryChanged(bool arg);
           //!
          void   urlChanged(const QString &url);
void  tabAdded(const QString &url);
           //!
          void selectedFoldersFiles(QString);

     public slots:

          void updateIcons();
          void closeAll();

          //!
          void addNewTab( const QString &url=QDir::homePath());

          //!
          void setUrl(const QString &url=QDir::homePath());

          //!
          void setModelFiltrer(const QString& nf);
//!


     private slots:

          //!
          void createThumbnail(const QString &dir);
          void saveImageThumb(const QFileInfo &fi) ;
          //void dataFileChanged(const QString &dir);
//void loadIcons(const QString &currentPath);
//void iconUpdate(QModelIndex index);
          //!
         // void setLargeDirectory(const QString &dir);

          //!
          void setShowThumbnails(bool arg);


          //!
          void closeCurentTab();

          //!
          void oncloseRequested(int index);

          //!
          void currentTabChanged(int index);

          //!
          void setCurTabText(const QString &title);

          //!
          void goForward();

          //!
          void goBack();

          //!
          void goUp();

          //!
          void goHome();

          //!
          void goTrash();

          //!
          void goSearch();

          //!
          void  setViewMode(int);

          //!
          void setHiddenFile(bool visible);

          //!
          void setSorting();

          //!
          void renameFiles();

          //!
          void deleteFiles();

          //!
          void pastFiles();

          //!
          void copyFiles();

          //!
          void cutFiles();

          //!
          void moveFilesToTrash();

          //!
          void selectAll();

          //!
          void slotShowOpenwithDlg();

          //!
          void slotShowProperties();

          void dragDropFiles(bool copy,QString path, QStringList list);
     private:

          /*!< cmment */
          Settings *mSettings;

          /*!< cmment */
          IconProvider *mIconProvider;

          /*!< cmment */
          Actions *mActions;

          /*!< cmment */
          bool mCute;

          /*!< cmment */
          PageWidget *pageWidget;

          /*!< cmment */
         MyFileSystemModel *myModel;
        //    QFileSystemModel *myModel;
          //!

          //!
          QMimeData *mimData;

          //!
          QStringList listDirectory;

          bool mSowTumbnails;

          QFileSystemWatcher *mFileSystemWatcher;
};

#endif // TAB_H
