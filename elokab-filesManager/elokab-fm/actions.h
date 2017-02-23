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
          explicit Actions(Settings *setting,const QString &lc,QObject *parent = 0);

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

QAction *actNewTab,*actQuit,*actConfigTool,* actPaste,*actCopy;
 QAction *actOpenInNewTab ,*actOpenTerminal;
QWidgetAction * actPathWidget;

     signals:
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

     public slots:
          //!
          void actionsShortcuts();
            //!
          void refreshIcons();
          //!
          QMenu *menuOpenWith(const QString &url, const QString &type);

          //!
          QMenu *menuNew(){return mMenuNew;}

          //!
          QMenu *menuFile(){return mMenufile;}

          //!

          //!
          QMenu *menuView(){return mMenuView;}

          //!
          QMenu *menuGo(){return mMenuGo;}

          //!
          QMenu *menuTools(){return mMenuTools;}

          //!
          QMenu *menuEdit(){return mMenuEdit;}

          //!
          QMenu *menuViewfile(){return mMenuViewfile;}

          //!
          QMenu *menuPanels(){return mMenuPanels;}

          //!
          QMenu *menuSettings(){return mMenuSettings;}

          /**
           * @brief listActions قائمة بجميع الازرار
           * @return قائمة
          */
          QList<QAction *>listActions(){return QList<QAction *>()
                         <<mMenuGo->actions()
                        <<mMenuView->actions()
                       <<mMenufile->actions()
                      <<mMenuTools->actions()
                     <<mMenuEdit->actions()
                    <<mMenuPanels->actions()
                   <<actPathWidget;}

          //!
          void addClosedTab(const QString &url);

          //!
          void setcurDir(const QString &url){  m_curDir=url;}

          //!
          void chargeFavorite();

          //!
          void chargeAppService(const QString &path);
          //!
          QMenu *menuService(const QStringList &files, const QString &mim);

     private:

          //!
          QString m_dirPath;

          //!
          QString m_curDir;

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
          QMenu *mMenufile,*mMenuViewfile,*mMenuView,*mMenuEdit,*mMenuTools,*mMenuSettings;

          QMenu *mMenuGo,*mMenuNew,*mMenuPanels,*mMenuClosedTab,*mMenuFavorite,*menuOpenW,*mMenuService;

         QAction *actOpenW;

         //menuFile
         QAction *actCloseTab,*actDelete,*actMoveToTrash,*actRename,*actProperties ;

         //menuEdit
         QAction* actCut ,*actSelectAll,*actFind;

         //menu go
         QAction*actGoBack ,*actGoUp,*actGoForward, *actGoHome, *actTrash;

         //menu view
         QAction *actDetailView,*actIconView,*actCompactView, *actSortByName,*actSortByType,*actSortBySize,*actSortAsc;
         QAction *actZoomIn,*actZoomOut,*actHiddenFiles,*actThumbnails,*actReloadIcons;
         QAction *actToolBar,*actMenuBar,*actPlacesFolder,*actInformation;

         // menu tools
         QAction *actFilter  ;
         QAction *actShooseTerminal;
         // menu o
         // menu other

         QAction *actSingleClick,*actExpandableFolder;

     private slots:

//!
          void slotSetUrl(QAction *action);

          //!
          void clipboardChanged();

          //!
          void setupMenuTemplate();

          //!
          void setUrl(const QString &);

          //!
          void openNewTab();

          //!
          void openInNewTab();

          //!
          QString  getTemplateApp(const QString &fileName);

          //!
         // void creatLinkFile();

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
          bool findProgram(const QString &program);

          void ShooseTerminal();
};

#endif // ACTONS_H
