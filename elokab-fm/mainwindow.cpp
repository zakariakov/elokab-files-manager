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

#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <EMimIcon>
#include "dialogactions.h"
#include  "messages.h"
#include  "about/widgetabout.h"
#include <QDebug>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QMetaProperty>
#include <QTextCodec>
#include<QDesktopServices>
#include<QUrl>
#include<QClipboard>
#include<QSplitter>

QIcon MyIconProvider::icon(const QFileInfo &info)const
{
    if(info.isDir()){
        QIcon icon;
        icon=   EMimIcon::iconFolder(info.absoluteFilePath());
        if(info.isSymLink()){
            icon=  EMimIcon::iconSymLink(icon);
        }
        return icon;
    }

    return QFileIconProvider::icon(info);
}

/*****************************************************************************************************
 *
 *****************************************************************************************************/
MainWindow::MainWindow( QString pathUrl,QWidget *parent) :
     QMainWindow(parent),
     ui(new Ui::MainWindow)/*,m_fileWatcher(0)*/
{


    if(pathUrl.isEmpty())
       pathUrl=QDir::homePath();
//    if(args.count()>1)
//    {

//        QUrl url(args.at(1));

//      //  pathUrl=url.toLocalFile();

//         pathUrl=url.toString();
//         if(pathUrl.startsWith("file://"))
//             pathUrl.remove("file://");
//    }

    qWarning("***********************************************************************************\n"
             "** ELOKAB FILES MANAGER VERTION %s BY ABOUZAKARIA LICENCE GPL 3                  **\n"
             "** ELOKAB FM WAS STARTED IN NORMAL MODE.                                         **\n"
             "** IF YOU WANT TO SEE DEBUGGING INFORMATION, PLEASE USE:  elokab-fm --debug      **\n"
             "***********************************************************************************\n"
             , qPrintable(QApplication::applicationVersion()));

Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__);
    ui->setupUi(this);


    if(QIcon::themeName()=="hicolor"||QIcon::themeName().isEmpty())
    {

        mElokabSettings=new ElokabSettings(this);
        mElokabSettings->sync();
        connect(mElokabSettings,SIGNAL(iconThemeChanged()) ,this,SLOT(refreshIcons()));
           loadIconThems();

    }
    mSettings=new Settings ;

    EMimIcon::setlocale(locale().name().section("_",0,0));
    EMimIcon::updateMimeAssociatedApplication();

    mActions  =new Actions(mSettings,locale().name().section("_",0,0));
    mIconProvider=new MyIconProvider;
   // mIconProvider->setlocale(locale().name().section("_",0,0));
    mTab =new Tab(mSettings,mActions,this);
    ui->vLayoutCenter->addWidget(mTab);
    placesTree= new PlaceTree(this);
    ui->verticalLayoutPlace->addWidget(placesTree);
    QMenu  *menufile=new QMenu(tr("&File"));
    menufile->addActions(mActions->menuFile()->actions());
     menufile->addAction(mActions->propertiesAction());
    menufile->addSeparator();
    menufile->addAction(mActions->quitAction());
    ui->menuBar->addMenu(menufile);

    ui->menuBar->addMenu(mActions->menuView());
    ui->menuBar->addMenu(mActions->menuEdit());
    ui->menuBar->addMenu(mActions->menuGo());
    ui->menuBar->addMenu(mActions->menuTools());
   // ui->menuBar->addMenu(mActions->menuSettings());
    QMenu *menuHelp=new QMenu(tr("Help"));
    ui->menuBar->addMenu(menuHelp);
    menuHelp->addAction(EIcon::fromTheme("help-about","help-about"),tr("&About"),this,SLOT(showAboutThis()));
    //   menuHelp->addAction(tr("About &Qt"),qApp,SLOT(aboutQt()));
    menuHelp->addAction(EIcon::fromTheme("help"),tr("&Help"),this,SLOT(showHelp()));

    addActions(mActions->listActions());

    QToolButton *btn=new QToolButton;
    btn->setIconSize(QSize(16,16));
    btn->setAutoRaise(true);
    btn->setDefaultAction(mActions->folderPlaceAction());
    ui->hLayoutPlace->addWidget(btn);
    //
    myModel = new QFileSystemModel(this);
    myModel->setRootPath(QDir::rootPath());
    myModel->setFilter(QDir::AllDirs|QDir::NoDotAndDotDot);

    myModel->setIconProvider(mIconProvider);
    myModel->setResolveSymlinks(true);

    ui->treeView->setModel(myModel);
    QModelIndex index= myModel->index(QDir::root().absolutePath());
    ui->treeView->setRootIndex(index);

    ui->treeView->setHeaderHidden(true);



    mFileInfo=new FileInformation(this);
    ui->hLayoutCenter->addWidget(mFileInfo);

    pathWidget=new PathWidget(this);
    //     pathWidget->setUrl(path);


    mActions->pathWidgetAction()->setDefaultWidget(pathWidget);
     //
      setupToolBarActions();


//    if(QApplication::layoutDirection()==Qt::RightToLeft)
//        addDockWidget(Qt::RightDockWidgetArea, ui->dockWidget);


    for (int i = 1; i <  myModel->columnCount(); ++i)
    {
        ui->treeView->hideColumn(i);
    }

    filterBar =new FilterBar;
    ui->vLayoutCenter->addWidget(filterBar);
    filterBar->hide();

    connect(placesTree,SIGNAL(urlPlacesChanged(QString)),mTab,SLOT(setUrl(QString)));
    connect(placesTree,SIGNAL(slotOpenNewTab(QString)),mTab,SLOT(addNewTab(QString)));
    connect(pathWidget,SIGNAL(urlChanged(QString)),mTab,SLOT(setUrl(QString)));

    connect(ui->treeView,SIGNAL(activated(QModelIndex)),this,SLOT(setUrl(QModelIndex)));
    connect(ui->treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(setUrl(QModelIndex)));
    connect(mTab,SIGNAL(urlChanged(QString)),this,SLOT(setUrl(QString)));
   connect(mTab,SIGNAL(tabAdded(QString)),this,SLOT(setUrl(QString)));
    connect (this,SIGNAL(closeAll()),mTab,SLOT(closeAll()));
    connect(mTab,SIGNAL(  selectedFoldersFiles(QString)),this,SLOT(setSelectedFoldersFiles(QString)));
    connect(ui->mainToolBar,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(toolCustomContextMenu(QPoint)));
    //connect(ui->dockWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(toolCustomContextMenu(QPoint)));
    connect(mFileInfo,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(toolCustomContextMenu(QPoint)));

    connect(filterBar,SIGNAL(filterChanged(QString)),mTab,SLOT(setModelFiltrer(QString)));
    connect(mActions,SIGNAL(showFilterBar()),this,SLOT(showHidFilterBar()));
    connect(mSettings,SIGNAL(showFolderNavigationChanged(bool)),this,SLOT(stackSetCurrentIndex(bool)));
    connect(mSettings,SIGNAL(showInformationChanged(bool)),mFileInfo,SLOT(setVisible(bool)));
    connect(mActions,SIGNAL(showConfigureTool()),this,SLOT(showConfigureToolBar()));
    connect(mSettings,SIGNAL(showMenuBarChanged(bool)),ui->menuBar,SLOT(setVisible(bool)));
    connect(mSettings,SIGNAL(showToolbarChanged(bool)),ui->mainToolBar,SLOT(setVisible(bool)));
    connect(mSettings,SIGNAL(singleclickChanged()),this,SLOT(changeSingleClick()));
    connect(mActions,SIGNAL(sendShowSettings()),this,SLOT(showSettings()));



    ui->splitter->setSizes(QList<int>()<<1<<2);
    QSettings settings(QApplication::organizationName(),QApplication::applicationName());
    settings.beginGroup("Window");
    restoreGeometry(settings.value("geometry").toByteArray());
    ui->splitter->restoreState(settings.value("splitterSizes").toByteArray());
    settings.endGroup();

    changeSingleClick();

    //user  settings
    mFileInfo->setVisible(mSettings->showInformation());
    ui->mainToolBar->setVisible(mSettings->showToolbar());
    stackSetCurrentIndex(mSettings->folderNavigation());
    ui->menuBar->setVisible(mSettings->showMenuBar());
     //--------------defaul-----------------


    QFileInfo fi(pathUrl);
    if(!fi.isDir())
        pathUrl=fi.path();

     mTab->addNewTab(pathUrl);

//     setUrl(pathUrl);
  Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__,"End");

}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::changeSingleClick()
{

  mTab->   setStyleSheet( QString("QAbstractItemView {activate-on-singleclick : %1; }")
                    .arg(mSettings->isSingleclick()));

}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::closeEvent(QCloseEvent *)
{
Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__);

     emit closeAll();

Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__,"End");
}
/**************************************************************************************
 *
 **************************************************************************************/
MainWindow::~MainWindow()
{
    Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__);

    QSettings settings(QApplication::organizationName(),QApplication::applicationName());
    settings.beginGroup("Window");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("splitterSizes", ui->splitter->saveState());
    settings.endGroup();

    //mIconProvider->saveCacheIcons();
    blockSignals(true);
    pathWidget->blockSignals(true);
    mTab->blockSignals(true);
    myModel->blockSignals(true);
    placesTree->blockSignals(true);
    mSettings->blockSignals(true);
    mActions->blockSignals(true);
    filterBar->blockSignals(true);

    delete pathWidget;
    delete mTab;
    delete placesTree;
    delete mFileInfo;
    delete myModel;
    delete mActions;
    delete mIconProvider;
    delete mSettings;
    delete filterBar;
//delete mTermWidget;

    delete ui;
Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__,"End");
}
/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::toolCustomContextMenu(QPoint)
{

     QMenu menu;
     menu.addActions(mActions->menuPanels()->actions());
     menu.addSeparator();
     menu.addAction(mActions->configToolAction());
     menu.exec(QCursor::pos());

}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::showHidFilterBar()
{

     if(filterBar->isHidden())
          filterBar->show();
     else
          filterBar->close();

}
/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::setUrl(QString url)
{
Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__,url);

     m_mainUrl=url;
     placesTree->setCurentUrl(url);
     pathWidget->setUrl(url);
     mActions->setUrl(url);
    QDir dir(url);

     QString name=dir.dirName();

     if(dir.path()==QDir::rootPath())
          name=tr("root");

     if(dir.path()==QDir::homePath())
          name=tr("Home");

     setWindowTitle(tr("ElokabFm")+"-"+name);
     QModelIndex index= myModel->index(url);
     ui->treeView->setCurrentIndex((index));
     calculatFiles();

Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__,"End");
}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::setSelectedFoldersFiles(  QString msg)
{

Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__);

     if(!msg.isEmpty())
     {

          if(msg.startsWith("file://"))
          {


               mFileInfo->setFileName(msg);
               msg.remove("file://");
               QFileInfo fi(msg);
               if(fi.isDir())
                    ui->statusBar->showMessage(QFileInfo(msg).fileName());
               else
                    ui->statusBar->showMessage(EMimIcon::formatSize(QFileInfo(msg).size())
                                               +" :  "+QFileInfo(msg).fileName());


          } else if(msg.startsWith("fileName://")){
               ui->statusBar->showMessage(msg.remove("fileName://"));

          }else{
               mFileInfo->setFileName(msg);
               ui->statusBar->showMessage(msg);
          }

     }else{
          calculatFiles();
     }

Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__,"End");
}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::calculatFiles()
{
Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__);

     QDir::Filters filter;
     if(mSettings->showHidden())
          filter=QDir::AllEntries|  QDir::NoDotAndDotDot|QDir::Hidden ;
     else
          filter=QDir::AllEntries|  QDir::NoDotAndDotDot ;

     mFileInfo->setFileName("file://"+m_mainUrl);
     QDir dir(m_mainUrl);
     int folder=0,file=0;
     foreach (QString subfile, dir.entryList(filter))
     {
          QFileInfo fi(m_mainUrl+"/"+subfile);

          if(fi.isDir())
               folder++;

          else
               file++;

     }

     ui->statusBar->showMessage(QString::number(folder)+tr(" Folders ")+QString::number(file)+ tr(" Files"));

Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__,"End");
}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::setUrl(QModelIndex index)
{
Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__);

     QString dir=  myModel->filePath(index);
     mTab->setUrl(dir);
//  pathWidget->setUrl(dir);
Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__,"End");
}


/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::stackSetCurrentIndex(bool arg)
{
Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__);

     ui->stackedWidget->setCurrentIndex(arg);

     if(arg)
          ui->labelPlace->setText(tr("Folders"));
     else
          ui->labelPlace->setText(tr("Places"));

Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__,"End");
}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::showConfigureToolBar()
{
Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__);

     DialogActions *dlg=new DialogActions(this);

     dlg->setListActions(mActions->listActions(),mSettings->listActions());

     if(dlg->exec()==QDialog::Accepted){
          //  mSettings->setListToolBarActions(dlg->getListActToAdd());
          mSettings->setListActions(dlg->listAvailableActions());

           setupToolBarActions();
     }

     delete dlg;

Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__,"End");
}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::setupToolBarActions()
{
Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__);

     ui->mainToolBar->clear();
     foreach (QString s, mSettings->listActions()) {

          if(s.isEmpty())
               continue;
          if(s=="Separator"){
               ui->mainToolBar->addSeparator();
               continue;
          }
          foreach (QAction *act, mActions->listActions()) {

               if(act->objectName()==s){
                    ui->mainToolBar->addAction(act);
               }
          }
     }

Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__,"End");
}
/*****************************************************************************************************
 *
 *****************************************************************************************************/
void MainWindow::loadIconThems()
{
    Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__);

    QString icnThem=QIcon::themeName();

    QByteArray sS=qgetenv("DESKTOP_SESSION");

    if(sS=="elokab-session"){
        QSettings setting(QApplication::organizationName(),"elokabsettings");
        setting.beginGroup("Themes");
        icnThem=  setting.value("iconsTheme",QIcon::themeName()).toString();
        setting.endGroup();

    }


    if(icnThem=="hicolor"||icnThem.isEmpty()){

        QStringList failback;
        failback <<"Adwaita"<< "oxygen"<< "Mint-X"<< "Humanity"<< "Tango"<< "Prudence-icon"<< "elementary"<< "gnome";

        QDir dir("/usr/share/icons/");
        foreach (QString s, failback)
        {
            if (dir.exists(s))
            {
                icnThem=s;

                break;
            }
        }
    }
    if(icnThem.isEmpty())
        icnThem="hicolor";
    QIcon::setThemeName(icnThem);


    Messages::debugMe(0,__LINE__,"MainWindow",__FUNCTION__,"end");
}
void MainWindow::refreshIcons()
{


//    mTab->refreshIcons();
    placesTree->refreshIcons();
//    mActions->refreshIcons();
}
/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::showSettings()
{
     SettingsDlg *mSettingsDlg=new SettingsDlg(mSettings);
     if(mSettingsDlg->exec()==QDialog::Accepted){
         mSettings->setSingleclick(mSettingsDlg->isSingleclick());
         mSettings->setConfirmDragDrop(mSettingsDlg->isConfirmDragDrop());
         mSettings->setRootIsDecorated(mSettingsDlg->isRootDecorated());
         mSettings->setClassicIcons(mSettingsDlg->isClassicIcons());
         mSettings->setTerminal(mSettingsDlg->terminal());
         mSettings->setPdfThumbnails(mSettingsDlg->pdfThumbNails());
         mSettings->setVideoThumbnails(mSettingsDlg->videoThumbNails());
         mSettings->setDoubleClickEdit(mSettingsDlg->doubleClickRemame());

     }

         delete mSettingsDlg;

}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::showAboutThis()
{

    QDialog *dlg=new QDialog;

    WidgetAbout *wid=new WidgetAbout(dlg);
    wid->setPixmap(EIcon::fromTheme("system-file-manager",("folder")).pixmap(64));
    wid->setDescription(tr("File Manager Based on Qt"));
    wid->setApplicationText(tr("Elokab File manager"));
    QVBoxLayout  *verticalLayout = new QVBoxLayout(dlg);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(dlg);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok);
    verticalLayout->addWidget(wid);
    verticalLayout->addWidget(buttonBox);
    QObject::connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    dlg->resize(380,380);
    dlg->exec();
    delete wid;
    delete dlg;

}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::showHelp()
{
     EMimIcon::launchApp("https://sourceforge.net/project/elokab/");
}
