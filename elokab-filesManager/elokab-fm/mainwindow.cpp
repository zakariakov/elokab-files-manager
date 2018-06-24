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
//#include<QStyleFactory>
/*****************************************************************************************************
 *
 *****************************************************************************************************/

MainWindow::MainWindow(QWidget *parent) :
     QMainWindow(parent),
     ui(new Ui::MainWindow)/*,m_fileWatcher(0)*/
{
   // qDebug()<<QStyleFactory::keys();
    QStringList args = QApplication::arguments();
    QString pathUrl=QDir::homePath();
    if(args.count()>1)
    {

        QUrl url(args.at(1));

      //  pathUrl=url.toLocalFile();

         pathUrl=url.toString();
         if(pathUrl.startsWith("file://"))
             pathUrl.remove("file://");
    }

    qWarning("***********************************************************************************************\n"
             "** ELOKAB FILES MANAGER VERTION %s BY ABOU ZAKARIA LICENCE GPL 3                            **\n"
             "**ELOKAB FM WAS STARTED IN NORMAL MODE. IF YOU WANT TO SEE DEBUGGING INFORMATION, PLEASE USE:**\n"
             "** elokab-fm --debug                                                                         **\n"
             "***********************************************************************************************\n"
             , qPrintable(QApplication::applicationVersion()));

    ui->setupUi(this);

//    mProgressBar=new QProgressBar(ui->statusBar);
//    mProgressBar->setRange(0,1);
//    mProgressBar->setTextVisible(false);
//    mProgressBar->setMaximumHeight(15);
//    mProgressBar->setVisible(false);

    if(QIcon::themeName()=="hicolor"||QIcon::themeName().isEmpty())
    {

        mElokabSettings=new ElokabSettings(this);
        mElokabSettings->sync();
        connect(mElokabSettings,SIGNAL(iconThemeChanged()) ,this,SLOT(refreshIcons()));
           loadIconThems();

    }
    mSettings=new Settings ;


    EMimIcon::updateMimeAssociatedApplication();

    mActions  =new Actions(mSettings,locale().name().section("_",0,0));
    mIconProvider=new IconProvider;
    mIconProvider->setlocale(locale().name().section("_",0,0));
    mTab =new Tab(mSettings,mIconProvider,mActions,this);
    ui->vLayoutCenter->addWidget(mTab);
    placesTree= new PlaceTree(this);
    ui->verticalLayoutPlace->addWidget(placesTree);
    QMenu  *menufile=new QMenu(tr("&File"));
    menufile->addActions(mActions->menuFile()->actions());
    menufile->addSeparator();
    menufile->addAction(mActions->actQuit);
    ui->menuBar->addMenu(menufile);

    ui->menuBar->addMenu(mActions->menuView());
    ui->menuBar->addMenu(mActions->menuEdit());
    ui->menuBar->addMenu(mActions->menuGo());
    ui->menuBar->addMenu(mActions->menuTools());
    ui->menuBar->addMenu(mActions->menuSettings());
    QMenu *menuHelp=new QMenu(tr("Help"));
    ui->menuBar->addMenu(menuHelp);
    menuHelp->addAction(EIcon::fromTheme("help-about","help-about"),tr("&About"),this,SLOT(showAboutThis()));
    //   menuHelp->addAction(tr("About &Qt"),qApp,SLOT(aboutQt()));
    menuHelp->addAction(EIcon::fromTheme("help"),tr("&Help"),this,SLOT(showHelp()));

    addActions(mActions->listActions());

    myModel = new QFileSystemModel(this);
    myModel->setRootPath(QDir::root().absolutePath());
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


    mActions->actPathWidget->setDefaultWidget(pathWidget);

      setupToolBarActions();


    if(QApplication::layoutDirection()==Qt::RightToLeft)
        addDockWidget(Qt::RightDockWidgetArea, ui->dockWidget);


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
    connect(mTab,SIGNAL(urlChanged(QString)),pathWidget,SLOT(setUrl(QString)));
    connect (this,SIGNAL(closeAll()),mTab,SLOT(closeAll()));
    connect(mTab,SIGNAL(  selectedFoldersFiles(QString)),this,SLOT(setSelectedFoldersFiles(QString)));
    connect(ui->mainToolBar,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(toolCustomContextMenu(QPoint)));
    connect(ui->dockWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(toolCustomContextMenu(QPoint)));
    connect(mFileInfo,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(toolCustomContextMenu(QPoint)));
  //  connect(ui->dockWidgetTerminal,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(toolCustomContextMenu(QPoint)));

    connect(filterBar,SIGNAL(filterChanged(QString)),mTab,SLOT(setModelFiltrer(QString)));
    connect(mActions,SIGNAL(showFilterBar()),this,SLOT(showHidFilterBar()));
    connect(mSettings,SIGNAL(showFolderNavigationChanged(bool)),this,SLOT(stackSetCurrentIndex(bool)));
    connect(mSettings,SIGNAL(showInformationChanged(bool)),mFileInfo,SLOT(setVisible(bool)));
    connect(mActions,SIGNAL(showConfigureTool()),this,SLOT(showConfigureToolBar()));
    connect(mSettings,SIGNAL(showMenuBarChanged(bool)),ui->menuBar,SLOT(setVisible(bool)));
    connect(mSettings,SIGNAL(showToolbarChanged(bool)),ui->mainToolBar,SLOT(setVisible(bool)));
    connect(mSettings,SIGNAL(singleclickChanged()),this,SLOT(changeSingleClick()));
   // connect(mSettings,SIGNAL(showTerminalToolChanged(bool)),ui->dockWidgetTerminal,SLOT(setVisible(bool)));
   // connect(ui->dockWidgetTerminal,SIGNAL(visibilityChanged(bool)),mSettings,SIGNAL(showTerminalToolChanged(bool)));
   // connect(ui->dockWidgetTerminal,SIGNAL(visibilityChanged(bool)),this,SLOT(terminalVisibilityChanged(bool)));

   // connect(mTab,SIGNAL(largeDirectoryChanged(bool)),this,SLOT(showProgress(bool)));




    QSettings settings("elokab", "elokabFm");
    settings.beginGroup("Window");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();

    changeSingleClick();
    ui->dockWidget->setMinimumWidth(200);
    //user  settings
    mFileInfo->setVisible(mSettings->showInformation());
    ui->mainToolBar->setVisible(mSettings->showToolbar());
    stackSetCurrentIndex(mSettings->folderNavigation());
    ui->menuBar->setVisible(mSettings->showMenuBar());
  //  ui->dockWidgetTerminal->setVisible(mSettings->showTerminalTool());
    //--------------defaul-----------------
    mTab->addNewTab(pathUrl);

    QFileInfo fi(pathUrl);
    if(!fi.isDir())
        pathUrl=fi.path();

   // mTermWidget->setInitialWorkingDirectory(pathUrl);
   // mTermWidget->startShellProgram();

    setUrl(pathUrl);

#ifdef DEBUG_APP
    Messages::showMessage(Messages::END,"MainWindow::MainWindow()");
#endif

}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::changeSingleClick()
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MainWindow::MainWindow()");
#endif

  mTab->   setStyleSheet( QString("QAbstractItemView {activate-on-singleclick : %1; }")
                    .arg(mSettings->isSingleclick()));

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"MainWindow::MainWindow()");
#endif
}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::closeEvent(QCloseEvent *)
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MainWindow::closeEvent()");
#endif

     emit closeAll();
     //
#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"MainWindow::closeEvent()");
#endif

}
/**************************************************************************************
 *
 **************************************************************************************/
MainWindow::~MainWindow()
{
    QSettings settings("elokab", "elokabFm");
    settings.beginGroup("Window");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

         QString homeThambnail=Edir::cachDir()+"/thambnail";
         QDir dir(homeThambnail);
         QTextCodec *codec = QTextCodec::codecForName("UTF-8");
         foreach (QString subfile, dir.entryList(QDir::Files|QDir::NoDotAndDotDot))
         {
             QByteArray encodedString =QByteArray::fromHex(subfile.toUtf8());

             QString string = codec->toUnicode(encodedString);
             QFile file(string);
             if(!file.exists()){
                QFile::remove(homeThambnail+"/"+subfile);
              //  qDebug()<<subfile<<"noexist";
             }
         }
         QSettings setting("elokab","thambnail");
         foreach (QString s, setting.allKeys()) {
         //    qDebug()<<"setting"<<"/"+s;
             if(!QFile::exists("/"+s))
                setting.remove(s);
         }
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

}
/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::toolCustomContextMenu(QPoint)
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MainWindow::toolCustomContextMenu()");
#endif

     QMenu menu;
     menu.addActions(mActions->menuPanels()->actions());
     menu.addSeparator();
     menu.addAction(mActions->actConfigTool);
     menu.exec(QCursor::pos());

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"MainWindow::toolCustomContextMenu()");
#endif
}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::showHidFilterBar()
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MainWindow::showHidFilterBar()");
#endif

     if(filterBar->isHidden())
          filterBar->show();
     else
          filterBar->close();

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"MainWindow::showHidFilterBar()");
#endif
}
/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::setUrl(QString url)
{


     m_mainUrl=url;
     placesTree->setCurentUrl(url);
     mTab->setUrl(url);
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


}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::setSelectedFoldersFiles(  QString msg)
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MainWindow::setSelectedFoldersFiles()");//
#endif

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
                    ui->statusBar->showMessage(QFileInfo(msg).fileName()
                                               +"   "+EMimIcon::formatSize(QFileInfo(msg).size()));


          } else if(msg.startsWith("fileName://")){
               ui->statusBar->showMessage(msg.remove("fileName://"));

          }else{
               mFileInfo->setFileName(msg);
               ui->statusBar->showMessage(msg);
          }

     }else{
          calculatFiles();
     }

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"MainWindow::setSelectedFoldersFiles()");//
#endif
}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::calculatFiles()
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MainWindow::calculatFiles()");
#endif

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

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"MainWindow::calculatFiles()");
#endif
}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::setUrl(QModelIndex index)
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MainWindow::setUrl()");
#endif

     QString dir=  myModel->filePath(index);
     mTab->setUrl(dir);
//  pathWidget->setUrl(dir);
#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"MainWindow::setUrl()");
#endif
}

/**************************************************************************************
 *
 **************************************************************************************/
//void MainWindow::urlChanged(const QString &url)
//{
//#ifdef DEBUG_APP
//     Messages::showMessage(Messages::BEGIN,"MainWindow::urlChanged()"+url);
//#endif

//    mTab->setUrl(url);
// //  pathWidget->setUrl(url);
//  //  if(mTermWidget->isVisible()){


//   // }
//#ifdef DEBUG_APP
//     Messages::showMessage(Messages::END,"MainWindow::urlChanged()"+url);
//#endif
//}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::stackSetCurrentIndex(bool arg)
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MainWindow::stackSetCurrentIndex()");
#endif

     ui->stackedWidget->setCurrentIndex(arg);

     if(arg)
          ui->dockWidget->setWindowTitle(tr("Folders"));
     else
          ui->dockWidget->setWindowTitle(tr("Places"));

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"MainWindow::stackSetCurrentIndex()");
#endif
}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::showConfigureToolBar()
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MainWindow::showConfigureToolBar()");// MainWindow::showConfigureToolBar()";
#endif

     DialogActions *dlg=new DialogActions(this);

     dlg->setListActions(mActions->listActions(),mSettings->listActions());

     if(dlg->exec()==QDialog::Accepted){
          //  mSettings->setListToolBarActions(dlg->getListActToAdd());
          mSettings->setListActions(dlg->listAvailableActions());

           setupToolBarActions();
     }

     delete dlg;

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"MainWindow::showConfigureToolBar()");// MainWindow::showConfigureToolBar()\n";
#endif
}

/**************************************************************************************
 *
 **************************************************************************************/
void MainWindow::setupToolBarActions()
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MainWindow::setupToolBarActions()");// MainWindow::setupToolBarActions()";
#endif

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

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"MainWindow::setupToolBarActions()");
#endif
}
/*****************************************************************************************************
 *
 *****************************************************************************************************/
void MainWindow::loadIconThems()
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"MainWindow::loadIconThems()");// MainWindow::loadIconThems()";
#endif

     QString icnThem=QIcon::themeName();

     QByteArray sS=qgetenv("DESKTOP_SESSION");

     if(sS=="elokab-session"){
         QSettings setting("elokab","elokabsettings");
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


  #ifdef DEBUG_APP
     Messages::showMessage(Messages::NORMALE,"MainWindow::loadIconThems()",QIcon::themeName());
     Messages::showMessage(Messages::END,"MainWindow::loadIconThems()");// MainWindow::loadIconThems()\n";
#endif
}
void MainWindow::refreshIcons()
{
//    mIconProvider->clearCache();
 //   myModel->setIconProvider(mIconProvider);
//    mTab->refreshIcons();
    placesTree->refreshIcons();
//    mActions->refreshIcons();
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
