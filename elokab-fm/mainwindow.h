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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "tab.h"
#include <ElokabSettings>
#include "actions.h"
#include "pathwidget.h"
#include "fileinformation.h"
#include "settingsdlg.h"
//#include <QTermWidget>
#include "placetree.h"
#include "filterbar.h"
#include "settings.h"
#include <QMainWindow>
#include <QWidgetAction>
 #include <QProgressBar>
 #include <QFileIconProvider>

class MyIconProvider:public QFileIconProvider
{
public:

    MyIconProvider(){}

   // virtual  QIcon icon(IconType type) const;

    virtual  QIcon icon(const QFileInfo &info) const ;

  //  virtual  QString type(const QFileInfo &info)const ;

};
/**
 *
 */
namespace Ui {
class MainWindow;
}

/**
 * @brief The MainWindow class
 */
class MainWindow : public QMainWindow
{
          Q_OBJECT

     public:

         //!
          explicit MainWindow(QString pathUrl=QString(), QWidget *parent = nullptr);

           //!
          ~MainWindow();

           //!
          void closeEvent(QCloseEvent *);



     signals:

          //!
          void closeAll();

     private slots:

          //!
          void loadIconThems();

          //!
          void toolCustomContextMenu(QPoint);


          //!
          void setSelectedFoldersFiles(QString msg);

          //!
          void calculatFiles();

          //!
         // void urlChanged(const QString &url);

          //!
          void stackSetCurrentIndex(bool arg);

          //!
          void setUrl(QModelIndex index);

          //!
          void setUrl(QString url);

         //!
          void showHidFilterBar();

          //!
          void showConfigureToolBar();

          //!
          void setupToolBarActions();

          //!
          void changeSingleClick();

          //!
          void showAboutThis();

          //!
          void showHelp();

          //!
          void refreshIcons();

          //!
          void showSettings();

     private:
          Ui::MainWindow *ui;

          /*!< cmment */
          Tab *mTab;

          /*!< cmment */
          QFileSystemModel  *myModel;

          /*!< cmment */
          Actions *mActions;

          /*!< cmment */
          QString m_mainUrl;

          /*!< cmment */
          PathWidget *pathWidget;

          /*!< cmment */
          PlaceTree *placesTree;

          /*!< cmment */
          FilterBar *filterBar;

          /*!< cmment */
          Settings *mSettings;

          /*!< cmment */
          FileInformation *mFileInfo;

          /*!< cmment */
          MyIconProvider *mIconProvider;

          //!
      //    QFileSystemWatcher *m_fileWatcher;

          //!
          ElokabSettings *mElokabSettings;



};

#endif // MAINWINDOW_H
