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
class MyFileSystemModel :public QFileSystemModel
{
 Q_OBJECT
public:
    explicit MyFileSystemModel(IconProvider *iconProvider,QObject *parent = 0);
    QIcon iconSymLink(QIcon icon,QSize size);
void loadIcons(QModelIndexList indexes);
void loadImage(QString path) const;
void loadIcon(QFileInfo fi, QString mim) const;
void refreshIcons(const QString &dir);
void clearCache();
void setPreview(bool preview){mPreview=preview;}
signals:
void imageLoaded(QFileInfo minfo);
    void dragDropFiles(bool copy,QString path, QStringList list);
 void iconUpdate(const QModelIndex index)const;

protected:
    QVariant data(const QModelIndex &index, int role) const ;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

 //   int columnCount(const QModelIndex &parent) const;
    bool dropMimeData(const QMimeData * data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      const QModelIndex & parent );

private:
    IconProvider *mIconProvider;
   QHash<QString,QIcon>*iconCach;
      QHash<QString,QIcon>*iconDesktopCach;
   QHash<QString,QByteArray> *hashImages;
   bool mPreview;
};

class DefaultIconProvider : public QFileIconProvider
{
     public:

         virtual  QIcon icon(const QFileInfo &info) const
          {
               if(info.isDir())
                    return QIcon::fromTheme("folder",QIcon::fromTheme("file"));
               else if(info.isExecutable())
                    return QIcon::fromTheme("application-x-executable",QIcon::fromTheme("file"));
               else
                    return QIcon::fromTheme("unknown",QIcon::fromTheme("file"));
          }

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
          void   urlHasChanged(const QString &url);

           //!
          void selectedFoldersFiles(QString);

     public slots:

          void refreshIcons();
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
          void directoryChanged(const QString &dir);
void loadIcons(const QString &currentPath);
void iconUpdate(QModelIndex index);
          //!
          void setLargeDirectory(const QString &dir);

          //!
          void setShowThambnails(bool arg);

          //!
          void setStandardIcons(bool arg);

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
          //!
          DefaultIconProvider *mDefaultprovider;

          //!
          QMimeData *mimData;

          //!
          QStringList listDirectory;

          bool mSowTumbnails;
};

#endif // TAB_H
