#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "tab.h"
#include <ElokabSettings>
#include "actions.h"
#include "pathwidget.h"
#include "fileinformation.h"
//#include <QTermWidget>
#include "placetree.h"
#include "filterbar.h"
#include "settings.h"
#include <QMainWindow>
#include <QWidgetAction>
 #include <QProgressBar>
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
          explicit MainWindow(QWidget *parent = 0);

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
          void mainUrlChanged(QString url);

          //!
          void setSelectedFoldersFiles(QString msg);

          //!
          void calculatFiles();

          //!
          void urlChanged(const QString &url);

          //!
          void stackSetCurrentIndex(bool arg);

          //!
          void setUrl(QModelIndex index);

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
void showProgress(bool arg);
          //!
          void refreshIcons();

          //!
        //  void terminalVisibilityChanged(bool arg);

//          void terminalCustomContextMenu(QPoint);

      //   void terminalPastText();
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
          IconProvider *mIconProvider;

          //!
      //    QFileSystemWatcher *m_fileWatcher;

          //!
          ElokabSettings *mElokabSettings;

          //!
          QProgressBar *mProgressBar;

          //!
//          QTermWidget  *mTermWidget;

//          QAction *actionCopy;
//           QAction *actionPast;
};

#endif // MAINWINDOW_H
