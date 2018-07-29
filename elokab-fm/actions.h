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

#ifndef ACTONS_H
#define ACTONS_H
#include "filesutils/dialogxdesktop.h"
#include "settings.h"
#include <QObject>
#include <QMenu>
#include <EMimIcon>

#include <QWidgetAction>

class Actions : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Actions
     * @param lc اللغة
     * @param parent
     */
    explicit Actions(Settings *setting,const QString &lc,QObject *parent = nullptr);

    //!
    enum ViewMode{
        IconView,
        CompactView,
        DetailView
    };

    //!
    enum SortBy{
        sortName=0,
        sortSize=1,
        sortType=2
    };


   //!
    QMenu   *menuService(const QStringList &files, const QString &mim);

     //!
    QAction *extractHereAction(const QString &file);

     //!
    QAction *AddArchiveAction(const QStringList &files);

     //!
    QAction *folderPlaceAction(){return actPlacesFolder;}

     //!
    QAction *quitAction(){return actQuit;}

     //!
    QWidgetAction *pathWidgetAction(){return actPathWidget;}

     //!
    QAction *configToolAction(){return actConfigTool;}

     //!
   // QAction *anewTabAction(){return actNewTab;}

     //!
    QAction *pasteAction(){return actPaste;}

     //!
    QAction *openTerminalAction(){ return actOpenTerminal;}

     //!
    QAction *openInNewTabAction(){ return actOpenInNewTab;}

    //!
    QAction *showSettingsAction(){return actShowSettings;}

     //!
    QAction *copyAction(){return actCopy;}

     //!
    QAction *propertiesAction(){return actProperties;}

    //!
    QMenu *menuNew(){return mMenuNew;}

    //!
    QMenu *menuFile(){return mMenufile;}

     //!
    QMenu *menuView(){return mMenuView;}

    //!
    QMenu *menuGo(){return mMenuGo;}

    //!
    QMenu *menuTools(){return mMenuTools;}

    //!
    QMenu *menuEdit(){return mMenuEdit;}

    //!
    QMenu *menuEditePopup(){return mMenuEditePopup;}

    //!
    QMenu *menuPanels(){return mMenuPanels;}

    //!
    QMenu *menuTrach(){return mMenuTrash;}

    //!
    //QMenu *menuSettings(){return mMenuSettings;}


    //!
    QMenu *menuOpenWith(const QString &url, const QString &type);

    //!
    void addClosedTab(const QString &url);


    QList<QAction *>listActions(){return QList<QAction *>()
                <<mMenuGo->actions()
               <<mMenuView->actions()
              <<mMenufile->actions()
             <<mMenuTools->actions()
            <<mMenuEdit->actions()
           <<mMenuPanels->actions()
          <<actPathWidget;}

signals:

    /*!< تحديث الايقونات */
    void reloadIcons();

    /*!< تلقي مؤشر على تغير المسار */
    void urlChanged(const QString &);

    /*!< تلقي مؤشر لتغير تحديد المجلد */
    void curDirChanged(const QString &);

    /*!< تلقي مؤشر لتوفر مسارات في الحافظة للنسخ واللصق */
    void clipboardAvailable(bool);

    /*!< تلقي مؤشر على توفر تحديد */
    void selectionAvialable(bool);

    //!
    void forwardAvailable(bool);

    /*!< تلقي رسالة بتوفر السابق */
    void backAvailable(bool);
    //---------------------------
    /*!< الرسال رسالة بفتح لسان جديد */
    void sendNewTab(const QString &);

    //!
    void sendNewPath(const QString &);

    /*!< رسالة بالتنقل الى مجلد الاب */
    void sendGoUpDir();

    /*!< رسالة بالتنقل الى المجلد السابق */
    void sendGoBackDir();

    /*!< رسالة بالتنقل الى المجلد السابق */
    void sendGoForwardDir();

    /*!<   رسالة بالتنقل الى مجلد البيت */
    void sendGoHomeDir();

    /*!< رسالة بالتنقل الى سلة المحذوفات */
    void sendGoTrachDir();

    //!
    void sendGoSearch();

    //!
    void sendShowSettings();

    /*!< رسالة ببداية تغيير الاسم */
    void startEdit();

    /*!< رسالة ببداية الحذف */
    void startDelete();

    /*!< رسالة بالحذف الى سلة المحذوفات */
    void startMoveToTrash();

    /*!< رسالة ببدأ القص */
    void startCutFile();

    /*!< رسالة ببدأ النسخ */
    void startCopyFile();

    /*!< رسالة بالصق محتويات الحافظة  */
    void pasteClipboard();

    /*!< رسالة بتحديد الكل */
    void selectAll();

    /*!< رسالة بغلق اللسان الحالي */
    void sendCloseTab();

    /*!< رسالة بعرص المرشح */
    void showFilterBar();

    /*!< رسالة بعرض خصائص الملفات */
    void showProperties();

    /*!< رسالة بعرص نافذة فتح باستخدام */
    void   showOpenwithDlg();

    /*!< رسالة بعرص نافذة اعدادات قائمة الازرار لللوحة */
    void showConfigureTool();

    //!
    void zoomInChanged();
    void ZoomOutChanged();

    void newFileCreated(const QString &);

    void trashRestoreFiles();
    void trashDeleteFile();
    void trashClean();


public slots:
    //!
    void setUrl(const QString &);
    void setSelectedDir(const QString &path){m_selectedPath=path;}
    void restorePath(){m_selectedPath=m_dirPath;}

private:

    //!
    void chargeFavorite();

    //!
    void chargeAppService(const QString &path);

    //!
    void refreshIcons();

    //!
    void actionsShortcuts();

    //!
    QString m_dirPath;
    //!
    QString m_selectedPath;

    //!
    QString m_lC;

    //!
    Settings *mSettings;

    //!
    QStringList mListService;

    //!
    QActionGroup  *alignSortBy;

    //!
    QActionGroup *alignViewMode ;

    //!
    QMenu *mMenufile,*mMenuEditePopup,*mMenuView,*mMenuEdit,*mMenuTools/*,*mMenuSettings*/;

    QMenu *mMenuGo,*mMenuNew,*mMenuPanels,*mMenuClosedTab,*mMenuFavorite,*menuOpenW,*mMenuService;

    QAction *actOpenW;

    //menuFile
    QAction *actCloseTab,*actDelete,*actMoveToTrash,*actRename,*actProperties ;
    QAction *actAddArchive,*actExtractHere,*actTerminal;
    //menuEdit
    QAction* actCut ,*actSelectAll,*actFind,* actPaste,*actCopy,*actShowSettings;

    //menu go
    QAction*actGoBack ,*actGoUp,*actGoForward, *actGoHome, *actTrash;

    //menu view
    QAction *actDetailView,*actIconView   ,*actCompactView, *actSortByName;
    QAction *actSortByType,*actSortBySize,*actSortAsc;
    QAction *actZoomIn,*actZoomOut,*actHiddenFiles,*actThumbnails,*actReloadIcons;
    //menu panels
    QAction *actToolBar,*actMenuBar,*actPlacesFolder,*actInformation;

    // menu tools
    QAction *actFilter  ;

    // Trash
    QAction *actTrash_Clean ,*actTrash_restore ,*actTrash_Delete   ;
    QMenu *mMenuTrash;
   // menu settings
  /* QAction *actShooseTerminal, *actConfirmDragDrop*/
//   QAction *actSingleClick,*actExpandableFolder;
    // menu other


    QAction /**actNewTab,*/*actQuit,*actConfigTool;
    QAction *actOpenInNewTab ,*actOpenTerminal;
    QWidgetAction * actPathWidget;

private slots:

    //!
    void slotSetUrl(QAction *action);

    //!
    void clipboardChanged();

    //!
    void setupMenuTemplate();


//    //!
//    void openNewTab();

    //!
    void openInNewTab();

    //!
    QString  getTemplateApp(const QString &fileName);

    //!
    void creatXdgProgramme();

    //!
    void creatLink();

    //!
    void toggleView(QAction *action);

    //!
    void executeTemplate();

    //!
    void creatNewFile(const QString &fileSource);

    //!
    void creatNewDir();

    //!
    void actionOpenwith();

    //!
   // void openTerminal();

    //!
    void openInTerminal();

    //!
    void execService();

    //!
    void toggleSortBy(QAction* act);

    //!
   // bool findProgram(const QString &program);


    //!
   // void chooseTerminal();

    //!
    void archiveExtractHere();

    //!
     void addArchive();
};

#endif // ACTONS_H
