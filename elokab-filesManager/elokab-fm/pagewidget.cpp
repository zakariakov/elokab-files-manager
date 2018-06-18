#include "pagewidget.h"
//#include <EMimIcon>
#include "filesutils/propertiesdlg.h"
#include "filesutils/openwithdlg.h"
//#include "itemdelegate.h"
#include "messages.h"
//#ifdef DEBUG_APP
#include <QDebug>
//#endif
#include <QTimer>
#include <QApplication>
#include <QSortFilterProxyModel>

/*****************************************************************************************************
 *
 *****************************************************************************************************/
PageWidget::PageWidget(QFileSystemModel *model,
                       Settings *setting,
                       Actions *action,
                       const QString &url,
                       IconProvider *iconProvider,
                       QWidget *parent) :
     QWidget(parent),
     mSettings(setting),
     myModel( model),
     mActions(action),
     propertiesDlg(0),
     openWithDlg(0)
{
     setObjectName("_pageWidget");

#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"PageWidget::PageWidget()");
#endif



    setObjectName("PageWidget");

     mViewMode=mSettings->viewMode();
     //seup tools
     QVBoxLayout *vLayout = new QVBoxLayout(this);
     vLayout->setSpacing(0);
     vLayout->setMargin(0);

     stackedWidget = new QStackedWidget(this);

     QWidget *pageList = new QWidget(this);
     QVBoxLayout *vLayoutList = new QVBoxLayout(pageList);
     vLayoutList->setSpacing(6);
     vLayoutList->setContentsMargins(0,0,0,0);
     listView = new MyListView(myModel,mActions,pageList);

//     listView->setItemDelegate(new ItemDelegate);

     vLayoutList->addWidget(listView);
     stackedWidget->addWidget(pageList);

     QWidget *pageTree = new QWidget();
     QVBoxLayout *vLayoutTree = new QVBoxLayout(pageTree);
     vLayoutTree->setSpacing(6);
     vLayoutTree->setContentsMargins(0,0,0,0);
     treeView = new MyTreeView(myModel,pageTree);
     vLayoutTree->addWidget(treeView);
     stackedWidget->addWidget(pageTree);


    QWidget *pageTrash = new QWidget();
     QVBoxLayout *vLayoutTrash = new QVBoxLayout(pageTrash);
     vLayoutTrash->setSpacing(6);
     vLayoutTrash->setContentsMargins(0,0,0,0);
     trashView = new TrashView(iconProvider,mActions,pageTrash);

     vLayoutTrash->addWidget(trashView);
     stackedWidget->addWidget(pageTrash);

     searchView = new SearchView(this);
     stackedWidget->addWidget(searchView);
     connect(searchView,SIGNAL(searchingCanceled(QString)),this,SLOT(urlChanged(QString)));
     connect(stackedWidget,SIGNAL(currentChanged(int)),this,SLOT(viewChangged(int)));

     vLayout->addWidget(stackedWidget);
     listSelectionModel=listView->selectionModel();
     treeView-> setSelectionModel(listView->selectionModel());
     //searchView->searchTreeView()->setSelectionModel(listSelectionModel);
     treeView->setContextMenuPolicy(Qt::CustomContextMenu);
     connect(treeView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));
     listView->setContextMenuPolicy(Qt::CustomContextMenu);
     connect(listView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));
     searchView->setContextMenuPolicy(Qt::CustomContextMenu);
     connect(searchView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));


     listView->setViewIconSize(mSettings->viewIconSize());
     treeView->setTreeIconSize(mSettings->treeIconSize());


  //   connect(mSettings,SIGNAL(iconSizeChanged(int)),this,SLOT(setZoomIn(int)));
     connect(mActions,SIGNAL(zoomInChanged()),this,SLOT(setZoomIn()));
     connect(mActions,SIGNAL(ZoomOutChanged()),this,SLOT(setZoomOut()));

     connect(mSettings,SIGNAL(rootDecorationChanged(bool)),treeView,SLOT(setExpandable(bool)));

     connect(searchView,SIGNAL(setUrl(QString)),this,SLOT(setUrl(QString)));
     connect(searchView,SIGNAL(showOpenwithDlg(QString)),this,SLOT(showOpenwithDlg(QString)));
     connect(searchView,SIGNAL(fileSelected(QString)),this,SIGNAL(selectedFoldersFiles(QString)));
     connect(trashView,SIGNAL(fileSelected(QString)),this,SIGNAL(selectedFoldersFiles(QString)));

     connect(listSelectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(selectionHasChanged(QItemSelection,QItemSelection)));

     connect(this,SIGNAL(historyBackAvailable(bool)),mActions,SIGNAL(backAvailable(bool))) ;
     connect(this,SIGNAL(historyForwardAvailable(bool)),mActions,SIGNAL(forwardAvailable(bool))) ;
     connect(this,SIGNAL(selectedAvailabe(bool)),mActions,SIGNAL(selectionAvialable(bool)));

     connect(this,SIGNAL(historyBackAvailable(bool)),this,SLOT(setBackEnabled(bool)));
     connect(this,SIGNAL(historyForwardAvailable(bool)),this,SLOT(setForwardEnabled(bool)));

     connect(listView,SIGNAL(activated(QModelIndex)),this,SLOT(slotItemActivated(QModelIndex)));
     connect(treeView,SIGNAL(activated(QModelIndex)),this,SLOT(slotItemActivated(QModelIndex)));
   //  connect(listView,SIGNAL(pressed(QModelIndex)),this,SLOT(slotItemPressed(QModelIndex)));
    // connect(treeView,SIGNAL(pressed(QModelIndex)),this,SLOT(slotItemPressed(QModelIndex)));
     connect(listView,SIGNAL(fileEntered(QString)),this,SIGNAL(selectedFoldersFiles(QString)));


     treeView->setRootIsDecorated(mSettings->rootIsDecorated());
     setUrl(url);

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"PageWidget::PageWidget()");

#endif
}

//___________________________________________________________________
PageWidget::~PageWidget()
{

#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"PageWidget::~PageWidget()");
#endif
     searchView->stopSearch();
     searchView->close();
     // delete searchView;
     delete listView;
     delete treeView;
     delete trashView;
}
void PageWidget::closeAll()
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"PageWidget::closeAll()");
#endif
     searchView->stopSearch();
     //searchView->close();
     //  delete searchView;
}
/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::customContextMenu(QPoint)
{

     int count=0;
     QString selectedPath;
     QFileInfo info;
     QMenu menu;
     bool isSearch=false;

     switch (focusedWidget()) {
     case WListView:
         selectedPath=myModel->filePath(listView->currentIndex());
         info=myModel->fileInfo(listView->currentIndex());
         count=listSelectionModel->selectedIndexes().count();
         break;

     case WTreeView:
         selectedPath=myModel->filePath(treeView->currentIndex());
         info=myModel->fileInfo(treeView->currentIndex());
         count=listSelectionModel->selectedIndexes().count();
         break;

     case WSearchView:
         selectedPath=searchView->selectedFile();
         info=QFileInfo(selectedPath);
         count=searchView->selectedFilesCount();
         isSearch=true;
         break;

     default:
         return;
     }
   /*
     if(focusWidget()==listView)
     {
          QModelIndex cIndex=   listView->currentIndex();
          selectedPath=myModel->filePath(cIndex);
          info=myModel->fileInfo(cIndex);
          count=listSelectionModel->selectedIndexes().count();
     }else if(focusWidget()==treeView){
          QModelIndex cIndex=  treeView->currentIndex();
          selectedPath=myModel->filePath(cIndex);
          info=myModel->fileInfo(cIndex);
          count=listSelectionModel->selectedIndexes().count();
     }else if(focusWidget()==searchView->searchTreeView()){
          selectedPath=searchView->selectedFile();
          info=QFileInfo(selectedPath);
          count=searchView->selectedFilesCount();
          isSearch=true;
     }else{
          return;
     }

*/

     QString mim=EMimIcon::mimeTyppe(info);
     if(!isSearch)
          menu.addMenu(mActions->menuNew());//nonsearch


     if(count>0)
     {
          if(isSearch){
               emit selectedAvailabe(true);
               mActions->actPaste->setEnabled(false);
               if(info.isDir())
               {
                    menu.addAction(mActions->actOpenTerminal);
                    menu.addAction(mActions->actOpenInNewTab);
                    menu.addSeparator();
                    mActions->setcurDir(selectedPath);

               }else{
                    menu.addAction(mActions->actOpenInNewTab);
                    QFileInfo fi(selectedPath);
                    mActions->setcurDir(fi.path());
                    if(mim!= "application/x-executable" )
                         menu.addMenu(mActions->menuOpenWith(selectedPath, mim))  ;

               }

          }
          else
          {

          //     menu.addMenu(mActions->menuNew());//nonsearch
               if(info.isDir())
               {
                 //   menu.addAction(mActions->actOpenInTerminal);
                    menu.addAction(mActions->actOpenInNewTab);
                    menu.addAction(mActions->actOpenTerminal);
                    mActions->setcurDir(selectedPath);

               }else{
                    mActions->setcurDir(m_dirPath);
                    if(mim!= "application/x-executable" )
                         menu.addMenu(mActions->menuOpenWith(selectedPath, mim))  ;

               }
              if(selectedFiles().count()>0)
               menu.addMenu(mActions->menuService(selectedFiles(),mim));//nonsearch
               else
               menu.addMenu(mActions->menuService(QStringList()<<m_dirPath,mim));

          }

          menu.addSeparator();
     }
     else{
     menu.addMenu(mActions->menuService(QStringList()<<m_dirPath,"inode/directory"));
     menu.addAction(mActions->actOpenTerminal);
}

     menu.addActions(mActions->menuViewfile()->actions());
     menu.exec(QCursor::pos());




//     return;

}

/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::slotItemActivated(QModelIndex index)
{


     if(listSelectionModel->selectedIndexes().count()>0)
     {

          if(myModel->isDir( listSelectionModel->currentIndex()))
          {
               setUrl(myModel->filePath(index));
          }else{

               QString mim=EMimIcon::mimeTyppe(myModel->fileInfo(index));
               if (EMimIcon::launchApp(myModel->filePath(index),mim)==false)
                    showOpenwithDlg(myModel->filePath(index));

          }

     }


}

/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::slotItemPressed(QModelIndex /*index*/)
{


     if(listSelectionModel->selectedIndexes().count()>0){

          if(myModel->isDir( listSelectionModel->currentIndex())){


          }else{
#ifdef DEBUG_APP
               //  QFile::Permissions per=myModel->permissions(index);
               //    QFlags<QFile::Permissions> perms = myModel->permissions(index);
               //  qDebug()<<" PageWidget::slotItemPressed()================permissions"
               //                      << (perms.testFlag(QFile::ReadOwner) ? "r" : "-")
               //                      <<(perms.testFlag(QFile::WriteOwner) ? "w" : "-" )
               //                     <<(perms.testFlag(QFile::ExeOwner) ? "x" : "-" )
               //                    <<(perms.testFlag(QFile::ReadGroup) ? "r" : "-" )
               //                   <<(perms.testFlag(QFile::WriteGroup) ? "w" : "-" )
               //                  <<(perms.testFlag(QFile::ExeGroup) ? "x" : "-" )
               //                 <<(perms.testFlag(QFile::ReadOther) ? "r" : "-" )
               //                <<(perms.testFlag(QFile::WriteOther) ? "w" : "-" )
               //               <<(perms.testFlag(QFile::ExeOther) ? "x" : "-" );
#endif
          }
     }


}

/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::selectionHasChanged(const QItemSelection &/*selected*/,
                                     const QItemSelection &/*deselected*/)
{


     int dir=0,file=0;
     foreach (QModelIndex index, selectedIndex()) {
          if(myModel->fileInfo(index).isDir())
               dir++;
          else
               file++;
     }

     QString msg;

     if(dir+file==1)
          msg="file://"+myModel->fileInfo(listSelectionModel->currentIndex()).filePath();
     else if(dir+file==0)
          msg="";
     else
          msg=QString(tr("%1 Folders selected %2 Files selected"))
                    .arg(QString::number(dir)).arg(QString::number(file));

     emit selectedFoldersFiles(msg);

     if(!QFileInfo(m_dirPath).isWritable())
     {
          emit selectedAvailabe(false);
          mActions->actCopy->setEnabled(listSelectionModel->selectedIndexes().count());
          mActions->actPaste->setEnabled(false);
     }else{
          emit selectedAvailabe(listSelectionModel->selectedIndexes().count());
     }

}

/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::setViewMode(int mode)
{

     mViewMode=mode;
     switch (mode)
     {
          case IconView:

               listView->setViewMode(QListView::IconMode);
               listView->setViewIconSize( mSettings->viewIconSize());
               break;
          case CompactView:

               listView->setViewMode(QListView::ListMode);
               listView->setViewIconSize( mSettings->viewIconSize());
               //               listView-> setFocus();
               //                stackedWidget->setCurrentIndex(WListView);
               break;
          case DetailView:

               //                treeView-> setFocus();
               //                stackedWidget->setCurrentIndex(WTreeView);
               break;

     }


     urlChanged(m_dirPath);


}
//__________________________________________________________________________________
bool urlIsHidden(QString url)
{


     QDir dir(url);
     do{
          QFileInfo fi(dir.absolutePath());

          if(fi.isHidden()){

               return true;
          }
     }while (dir.cdUp());


     return false;
}

/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::setUrl(const QString &url)
{


    if(m_dirPath==url)return;
    if(url.isEmpty())return;
    if(m_dirPath==":/trash" && url==":/search")return;



    QModelIndex i = myModel->index(url);
    if(url!=":/trash"&&url!=":/search"){

        if(!i.isValid()){
            if(urlIsHidden(url))
                emit selectedFoldersFiles(("this path is hidden please presse Ctrl+h"));
            else
                emit selectedFoldersFiles(tr("this path is no valid "));

            return;
        }
    }

    if(!m_dirPath.isEmpty() && m_dirPath!=":/search"){
        mHistoryBack.push(m_dirPath);

        mHistoryForward.clear();
        emit  historyForwardAvailable(false);
        emit  historyBackAvailable(!mHistoryBack.isEmpty());
    }

    urlChanged(url);


    //     if(url!=":/search")
    //          mHistory.append(url);
    //     if(mHistory.count()>12)
    //          mHistory.removeFirst();

    //     mHindex=mHistory.count()-1;




}



/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::urlChanged(const QString &url)
{


     qApp->processEvents();

     QString oldPath=m_dirPath;
     m_dirPath=url;


     if(m_dirPath.isEmpty())
          m_dirPath=QDir::rootPath();
     QModelIndex i = myModel->index(url);

     if(m_dirPath==":/trash")
     {
          stackedWidget->setCurrentIndex(WTrashView);
          trashView->updateTrashFiles();
          trashView->setFocus();


     }
     else if(m_dirPath==":/search")
     {

          stackedWidget->setCurrentIndex(WSearchView);
          if(oldPath!=":/search")
               searchView->setPath(oldPath);

     }else{


          if(!i.isValid()){
               if(urlIsHidden(url))
                    emit selectedFoldersFiles(("this path is hidden please presse Ctrl+h"));
               else
                    emit selectedFoldersFiles(tr("this path is no valid "));

               return;
          }

          /*-if files and folders count > 500
           *change the IconProvider
           *emit the tab parent
           * slot setLargeDirectory(QString)
           */
          QDir dir(m_dirPath);
          if(dir.count()>100)
              emit isLargeDirectory(m_dirPath);

          //-----------------------------------
          switch (mViewMode)
          {

               case IconView:
                    stackedWidget->setCurrentIndex(WListView);
                    listView->setRootPath(m_dirPath);
                    listView->setFocus();
                    break;
               case CompactView:
                    stackedWidget->setCurrentIndex(WListView);
                    listView->setRootPath(m_dirPath);
                    listView->setFocus();
                    break;
               case DetailView:
                    stackedWidget->setCurrentIndex(WTreeView);
                    treeView->setRootPath(m_dirPath);
                    treeView->setFocus();
                    break;


          }
          //-----------------------------------
        //  QApplication::restoreOverrideCursor();
     }


     emit urlHasChanged(m_dirPath);
     emit indexHasChanged(i);
    // listSelectionModel->clearSelection();





}
/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::updateSignals()
{

    emit historyBackAvailable(mbackEnabled);
    emit historyForwardAvailable(mForwardEnabled);
    emit urlHasChanged(m_dirPath);
    setViewMode(mSettings->viewMode());
    emit selectedAvailabe(listSelectionModel->selection().count());


}
/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::viewChangged(int index)
{

     if(index!=WSearchView)
          searchView->stopSearch();

}

/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::goBack()
{
    if(mHistoryBack.isEmpty()){
        emit  historyBackAvailable(false);
        return;
    }


    QString url=  mHistoryBack.pop();
    emit  historyBackAvailable(!mHistoryBack.isEmpty());

    if(!m_dirPath.isEmpty())
        mHistoryForward.push(m_dirPath);

    emit  historyForwardAvailable(!mHistoryForward.isEmpty());

    urlChanged(url);




    /*
     int i=   mHindex-1;

     if(i<0||i>mHistory.count()-1){
          emit  historyBackAvailable(false);
          return;
     }

     emit  historyBackAvailable(i);

     QString url=  mHistory.at(i)  ;
     mHindex=i;
     urlChanged(url);
     emit   historyForwardAvailable(true);
*/

}

/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::goForward()
{
    if(mHistoryForward.isEmpty()){
        emit  historyForwardAvailable(false);
        return;
   }

    QString url=  mHistoryForward.pop();

      emit  historyForwardAvailable(!mHistoryForward.isEmpty());

    urlChanged(url);

    /*
     int i= mHindex+1;

     if(i>mHistory.count()-1||i<0)
     {

          emit  historyForwardAvailable(false);
          return;

     }

     if(i==mHistory.count()-1){

          emit  historyForwardAvailable(false);

     }

     QString url=  mHistory.at(i)  ;
     mHindex=i;
     urlChanged(url);

     emit   historyBackAvailable(true);

*/
}

/**************************************************************************************
 *                             تغيير اسم الملف
 **************************************************************************************/
void PageWidget::renameFiles()
{
    switch (focusedWidget()) {

    case WListView:
        listView->edit(listSelectionModel->currentIndex());
        break;

    case WTreeView:
        treeView->edit(listSelectionModel->currentIndex());
        break;

    case WSearchView:
        searchView->editCurentItem();
        break;

    default:
        break;

    }
    /*
     if (focusWidget() == treeView)
     {

          treeView->edit(listSelectionModel->currentIndex());

     } else if(focusWidget() == listView) {

          listView->edit(listSelectionModel->currentIndex());

     }else if(focusWidget() == searchView->searchTreeView()){
          searchView->editCurentItem();
          //  searchView->searchTreeView()->editItem(searchView->searchTreeView()->currentItem(),0);

     }
*/

}

/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::moveFilesToTrash()
{
     trashView->moveFilesToTrash(selectedFiles());
}

/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::selectAll()
{

    switch (focusedWidget()) {
    case WListView:
        listView->selectAll();
        break;
    case WTreeView:
        treeView->selectAll();
        break;
    case WTrashView:
        trashView->selectAll();
        break;
    case WSearchView:
        searchView->searchTreeView()->selectAll();
        break;
    default:
        break;
    }

    /*
     if (focusWidget() == treeView)
     {

          treeView->selectAll();

     }else if(focusWidget() == listView) {

          listView->selectAll();

     }else if(focusWidget() == trashView) {

          trashView->selectAll();

     }else if(focusWidget() == searchView->searchTreeView()) {

          searchView->searchTreeView()->selectAll();

     }
*/
}

/**************************************************************************************
 *
 **************************************************************************************/
QString PageWidget::selectedFile()
{


     QString file;

     switch (focusedWidget()) {
     case WListView:
         file=myModel->filePath(listView->currentIndex());
         break;

     case WTreeView:
         file=myModel->filePath(treeView->currentIndex());
         break;

     case WSearchView:
         file=searchView->selectedFile();
         break;

     default:
         break;
     }

     /*
     if (focusWidget() == listView)
     {
          QModelIndex cIndex=   listView->currentIndex();
          file=myModel->filePath(cIndex);
     }else  if (focusWidget() == treeView){

          QModelIndex cIndex=   treeView->currentIndex();
          file=myModel->filePath(cIndex);
     }else if(focusWidget() == searchView->searchTreeView()){
          file=searchView->selectedFile();
     }
*/
     return file;

}

/**************************************************************************************
 *
 **************************************************************************************/
QStringList PageWidget::selectedFiles()
{

    QStringList listFiles;

    switch (focusedWidget()) {
    case WListView:
        listFiles=listView->selectedFiles();
        break;

    case WTreeView:
        listFiles=treeView->selectedFiles();
        break;

    case WSearchView:
        listFiles=searchView->selectedFiles();
        break;

    default:
        break;
    }

    /*
     if (focusWidget() == treeView||focusWidget() == listView)
     {
          QModelIndexList indexList=selectedIndex();
          foreach (QModelIndex index, indexList) {
               listFiles.append(myModel->filePath(index));
          }
     }else if(focusWidget() == searchView->searchTreeView()){
          listFiles=searchView->selectedFiles();
     }
*/
    return listFiles;

}

/**************************************************************************************
 *
 **************************************************************************************/
QModelIndexList PageWidget::selectedIndex()
{

    QModelIndexList indexList;

    switch (focusedWidget()) {
    case WListView:
        indexList=listSelectionModel->selectedIndexes();
        break;

    case WTreeView:
        indexList=listSelectionModel->selectedRows(0);
        break;

    default:
        break;
    }

    /*
     if (focusWidget() == treeView)
     {

          indexList=listSelectionModel->selectedRows(0);

     } else if(focusWidget() == listView) {

          indexList=listSelectionModel->selectedIndexes();

     }else if(focusWidget() == trashView) {

          //NULL ACTION

     }

*/
    return indexList;

}


int getIconSize(int size,bool zoomin)
{

    int retSize;
    if(zoomin){
             if(size<16)retSize= 16;
        else if(size<22)retSize= 22;
        else if(size<24)retSize= 24;
        else if(size<32)retSize =32;
        else if(size<48)retSize= 48;
        else if(size<64)retSize= 64;
        else if(size<72)retSize= 72;
        else if(size<96)retSize= 96;
        else if(size<128)retSize= 128;
        else retSize= 128;
    }else{

             if(size>128)retSize= 128;
        else if(size>96)retSize= 96;
        else if(size>72)retSize= 72;
        else if(size>64)retSize= 64;
        else if(size>48)retSize= 48;
        else if(size>32)retSize= 32;
        else if(size>24)retSize= 24;
        else if(size>22)retSize= 22;
        else if(size>16)retSize= 16;
        else retSize= 16;
    }

    if(retSize<16)retSize=16;
    if(retSize>128)retSize=128;

    return retSize;
}

void PageWidget::setZoomOut()
{

    int size;
    int newsize;
    switch (focusedWidget()) {
    case WTreeView:
        size=mSettings->treeIconSize();
        //if(size<16)size=16;
         newsize=getIconSize(size,false);
        treeView->setTreeIconSize(newsize);
        mSettings->setValue("TreeIconSize",newsize);
        break;

    case WListView:
        size=mSettings->viewIconSize();
       // if(size<16)size=16;
         newsize=getIconSize(size,false);
        listView->setViewIconSize(newsize);
        mSettings->setValue("ViewIconSize",newsize);
        break;

    default:
        break;
    }
    /*
     if (focusWidget() == treeView)
     {
        int size=mSettings->treeIconSize()-8;
         if(size<16)size=16;
        treeView->setTreeIconSize(size);
        mSettings->setValue("TreeIconSize",size);

     } else if(focusWidget() == listView) {
         int size=mSettings->viewIconSize()-8;
          if(size<16)size=16;
         listView->setViewIconSize(size);
         mSettings->setValue("ViewIconSize",size);

     }
*/
}
void PageWidget::setZoomIn()
{
    int size;
    int newsize;

    switch (focusedWidget()) {
    case WTreeView:
        size=mSettings->treeIconSize();
       // if(size>128)size=128;
         newsize=getIconSize(size,true);
        treeView->setTreeIconSize(newsize);
        mSettings->setValue("TreeIconSize",newsize);
        break;

    case WListView:
        size=mSettings->viewIconSize();
       // if(size>128)size=128;
         newsize=getIconSize(size,true);
        listView->setViewIconSize(newsize);
        mSettings->setValue("ViewIconSize",newsize);
        break;

    default:
        break;
    }

    /*
     if (focusWidget() == treeView)
     {
        int size=mSettings->treeIconSize()+8;
        if(size>128)size=128;
        treeView->setTreeIconSize(size);
        mSettings->setValue("TreeIconSize",size);

     } else if(focusWidget() == listView) {
         int size=mSettings->viewIconSize()+8;
          if(size>128)size=128;
         listView->setViewIconSize(size);
         mSettings->setValue("ViewIconSize",size);

     }
*/
}

/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::showProperties()
{

     QStringList list=selectedFiles();

     if(propertiesDlg){
          propertiesDlg=0;
          delete propertiesDlg;
     }
     if(list.count()<1)
          list.append(m_dirPath);

     propertiesDlg=new PropertiesDlg(list,this);

     propertiesDlg->show();



}

/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::showOpenwithDlg(const QString &fileName)
{

  QString mim=EMimIcon::mimeTyppe(QFileInfo(fileName));
     if(!openWithDlg)
          openWithDlg =new OpenWithDlg(this);


     if(openWithDlg->exec()==QDialog::Accepted){


          QString proc=openWithDlg->execName();

          if(!proc.isEmpty()){
               EMimIcon::launchAppFile(fileName,proc);

               if(openWithDlg->addAssociation()){
                    QString desktopName=openWithDlg->desktopFileName();

                    if(desktopName.isEmpty()){
                        desktopName= EMimIcon::createDescktopFile(proc);
                    }

                     EMimIcon::AddMimeAssociatedApplication(mim,desktopName);
               }
          }

     }
     openWithDlg=0;
     delete openWithDlg;


}

 int PageWidget::focusedWidget()
 {
     if (focusWidget() == listView)return WListView;
     if (focusWidget() == treeView) return WTreeView;
     if (focusWidget() == searchView->searchTreeView()) return WSearchView;
     if (focusWidget() == trashView) return WTrashView;
     return -1;
 }
void PageWidget::iconUpdate(QModelIndex index)
{
     if(!index.isValid())return;
    if (focusWidget() == listView){

        listView->update(index);
    }
    if (focusWidget() == treeView){

        treeView->update(index);

    }

}
