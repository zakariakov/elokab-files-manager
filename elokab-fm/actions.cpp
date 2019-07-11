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

#include "actions.h"
#include "defines.h"
#include "messages.h"
#include "filesutils/symlinkdlg.h"

#include <QDir>
#include <QApplication>
#include <QFileDialog>
#include <QSettings>
#include <QInputDialog>
#include<QTextCodec>
#include<QProcess>
#include<QClipboard>
#include<QMimeData>
#include<QDirIterator>
#include<QSettings>



/*********************************************************************
 *
 *********************************************************************/
Actions::Actions(Settings *setting, const QString &lc, QObject *parent) :
     QObject(parent),m_lC(lc)
{

#ifdef DEBUG_APP
     Messages::showMessage(Messages::TOP,"Actions::Actions()");// Actions::Actions()";
#endif
     mSettings=setting;

     const QClipboard *clipboard = QApplication::clipboard();

     m_dirPath=QDir::homePath();

     setObjectName("_Actions");

     connect(this ,SIGNAL(urlChanged(QString)),this,SLOT(setUrl(QString)));
   //  connect(this ,SIGNAL(curDirChanged(QString)),this,SLOT(setcurDir(QString)));
     connect(clipboard ,SIGNAL(dataChanged()),this,SLOT(clipboardChanged()));

     //
     actPathWidget=new QWidgetAction(this);
     actPathWidget->setText(tr("Path Edit"));
     actPathWidget->setObjectName("Path_Edit_Act");
     //
     menuOpenW=new QMenu(tr("Open with..."));
     actOpenW=new QAction(tr("Other..."),this);
     connect(actOpenW,SIGNAL(triggered()),this,SIGNAL(showOpenwithDlg()));
     menuOpenW->addAction(actOpenW);
     //menuOpenW->setDefaultAction(actOpenW);
     mMenuService=new QMenu(tr("Actions..."));
     //MENU FILE ACTIONS-----------------------------------------------------------------------
//     actNewTab = new QAction(tr("New tab"), this);
//     connect(actNewTab, SIGNAL(triggered()), this, SLOT(openNewTab()));
//     actNewTab->setObjectName("New_Tab_Act");
//     actNewTab->setStatusTip(tr("Open new tab  ")+actNewTab->shortcut().toString());


     // new Trash---
     // *actTrash_Clean ,*actTrash_restore ,*actTrash_Delete   ;
     actTrash_Clean = new QAction(tr("Empty trash"), this);
     connect(actTrash_Clean, SIGNAL(triggered()), this, SIGNAL(trashClean()));

     actTrash_restore = new QAction(tr("Restore"), this);
     connect(actTrash_restore, SIGNAL(triggered()), this, SIGNAL(trashRestoreFiles()));
     connect(this,SIGNAL(selectionAvialable(bool)),actTrash_restore,SLOT(setEnabled(bool)));

     actTrash_Delete = new QAction(tr("Delete"), this);
     connect(actTrash_Delete, SIGNAL(triggered()), this, SIGNAL(trashDeleteFile()));
     connect(this,SIGNAL(selectionAvialable(bool)),actTrash_Delete,SLOT(setEnabled(bool)));
     mMenuTrash=new QMenu;
     mMenuTrash->addAction(actTrash_Clean);
     mMenuTrash->addAction(actTrash_restore);
     mMenuTrash->addSeparator();
     mMenuTrash->addAction(actTrash_Delete);

     // new tab contextmenu---
     actOpenInNewTab = new QAction(tr("Open In new tab"), this);
     connect(actOpenInNewTab, SIGNAL(triggered()), this, SLOT(openInNewTab()));
     actOpenInNewTab->setObjectName("Open_In_newtab_Act");


     actCloseTab = new QAction(tr("Close tab"), this);
     connect(actCloseTab, SIGNAL(triggered()), this, SIGNAL(sendCloseTab()));
     actCloseTab->setObjectName("Close_Tab_Act");
     actCloseTab->setStatusTip(tr("Close current tab  ")+actCloseTab->shortcut().toString());


     actQuit = new QAction(tr("Quit"), this);
     connect(actQuit, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
     actQuit->setObjectName("Quit_Act");
     actQuit->setStatusTip(tr("Quit this Application  ")+actQuit->shortcut().toString());

     actMoveToTrash = new QAction(tr("Move To Trash"), this);
     connect(actMoveToTrash, SIGNAL(triggered()), this, SIGNAL(startMoveToTrash()));
     connect(this,SIGNAL(selectionAvialable(bool)),actMoveToTrash,SLOT(setEnabled(bool)));
     actMoveToTrash->setObjectName("Move_To_Trash_Act");
     actMoveToTrash->setStatusTip(tr("Move selected files to trash   ")+actMoveToTrash->shortcut().toString());

     actDelete = new QAction(tr("Delete"), this);
     actDelete->setStatusTip(tr("Delete selected file"));
     connect(actDelete, SIGNAL(triggered()), this, SIGNAL(startDelete()));
     connect(this,SIGNAL(selectionAvialable(bool)),actDelete,SLOT(setEnabled(bool)));
     actDelete->setObjectName("Delete_Files_Act");
     actDelete->setStatusTip(tr("Delete selected files   ")+actDelete->shortcut().toString());

     actRename = new QAction(tr("Rename"), this);
     connect(actRename, SIGNAL(triggered()),this, SIGNAL(startEdit()));
     connect(this,SIGNAL(selectionAvialable(bool)),actRename,SLOT(setEnabled(bool)));
     actRename->setEnabled(false);
     actRename->setObjectName("Rename_File_Act");
     actRename->setStatusTip(tr("Rename file   ")+actRename->shortcut().toString());

     //MENU EDIT ACTIONS-----------------------------------------------------------------------------
     actCut = new QAction(tr("Cut"), this);
     connect(actCut, SIGNAL(triggered()), this, SIGNAL(startCutFile()));
     connect(this,SIGNAL(selectionAvialable(bool)),actCut,SLOT(setEnabled(bool)));
     actCut->setEnabled(false);
     actCut->setObjectName("Cut_Files_Act");
     actCut->setStatusTip(tr("Move selected files   ")+actCut->shortcut().toString());

     actCopy = new QAction(tr("Copy"), this);
     connect(actCopy, SIGNAL(triggered()), this, SIGNAL(startCopyFile()));
     connect(this,SIGNAL(selectionAvialable(bool)),actCopy,SLOT(setEnabled(bool)));
     actCopy->setEnabled(false);
     actCopy->setObjectName("Copy_Files_Act");
     actCopy->setStatusTip(tr("Copy selected files   ")+actCopy->shortcut().toString());

     actPaste = new QAction(tr("Paste"), this);
     connect(actPaste, SIGNAL(triggered()), this, SIGNAL(pasteClipboard()));
     connect(this,SIGNAL(clipboardAvailable(bool)),actPaste,SLOT(setEnabled(bool)));
     actPaste->setObjectName("Paste_Files_Act");
     actPaste->setStatusTip(tr("Paste clipboard contents   ")+actPaste->shortcut().toString());

     actExtractHere= new QAction(tr("Extract Here"), this);
     connect(actExtractHere, SIGNAL(triggered()), this, SLOT(archiveExtractHere()));
     actExtractHere->setStatusTip(tr("Extract Archive in curent folder"));

     actAddArchive= new QAction(tr("Add To Archive"), this);
     connect(actAddArchive, SIGNAL(triggered()), this, SLOT(addArchive()));
     actAddArchive->setStatusTip(tr("Add selected to Archive tar.gz"));



     actSelectAll = new QAction(tr("Select All"), this);
     actSelectAll->setStatusTip(tr("Select all files"));
     connect(actSelectAll, SIGNAL(triggered()),this, SIGNAL(selectAll()));
     actSelectAll->setObjectName("Select_All_Act");


     actProperties = new QAction(tr("Properties"), this);
     connect(actProperties, SIGNAL(triggered()),this, SIGNAL(showProperties()));
     actProperties->setObjectName("Properties_Act");

     actFind = new QAction(tr("Find..."), this);
     connect(actFind, SIGNAL(triggered()),this, SIGNAL(sendGoSearch()));
     actFind->setObjectName("Find_Act");

     actShowSettings = new QAction(tr("Settings"), this);
     connect(actShowSettings, SIGNAL(triggered()),this, SIGNAL(sendShowSettings()));
     actShowSettings->setObjectName("Settings_Act");
     actShowSettings->setStatusTip(tr("Show settings dialog")+actShowSettings->shortcut().toString());

     //MENU VIEW ACTIONS-----------------------------------------------------------------------------
     actIconView = new QAction(tr("Icon view"),this);
     actIconView->setStatusTip(tr("Toggle icon view"));
     actIconView->setCheckable(true);
     actIconView->setChecked(mSettings->viewMode()==IconView);
     actIconView->setData(IconView);
     actIconView->setObjectName("Icon_View_Act");

     actCompactView = new QAction(tr("Compact view"),this);
     actCompactView->setStatusTip(tr("Toggle compact View"));
     actCompactView->setCheckable(true);
     actCompactView->setChecked(mSettings->viewMode()==CompactView);
     actCompactView->setData(CompactView);
     actCompactView->setObjectName("Compact_View_Act");

     actDetailView = new QAction(tr("Detail view"),this);
     actDetailView->setStatusTip(tr("Toggle detailed list"));
     actDetailView->setCheckable(true);
     actDetailView->setChecked(mSettings->viewMode()==DetailView);
     actDetailView->setData(DetailView);
     actDetailView->setObjectName("Detail_View_Act");

     actThumbnails = new QAction(tr("Preview"),this);
     actThumbnails->setStatusTip(tr("Show Preview of images files"));
     actThumbnails->setCheckable(true);
     //  actThumbnails->setIcon(IconXdg::fromTheme("image-x-generic","image"));

     actThumbnails->setChecked(mSettings->showThumbnails());
     connect(actThumbnails, SIGNAL(toggled(bool)),mSettings,SLOT(setshowThumbnails(bool)));
     actThumbnails->setObjectName("Show_Preview_Act");

     actReloadIcons = new QAction(tr("Reload"),this);
     actReloadIcons->setStatusTip(tr("Reload"));

     connect(actReloadIcons, SIGNAL(triggered()),this,SIGNAL(reloadIcons()));
     actReloadIcons->setObjectName("ReloadIcons");

     actZoomIn = new QAction(tr("Zoom In"),this);
     actZoomIn->setStatusTip(tr("Zoom in icons"));
     connect(actZoomIn, SIGNAL(triggered()),this,SIGNAL(zoomInChanged()));
     actZoomIn->setObjectName("Zoom_In_Act");

     actZoomOut = new QAction(tr("Zoom Out"),this);
     actZoomOut->setStatusTip(tr("Zoom out icons "));
     connect(actZoomOut, SIGNAL(triggered()),this,SIGNAL(ZoomOutChanged()));
     actZoomOut->setObjectName("Zoom_Out_Act");

     actHiddenFiles = new QAction(tr("Show Hidden files"), this);
     actHiddenFiles->setStatusTip(tr("Toggle hidden files"));
     actHiddenFiles->setCheckable(true);
     connect(actHiddenFiles, SIGNAL(toggled(bool)),mSettings,SLOT(setShowHidden(bool)));
     actHiddenFiles->setChecked(mSettings->showHidden());
     actHiddenFiles->setObjectName("Show_Hidden_Act");
     // TODO:Add Icons resource
     // add image 'folder-saved-search'
     actSortByName = new QAction(tr("Sort By Name"), this);
     actSortByName->setStatusTip(tr("Sort icons by name"));
     actSortByName->setData(sortName);
     actSortByName->setCheckable(true);
     actSortByName->setChecked(mSettings->sortBy()==0);
     actSortByName->setObjectName("Sort_By_Name_Act");

     actSortBySize = new QAction(tr("Sort By Size"), this);
     actSortBySize->setStatusTip(tr("Sort icons by size"));
     actSortBySize->setData(sortSize);
     actSortBySize->setCheckable(true);
     actSortBySize->setChecked(mSettings->sortBy()==1);
     actSortBySize->setObjectName("Sort_By_Size_Act");

     actSortByType = new QAction(tr("Sort By type"), this);
     actSortByType->setStatusTip(tr("Sort icons by type"));
     actSortByType->setData(sortType);
     actSortByType->setCheckable(true);
     actSortByType->setChecked(mSettings->sortBy()==2);
     actSortByType->setObjectName("Sort_By_Type_Act");

     actSortAsc = new QAction(tr("Sort Ascending"), this);
     actSortAsc->setStatusTip(tr("Sort icons in ascending order"));
     actSortAsc->setCheckable(true);
     connect(actSortAsc, SIGNAL(toggled(bool)), mSettings, SLOT(setSortAscending(bool)));
     actSortAsc->setChecked(mSettings->sortAscending());
     actSortAsc->setObjectName("Sort_Ascending_Act");

     actPlacesFolder= new QAction(tr("Folders"), this);
     actPlacesFolder->setStatusTip(tr("folders Navigation ")+actPlacesFolder->shortcut().toString());
     actPlacesFolder->setCheckable(true);
     actPlacesFolder->setChecked(mSettings->folderNavigation());
     connect(actPlacesFolder, SIGNAL(toggled(bool)), mSettings, SLOT(setFolderNavigation(bool)));
     actPlacesFolder->setObjectName("Show_Folders_Navigation_Act");

     actInformation= new QAction(tr("Information"), this);
     actInformation->setStatusTip(tr("File Information ")+actInformation->shortcut().toString());
     actInformation->setCheckable(true);
     actInformation->setChecked(mSettings->showInformation());
     connect(actInformation, SIGNAL(toggled(bool)), mSettings, SLOT(setShowInformation(bool)));
     actInformation->setObjectName("File_Information_Act");

     actToolBar= new QAction(tr("Toolbar"), this);
     actToolBar->setStatusTip(tr("Show Hide Toolbar ")+actToolBar->shortcut().toString());
     actToolBar->setCheckable(true);
     actToolBar->setChecked(mSettings->showToolbar());
     connect(actToolBar, SIGNAL(toggled(bool)), mSettings, SLOT(setShowToolbar(bool)));
     actToolBar->setObjectName("Show_Toolbar_Act");


     actMenuBar= new QAction(tr("MenuBar"), this);
     actMenuBar->setStatusTip(tr("Show Hide MenuBar ")+actMenuBar->shortcut().toString());
     actMenuBar->setCheckable(true);
     actMenuBar->setChecked(mSettings->showMenuBar());
     connect(actMenuBar, SIGNAL(toggled(bool)), mSettings, SLOT(setShowMenuBar(bool)));
     actMenuBar->setObjectName("Show_MenuBar_Act");


     //MENU GO ACTIONS--------------------------------------------------------------------------------------

     actGoUp = new QAction(tr("Up"),this);
     actGoUp->setStatusTip(tr("Go up one directory ")+actGoUp->shortcut().toString());
     connect(actGoUp, SIGNAL(triggered()),this,SIGNAL(sendGoUpDir()));
     actGoUp->setObjectName("Go_Up_Act");

     actGoBack = new QAction(tr("Back"),this);
     actGoBack->setStatusTip(tr("Go back one directory ")+actGoBack->shortcut().toString());
     connect(this, SIGNAL(backAvailable(bool)),actGoBack,SLOT(setEnabled(bool)));
     connect(actGoBack, SIGNAL(triggered()),this,SIGNAL(sendGoBackDir()));
     actGoBack->setObjectName("Go_Back_Act");


     actGoForward = new QAction(tr("Forward"),this);
     actGoForward->setStatusTip(tr("Go forward one directory ")+actGoForward->shortcut().toString());
     connect(this, SIGNAL(forwardAvailable(bool)),actGoForward,SLOT(setEnabled(bool)));
     connect(actGoForward, SIGNAL(triggered()),this,SIGNAL(sendGoForwardDir()));
     actGoForward->setObjectName("Go_Forward_Act");


     actGoHome = new QAction(tr("Home"),this);
     actGoHome->setStatusTip(tr("Go to home directory"));
     connect(actGoHome, SIGNAL(triggered()),this,SIGNAL(sendGoHomeDir()));
     actGoHome->setObjectName("Go_Home_Act");

     actTrash = new QAction(tr("trash"),this);
     actTrash->setStatusTip(tr("Go to trash directory"));
     connect(actTrash, SIGNAL(triggered()),this,SIGNAL(sendGoTrachDir()));
     actTrash->setObjectName("Go_Trash_Act");

     //MENU TOOLS ACTIONS---------------------------------------------------------------------------
     actFilter = new QAction(tr("Filter"),this);
     actFilter->setStatusTip(tr("Show filter bar"));
     connect(actFilter, SIGNAL(triggered()),this,SIGNAL(showFilterBar()));
     actFilter->setObjectName("Show_Filter_Act");

     actOpenTerminal = new QAction(tr("Open terminal"),this);
     actOpenTerminal->setStatusTip(tr("Open terminal in curent path"));
     connect(actOpenTerminal, SIGNAL(triggered()), this, SLOT(openInTerminal()));

//     actTerminal = new QAction(tr("Open terminal"),this);
//     actTerminal->setStatusTip(tr("Open terminal in curent path"));
//     connect(actTerminal, SIGNAL(triggered()), this, SLOT(openTerminal()));
//     actTerminal->setObjectName("Open_Terminal_Act");

     //OTHER ACTIONS--------


//     actSingleClick= new QAction(tr("Single-click to open files and folders"), this);
//     actSingleClick->setCheckable(true);
//     actSingleClick->setChecked(mSettings->isSingleclick());
//     connect(actSingleClick, SIGNAL(toggled(bool)), mSettings, SLOT(setSingleclick(bool)));
//     actSingleClick->setObjectName("SingleClick_Act");

//     actExpandableFolder= new QAction(tr("Expandable folders in view details"), this);
//     actExpandableFolder->setCheckable(true);
//     actExpandableFolder->setChecked(mSettings->isRootDecorated());
//     connect(actExpandableFolder, SIGNAL(toggled(bool)), mSettings, SLOT(setRootIsDecorated(bool)));
//     actExpandableFolder->setObjectName("ExpandableFolder_Act");

     actConfigTool = new QAction(tr("Configure toolbars..."),this);
     actConfigTool->setStatusTip(tr("Configure toolbars"));
     connect(actConfigTool, SIGNAL(triggered()),this,SIGNAL(showConfigureTool()));
     actConfigTool->setObjectName("Configure_Tool_Act");

//     actShooseTerminal = new QAction(tr("Configure Terminal..."),this);
//     connect(actShooseTerminal, SIGNAL(triggered()),this,SLOT(chooseTerminal()));
//     actShooseTerminal->setObjectName("ConfigureTerminal_Tool_Act");

//     actConfirmDragDrop = new QAction(tr("Confirm Drag Drop files"),this);
//     actConfirmDragDrop->setCheckable(true);
//     actConfirmDragDrop->setChecked(mSettings->isConfirmDragDrop());
//     connect(actConfirmDragDrop, SIGNAL(toggled(bool)),mSettings,SLOT(setConfirmDragDrop(bool)));

     //-----------mMenufile--------
     mMenufile=new QMenu(tr("&File"));
     mMenuNew=new QMenu(trUtf8("Create New"));
     setupMenuTemplate();
     mMenufile ->addMenu(mMenuNew);
     mMenufile->addSeparator();
     mMenufile->addAction(actOpenInNewTab);
     mMenufile->addAction(actCloseTab);
     mMenufile->addSeparator();

     mMenufile->addAction(actMoveToTrash);
     mMenufile->addAction(actDelete);
     mMenufile->addSeparator();
    // mMenufile->addAction(actProperties);
     mMenufile->addAction(actOpenTerminal);
     //-----------mMenuViewfile--------

     mMenuEditePopup=new QMenu(tr("&File"));
     mMenuEditePopup->addAction(actCut);
     mMenuEditePopup->addAction(actCopy);
     mMenuEditePopup->addAction(actPaste);
     mMenuEditePopup->addSeparator();
     mMenuEditePopup->addAction(actRename);
     mMenuEditePopup->addAction(actMoveToTrash);
     mMenuEditePopup->addAction(actDelete);
     mMenuEditePopup->addSeparator();
    // mMenuViewfile->addAction(actProperties);


     //-----------mMenuEdit--------
     mMenuEdit=new QMenu(tr("&Edit"));
     mMenuEdit->addAction(actCut);
     mMenuEdit->addAction(actCopy);
     mMenuEdit->addAction(actPaste);
      mMenuEdit->addSeparator();
     mMenuEdit->addAction(actRename);
     mMenuEdit->addSeparator();
     mMenuEdit->addAction(actFind);
     mMenuEdit->addSeparator();
     mMenuEdit->addAction(actSelectAll);
     mMenuEdit->addSeparator();
     mMenuEdit->addAction(actShowSettings);

     //-----------mMenuView--------
     mMenuView=new QMenu(tr("&View"));
     alignSortBy = new QActionGroup(this);
     alignViewMode = new QActionGroup(this);

     alignViewMode->addAction(actIconView);
     alignViewMode->addAction(actDetailView);
     alignViewMode->addAction(actCompactView);
     connect(alignViewMode, SIGNAL(triggered(QAction*)), SLOT(toggleView(QAction*)));

     alignSortBy->addAction(actSortByName);
     alignSortBy->addAction(actSortByType);
     alignSortBy->addAction(actSortBySize);
     connect(alignSortBy, SIGNAL(triggered(QAction*)), SLOT(toggleSortBy(QAction*)));
     mMenuView->addAction(actZoomIn);
     mMenuView->addAction(actZoomOut);
     mMenuView->addSeparator();
     mMenuView->addAction(actIconView);
     mMenuView->addAction(actDetailView);
     mMenuView->addAction(actCompactView);
     mMenuView->addSeparator();
     mMenuView->addAction(actSortByName);
     mMenuView->addAction(actSortBySize);
     mMenuView->addAction(actSortByType);
     mMenuView->addSeparator();
     mMenuView->addAction(actSortAsc);
     mMenuView->addSeparator();
     mMenuView->addAction(actThumbnails);

     mMenuView->addAction(actHiddenFiles);
     mMenuView->addSeparator();
     mMenuView->addAction(actReloadIcons);

     //-----------mMenuSettings-------------
   //  mMenuSettings=new QMenu(tr("&Settings"));
    // mMenuSettings->addMenu(mMenuPanels);
    // mMenuSettings->addSeparator();
    // mMenuSettings->addAction(actConfigTool);
    // mMenuSettings->addAction(actSingleClick);
    // mMenuSettings->addAction(actExpandableFolder);
   //  mMenuSettings->addAction(actConfirmDragDrop);
    // mMenuSettings->addSeparator();
    // mMenuSettings->addAction(actShooseTerminal);
    // mMenuSettings->addSeparator();
     //     mMenuSettings->addAction(actStandardIcon);
     //-----------mMenuTools--------
     mMenuTools=new QMenu(tr("&Tools"));
     mMenuTools->addAction(actFilter);
     mMenuPanels=new QMenu(tr("Pa&nels"));
     mMenuPanels->addAction(actPlacesFolder);
     mMenuPanels->addAction(actInformation);
     mMenuPanels->addAction(actToolBar);
     mMenuPanels->addAction(actMenuBar);
     mMenuTools->addMenu(mMenuPanels);
     mMenuTools->addSeparator();
     mMenuTools->addAction(actConfigTool);

     //-----------mMenuGo--------
     mMenuGo=new QMenu(tr("&Go"));
     mMenuClosedTab=new QMenu(tr("&Closed tab"));
     mMenuFavorite=new QMenu(tr("&Bookmarks"));
     connect(mMenuFavorite,SIGNAL(triggered(QAction*)),SLOT(slotSetUrl(QAction *)));
     connect(mMenuClosedTab,SIGNAL(triggered(QAction*)),SLOT(slotSetUrl(QAction *)));

     chargeFavorite();

     mMenuGo->addAction(actGoBack);
     mMenuGo->addAction(actGoForward);
     mMenuGo->addAction(actGoUp);
     mMenuGo->addAction(actGoHome);
     mMenuGo->addAction(actTrash);
     mMenuGo->addSeparator();
     mMenuGo->addMenu(mMenuFavorite);
     mMenuGo->addMenu(mMenuClosedTab);
     refreshIcons();
     actionsShortcuts();
     clipboardChanged();

     //---------------service-----------------------
     /// مسار مجلد البرنامج


     QString serviceLocal=Edir::dataHomeAppDir()+"/service/";
     QDir dir(serviceLocal);
     if(!dir.exists())
        dir.mkpath(serviceLocal);

     QString serviceUser=  Edir::dataAppDir()+"/service/";
     chargeAppService(serviceLocal);
     chargeAppService (serviceUser);



#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"Actions::Actions()");// Actions::Actions()";
#endif
}


/*********************************************************************
 *
 *********************************************************************/
void Actions::refreshIcons()
{
    actPathWidget->setIcon(EIcon::fromTheme("edit-rename","edit"));
   // actNewTab->setIcon(EIcon::fromTheme("tab-new",("add")));
    actCloseTab->setIcon(EIcon::fromTheme("window-close"));
    actQuit->setIcon(EIcon::fromTheme("application-exit","window-close"));
    actMoveToTrash->setIcon(EIcon::fromTheme("user-trash","emptytrash"));
    actDelete->setIcon(EIcon::fromTheme("edit-delete","delete"));
    actRename->setIcon(EIcon::fromTheme("edit-rename","edit"));
    actCut->setIcon(EIcon::fromTheme("edit-cut"));
    actCopy->setIcon(EIcon::fromTheme("edit-copy"));
    actPaste->setIcon(EIcon::fromTheme("edit-paste"));
    actProperties->setIcon(EIcon::fromTheme("configure"));
    actFind->setIcon(EIcon::fromTheme("find",("edit-find")));
    actIconView->setIcon(EIcon::fromTheme("view-list-icons"));
    actCompactView->setIcon(EIcon::fromTheme("view-list-tree", "view-list-compact"));
    actDetailView->setIcon(EIcon::fromTheme("view-list-details"));
    actThumbnails->setIcon(EIcon::fromTheme("view-preview","image-x-generic"));
    actReloadIcons->setIcon(EIcon::fromTheme("reload"));
    actZoomIn->setIcon(EIcon::fromTheme("zoom-in"));
    actZoomOut->setIcon(EIcon::fromTheme("zoom-out"));
    actHiddenFiles->setIcon(EIcon::fromTheme("folder-saved-search",("edit-find")));
    actSortAsc->setIcon(EIcon::fromTheme("view-sort-ascending"));
    actPlacesFolder->setIcon(EIcon::fromTheme("view-list-tree", "view-list-compact"));
    actInformation->setIcon(EIcon::fromTheme("help-info"));
    actToolBar->setIcon(EIcon::fromTheme("help-info"));
    actMenuBar->setIcon(EIcon::fromTheme("show-menu"));
    actGoUp->setIcon(EIcon::fromTheme("go-up"));
    if(QApplication::isLeftToRight()){
        actGoBack->setIcon(EIcon::fromTheme("go-previous"));
        actGoForward->setIcon(EIcon::fromTheme("go-next"));
    }else{
        actGoBack->setIcon(EIcon::fromTheme("go-next"));
        actGoForward->setIcon(EIcon::fromTheme("go-previous"));
    }
    actGoHome->setIcon(EIcon::fromTheme("go-home","user-home"));
    actTrash->setIcon(EIcon::fromTheme("user-trash","emptytrash"));
    actFilter->setIcon(EIcon::fromTheme("view-filter"));
    actOpenTerminal->setIcon(EIcon::fromTheme("terminal"));
   // actTerminal->setIcon(EIcon::fromTheme("terminal"));

    actOpenInNewTab->setIcon(EIcon::fromTheme("tab-new",("add")));
    actConfigTool->setIcon(EIcon::fromTheme("configure"));
  //  actShooseTerminal->setIcon(EIcon::fromTheme("configure"));
    actShowSettings->setIcon(EIcon::fromTheme("configure"));

   // *actTrash_Clean ,*actTrash_restore ,*actTrash_Delete   ;
        actTrash_Clean->setIcon(EIcon::fromTheme("user-trash","emptytrash"));
        actTrash_Delete->setIcon(EIcon::fromTheme("edit-delete"));
        actTrash_restore->setIcon(EIcon::fromTheme("reload"));



}

/*********************************************************************
 *
 *********************************************************************/
void Actions::actionsShortcuts()
{
     actOpenInNewTab->setShortcut(QKeySequence::AddTab);
    actCloseTab->setShortcut(QKeySequence::Close);
    actQuit->setShortcut(QKeySequence("ALT+F4"));
    actMoveToTrash->setShortcut(QKeySequence::Delete);
    actDelete->setShortcut(QKeySequence("Shift+Del"));
    actRename->setShortcut(QKeySequence(Qt::Key_F2));
    actCut->setShortcut(QKeySequence::Cut);
    actCopy->setShortcut(QKeySequence::Copy);
    actPaste->setShortcut(QKeySequence::Paste);
    actSelectAll->setShortcut(QKeySequence::SelectAll);
    actProperties->setShortcut(QKeySequence("Alt+Return"));
    actFind->setShortcut(QKeySequence::Find);
    actIconView->setShortcut(QKeySequence("Ctrl+1"));
    actCompactView->setShortcut(QKeySequence("Ctrl+3"));
    actDetailView->setShortcut(QKeySequence("Ctrl+2"));
    actThumbnails->setShortcut(QKeySequence("Ctrl+4"));
    actReloadIcons->setShortcut(QKeySequence::Refresh);
    actZoomIn->setShortcut(QKeySequence::ZoomIn);
    actZoomOut->setShortcut(QKeySequence::ZoomOut);
    actHiddenFiles->setShortcut(QKeySequence("Ctrl+H"));
    actPlacesFolder->setShortcut(QKeySequence("F7"));
    actInformation->setShortcut(QKeySequence("F11"));
    actToolBar->setShortcut(QKeySequence("F8"));
    actMenuBar->setShortcut(QKeySequence("F9"));
    actGoUp->setShortcut(QKeySequence("Alt+Up"));
    actGoBack->setShortcut(QKeySequence::Back);
    actGoForward->setShortcut(QKeySequence::Forward);
    actGoHome->setShortcut(QKeySequence::MoveToStartOfDocument);
    actFilter->setShortcut(QKeySequence("Ctrl+I"));
   actOpenTerminal->setShortcut(QKeySequence("Shift+F4"));
    // actTerminal->setShortcut(QKeySequence("Shift+F4"));
actShowSettings->setShortcut(QKeySequence::Preferences);
}

/*********************************************************************
 *
 *********************************************************************/
void Actions::chargeFavorite()
{


     foreach (QAction *act, mMenuFavorite->actions()) {
          delete act;
     }
     mMenuFavorite->clear();
     QSettings setting(QApplication::organizationName(),QApplication::applicationName());

     int count = setting.beginReadArray("Boukmarks");

     for (int i = 0; i < count; ++i){

          setting.setArrayIndex(i);

          QString   file = setting.value("file", "").toString();

          if (! file.isEmpty()){
               QDir dir(file);
               QString name=dir.dirName();
               if(name.isEmpty())name="/";
               QAction  *actBookMark=new QAction(this);
               actBookMark->setText(name);
               actBookMark->setIcon(EMimIcon::iconFolder(file));
               actBookMark->setData(file);

               mMenuFavorite->addAction(actBookMark);
          }

     } // for

     setting.endArray();


}

/*********************************************************************
 *
 *********************************************************************/
void Actions::slotSetUrl(QAction *action)
{
     emit sendNewPath(action->data().toString());

}

/*********************************************************************
 *
 *********************************************************************/
void Actions::clipboardChanged()
{


     QClipboard *clipboard = QApplication::clipboard();
     const QMimeData *mimeData = clipboard->mimeData();
     if(mimeData->hasText()){

     }
//     if(m_dirPath==_TRASH || m_dirPath==_SEARCH){
//        emit  clipboardAvailable(false);
//        return;
//     }

     if(mimeData->hasUrls())
     {
          emit  clipboardAvailable(true);
     }else{
          emit  clipboardAvailable(false);
     }


}

/*********************************************************************
 *
 *********************************************************************/
void Actions::setUrl(const QString &url)
{

     m_dirPath=url;
     m_selectedPath=url;
     actGoUp->setEnabled(m_dirPath!=QDir::rootPath()&&m_dirPath!=D_TRASH);
     if(m_dirPath==D_TRASH || m_dirPath==D_SEARCH){
       mMenuNew->setEnabled(false);
     }else{
          mMenuNew->setEnabled(true);
     }
//     actOpenInNewTab->setData(m_dirPath);
//     actOpenTerminal->setData(m_dirPath);
   // qDebug()<<__FILE__<<__FUNCTION__<<m_dirPath;

}

/*********************************************************************
 *
 *********************************************************************/
QMenu *Actions::menuOpenWith(const QString &url,const QString &type)
{


     if(menuOpenW->actions().count()>1)
     {
          QList<QAction *>list;
          for (int i = 0; i < menuOpenW->actions().count()-1; ++i) {
               list<<menuOpenW->actions().at(i);

          }
          foreach (QAction *act, list) {
               delete act;
          }
     }

     //get the associated applications
     QStringList list=EMimIcon::associatedApplication(type);
     QHash<QString ,QVariant> hash;
     QList<QAction *>listActs;
     foreach (QString desktopFN, list)
     {
          QString fpath=   EMimIcon::desktopFilePath(desktopFN);
          if(fpath.isEmpty())
               continue;

          hash=EMimIcon:: desktopFile(fpath,m_lC);

          QAction *actProc=new QAction(EIcon::fromTheme(hash["Icon"].toString()),hash["Name"].toString(),this);
          actProc->setData(hash["Exec"].toString()+" \""+url+"\"");
          connect(actProc,SIGNAL(triggered()),this,SLOT(actionOpenwith()));
          listActs.append(actProc);
          // menuOpenW->addAction(actProc);
     }
     //   menuOpenW->addSeparator();

     if(!listActs.isEmpty()){
          menuOpenW->insertActions(actOpenW,listActs);
          menuOpenW->insertSeparator(actOpenW);
     }else{
          //  menuOpenW->removeAction(actOpenW);
          //  menuOpenW->clear();
     }

     return menuOpenW;

}

/*********************************************************************
 *
 *********************************************************************/
void Actions::actionOpenwith()
{

     QAction *action = qobject_cast<QAction *>(sender());
     if(action)
     {
          QString data=action->data().toString();
          data=   EMimIcon::replaceArgument(data);
          QProcess proc;

          proc.startDetached(data);
     }

}

/*********************************************************************
 *
 *********************************************************************/
void Actions::setupMenuTemplate()
{

    mMenuNew->setIcon(EIcon::fromTheme("list-add",("add")));



    QStringList list;
    list<<Edir::dataHomeDir()+"/templates"
        <<Edir::dataDir()+"/templates"
        <<"/usr/share/templates";

    QStringList listTemp;
    QStringList listPath;
    foreach (QString s, list)
    {
        QDir d(s);
        foreach (QFileInfo fi, d.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries ))
        {
            if(fi.fileName().startsWith("link"))
                continue;

            if(!listTemp.contains(fi.fileName())){
                listTemp<<fi.fileName();
                listPath<<fi.filePath();
            }

        }
    }

    foreach (QString fileName, listPath) {

        QString line=getTemplateApp((fileName));
        QAction *act=new QAction(EIcon::fromTheme(line.section("|",1,1)),
                                 line.section("|",0,0),this);
        act->setData(line.section("|",2,2));
        connect(act,SIGNAL(triggered()),this,SLOT(executeTemplate()));

        mMenuNew->addAction(act);
   //    qDebug()<<"templat================================"<<fileName<<line;

    }

}

/*********************************************************************
 *
 *********************************************************************/
QString  Actions::getTemplateApp(const QString &fileName)
{


     QFileInfo fi(fileName);
     QString baseName=fi.baseName();

     QSettings settings(fileName,QSettings::IniFormat);
     settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
     settings.beginGroup("Desktop Entry");
     QString nameold=settings.value("Name",baseName).toString();
     QString name=settings.value("Name["+m_lC+"]",nameold).toString();
     QString URL=settings.value("URL","").toString();
     QString icon=settings.value("Icon",baseName).toString();
     settings.endGroup();



     return name+"|"+icon+"|"+URL;

}

/*********************************************************************
 *
 *********************************************************************/
void Actions::executeTemplate()
{

     QAction *action = qobject_cast<QAction *>(sender());
     if (action)
     {
          QString data=action->data().toString();

          if(data.contains("emptydir"))
          {
               creatNewDir();
          }else if(data=="__CREATE_SYMLINK__"){
              creatLink();
          }else if(data=="__CREATE_XDGPROGRAME__"){
              creatXdgProgramme();
           }else{
              QString fileSource;
               QStringList list;
               list<<Edir::dataHomeDir()+"/templates"
                   <<Edir::dataDir()+"/templates"
                   <<"/usr/share/templates";

               foreach (QString s, list) {
                   if(QFile::exists(s+"/"+data)){
                       fileSource=s+"/"+data;
                       break;
                   }
               }
               qDebug()<<fileSource;
               creatNewFile(fileSource);
          }

     }

}

/*********************************************************************
 *
 *********************************************************************/
void Actions::creatNewFile(const QString &fileSource)
{

     QFileInfo fiS(fileSource);
     bool ok;
     QString newName = QInputDialog::getText(nullptr, tr("New File"),
                                             tr("Enter name:"), QLineEdit::Normal,
                                             fiS.baseName(), &ok);
     if (ok && !newName.isEmpty())
     {
          QFile file(m_dirPath+"/"+newName+"."+fiS.suffix());
          if(file.exists())
          {
               creatNewFile(fileSource);
          }else{
               //   file.open(QIODevice::WriteOnly | QIODevice::Text);
               QFile::copy(fileSource,file.fileName());
               emit newFileCreated(file.fileName());
          }
          file.close();
     }

}

/*********************************************************************
 *
 *********************************************************************/
void Actions::creatNewDir()
{
     QDir  path(m_dirPath);

     bool ok;
     QString newName = QInputDialog::getText(nullptr, tr("New Folder"),
                                             tr("Enter name:"), QLineEdit::Normal,
                                             tr("New Folder"), &ok);

     if (ok && !newName.isEmpty())
     {
          QFile file(m_dirPath+"/"+newName);

          if(file.exists()){
               creatNewDir();
          }else{
               path.mkdir(newName);
              emit newFileCreated(m_dirPath+"/"+newName);
          }

     }

}


/**********************************************************************
 *                        *انشاء اختصار
 *********************************************************************/
void Actions::creatLink()
{

     SymLinkDlg *dlg=new SymLinkDlg(m_dirPath);
     dlg->exec();
//     if(dlg->exec()==QDialog::Accepted){
//         emit newFileCreated(dlg->fileName());
//     }


}
/***********************************************************************
 *                        *انشاء اختصار
 ***********************************************************************/

void Actions::creatXdgProgramme()
{


     DialogXdesktop *dlg=new DialogXdesktop(m_dirPath);
     if(dlg->exec()==QDialog::Accepted){
         emit newFileCreated(dlg->fileName());
     }

}

/**********************************************************************
 *
 **********************************************************************/
void Actions::toggleView(QAction *action)
{

     int mode=action->data().toInt();

     mSettings->setViewMode(mode);

}

/*********************************************************************
 *
 *********************************************************************/
//void Actions::openNewTab()
//{


//     emit sendNewTab(m_dirPath);

//}

/*********************************************************************
 *
 *********************************************************************/
void Actions::openInNewTab()
{
    QString path=m_selectedPath;
    if(path==D_TRASH || path==D_SEARCH)
        path=QDir::homePath();

     emit sendNewTab(path);

}

/*********************************************************************
 *
 *********************************************************************/


/*********************************************************************
 *
 *********************************************************************/
void Actions::openInTerminal()
{

     QString exec=mSettings->terminal();
     if(!EMimIcon::findProgram(exec)){
         exec=EMimIcon::defaultTerminal();
     }

//     QString path=actOpenTerminal->data().toString();
//     if(path.isEmpty()) path=m_dirPath;

     QString path=m_selectedPath;
     if(path==D_TRASH || path==D_SEARCH)
         path=QDir::homePath();
       QProcess proc;
     proc.setWorkingDirectory(path);
       QDir::setCurrent(path);
     proc.startDetached(exec,QStringList(),path);


}


//void Actions::openTerminal()
//{

//    QString exec=mSettings->terminal();
//    qDebug()<<"openTerminal"<<exec;
//    if(!EMimIcon::findProgram(exec)){
//        exec=EMimIcon::defaultTerminal();
//    }
//qDebug()<<"openTerminal"<<exec;
//    QProcess proc;
//    proc.setWorkingDirectory(m_dirPath);
//    QDir::setCurrent(m_dirPath);
//    proc.startDetached(exec,QStringList(),m_dirPath);


//}

/*********************************************************************
 *
 *********************************************************************/
void Actions::toggleSortBy(QAction* act)
{


     int data=act->data().toInt();
     mSettings->setSortBy(data);

}

/*********************************************************************
 *
 *********************************************************************/
void Actions::addClosedTab(const QString &url)
{


     foreach (QAction *s, mMenuClosedTab->actions()) {
          if(s->text()==url)return;
     }
     QAction *act=new QAction(EIcon::fromTheme("folder"),url,this);
     act->setData(url);
     //     connect(act,SIGNAL(triggered()),this,SLOT(setcurDir(QString)));
     mMenuClosedTab->insertAction(mMenuClosedTab->actions().first(),act);
     if(mMenuClosedTab->actions().count()>7)
          delete mMenuClosedTab->actions().last();


}

/*********************************************************************
 *
 *********************************************************************/
//void Actions::slotZoomIn()
//{


//     int size=mSettings->iconSize()+8;

//     if(size>128)size=128;
//     mSettings->setIconSize(size);


//}

/*********************************************************************
 *
 *********************************************************************/
//void Actions::slotZoomOut()
//{


//     int size=mSettings->iconSize()-8;

//     if(size<16)size=16;
//     mSettings->setIconSize(size);


//}
/*********************************************************************
 *
 *********************************************************************/
void Actions::chargeAppService(const QString &path)
{
qDebug()<<"chargeAppService"<<path;
    QDirIterator it(path, QStringList("*.desktop"),
                    QDir::Files | QDir::NoDotAndDotDot);
    while (it.hasNext())
    {
 it.next();
        qDebug()<<"chargeAppService"<<it.filePath();
        mListService.append(it.filePath());


    }

}
/*********************************************************************
 *
 *********************************************************************/
QMenu *Actions::menuService(const QStringList &files,const QString &mim)
{

qDebug()<<"menuService files"<<files;
     foreach (QAction *act, mMenuService->actions()) {
          delete act;
     }
     mMenuService->clear();

     foreach (QString filePath, mListService)
     {

         QSettings setting(filePath,QSettings::IniFormat);
         setting.beginGroup("Desktop Entry");
         setting.setIniCodec(QTextCodec::codecForName("UTF-8"));
         QStringList list=setting.value("MimeType").toStringList();
         // QString str=setting.value("MimeType").toString();
         //QStringList list;
         // list=str.split(";");

         //  qDebug()<<mim<<filePath;

         bool exist=false;
         foreach (QString s, list) {
             if(s==mim){exist=true;break;}
             if(s=="all/all"){exist=true;break;}
             if(s.endsWith("/*")){
                 QString mimstart=mim.section("/",0,0);
                 qDebug()<<"mimstart"<<mimstart;
                 if(s.startsWith(mimstart)){exist=true;break;}
             }
         }
         if(exist)
         {
             QString icon=setting.value("Icon").toString();
             QString txt=setting.value("Name").toString();
             QString txtLng=setting.value("Name["+m_lC+"]",txt).toString();
             QAction *act=new QAction(EIcon::fromTheme(icon),txtLng,this);
             QString exec=setting.value("Exec").toString();
             QString arg=files.join("\" \"");
             exec.replace("%F","\""+arg+"\"");
             QString file=files.at(0).trimmed();

             if(exec.startsWith("elokab-terminal"))
                 exec.replace("%f"," \"'"+file+"'\" ");
             else
                 exec.replace("%f"," \""+file+"\" ");

             QFileInfo fi(files.at(0));

             if(exec.startsWith("elokab-terminal"))
                 exec.replace("%n","\"'"+fi.baseName()+"'\"");
             else
                 exec.replace("%n","\""+fi.baseName()+"\"");

             act->setData(exec);
             qDebug()<<"act->setData"<<exec;
             connect(act,SIGNAL(triggered()),this,SLOT(execService()));
             mMenuService->addAction(act);
         }
         setting.endGroup();

     }
     return mMenuService;


}

/*********************************************************************
 *
 *********************************************************************/
void Actions::execService()
{


     QAction *action = qobject_cast<QAction *>(sender());
     if(action)
     {
          QString data=action->data().toString();

          QProcess proc;
qDebug()<<"data exec"<<data;

          proc.setWorkingDirectory(m_dirPath);
          QDir::setCurrent(m_dirPath);


          proc.startDetached(data);

     }

}

/*********************************************************************
 *
 *********************************************************************/


///*********************************************************************
// *
// *********************************************************************/
//void Actions::chooseTerminal()
//{
//    bool ok;
//       QString text = QInputDialog::getText(0, tr("Shoose Terminaml"),
//                                            tr("Name:"), QLineEdit::Normal,
//                                            mSettings->terminal(), &ok);
//       if (ok && !text.isEmpty())
//       mSettings->setTerminal(text);
//}


/*********************************************************************
 *                        ARCHIVE
 *********************************************************************/
QAction *Actions::AddArchiveAction(const QStringList &files)
{
    actAddArchive->setData(files);
    return actAddArchive;
}

void Actions::addArchive()
{

    QFileInfo info=m_dirPath;
    QStringList list;
    list=actAddArchive->data().toStringList();

    if(list.count()<1)return;

    if(list.count()==1) info.setFile(list.at(0));

    QString archName=info.baseName();

    // DIALOG----------
    bool ok;

    QString text = QInputDialog::getText(nullptr, tr("Create archive"),
                                         tr("Archive name:"), QLineEdit::Normal,
                                         archName, &ok);
    if (ok && !text.isEmpty())
        archName=text;
    else
        return;
    // DIALOG ~----------

    for (int i = 0; i < list.count(); ++i) {
        QFileInfo fi(list.at(i));
        list.replace(i,fi.fileName());
    }

    list.insert(0,archName+".tar.gz");
    list.insert(0,"-zcvf");

    QProcess p;
    p.startDetached("tar",list,m_dirPath);

    qDebug()<<"tar"<<"-cvf"<<archName<<m_dirPath;


}

//_________________________________________________________________________________
QAction *Actions::extractHereAction(const QString &file)
{

    actExtractHere->setData(file);
    return actExtractHere;

}



void Actions::archiveExtractHere()
{
    bool exist=false;
    QString prog;
    QString arg;
QFileInfo info=actExtractHere->data().toString();
    QSettings setting(QApplication::organizationName(),"Archiver");

    //DEFAULT
    setting.beginGroup("Default");
    prog = setting.value("File").toString();
    arg = setting.value("Arg").toString();
    if(EMimIcon::findProgram(prog)){ exist=true; }
    setting.endGroup();
    //Find Any Archive

    if(!exist){
        int size = setting.beginReadArray("Prog");
        for (int i = 0; i < size; ++i) {
            setting.setArrayIndex(i);
            prog = setting.value("File").toString();
            arg = setting.value("Arg").toString();
            if(EMimIcon::findProgram(prog)){
                exist=true;
                break;
            }
        }
    }
    setting.endArray();

    QProcess p;

    if(!exist)
    {
//        prog=Edir::libDir()+"/atool";
//        arg="-x";
//        p.startDetached("perl",QStringList()<<prog<<arg<<info.filePath(),info.path());
//        return;
    }


    //QString cmd="sh "+prog+" "+arg +" "+
    p.startDetached(prog,QStringList()<<arg<<info.filePath(),info.path());

    qDebug()<<"Extract archive:"<<prog<<arg<<info.filePath()<<info.path();
}
