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

#include "pagewidget.h"

#include "defines.h"
#include "filesutils/propertiesdlg.h"
#include "filesutils/openwithdlg.h"
#include "dialogrenamefiles.h"
//#include "itemdelegate.h"
#include "messages.h"
//#ifdef DEBUG_APP
#include <QDebug>
//#endif
#include <QTimer>
#include <QApplication>
#include <QSortFilterProxyModel>
#include <QImageReader>
#include <QDateTime>
#include <QtConcurrent>
#include <QMessageBox>
#include <QInputDialog>
/*****************************************************************************************************
 *
 *****************************************************************************************************/
PageWidget::PageWidget(MyFileSystemModel *model,
                       Settings *setting,
                       Actions *action,
                       const QString &url,
                       QWidget *parent) :
    QWidget(parent),
    mSettings(setting),
    myModel( model),
    mActions(action),
    propertiesDlg(nullptr),
    openWithDlg(nullptr)
{

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

    mItemDelegate=new ItemDelegate(!mSettings->isClassicIcons());

    mThumbnails=new Thumbnails ;

    listView = new MyListView(myModel,mActions,pageList);
    listView->setItemDelegate(mItemDelegate);

    vLayoutList->addWidget(listView);
    stackedWidget->addWidget(pageList);

    QWidget *pageTree = new QWidget();
    QVBoxLayout *vLayoutTree = new QVBoxLayout(pageTree);
    vLayoutTree->setSpacing(6);
    vLayoutTree->setContentsMargins(0,0,0,0);

    treeView = new MyTreeView(myModel,pageTree);
    treeView->setItemDelegate(mItemDelegate);

    vLayoutTree->addWidget(treeView);
    stackedWidget->addWidget(pageTree);


    mTrash=new Trash;
    searchView = new SearchView(this);
    stackedWidget->addWidget(searchView);


    vLayout->addWidget(stackedWidget);
    listSelectionModel=listView->selectionModel();
    treeView-> setSelectionModel(listView->selectionModel());

    treeView->setContextMenuPolicy  (Qt::CustomContextMenu);
    listView->setContextMenuPolicy  (Qt::CustomContextMenu);
    searchView->setContextMenuPolicy(Qt::CustomContextMenu);



    connect(this,SIGNAL(urlChanged(QString)),mThumbnails,SLOT(directoryChanged(QString)));
    connect(listSelectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(selectionHasChanged(QItemSelection,QItemSelection)));

    connect(treeView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));
    connect(treeView,SIGNAL(activated(QModelIndex)),this,SLOT(slotItemActivated(QModelIndex)));

    connect(listView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));
    connect(listView,SIGNAL(activated(QModelIndex)),this,SLOT(slotItemActivated(QModelIndex)));
    connect(listView,SIGNAL(fileEntered(QString)),this,SIGNAL(selectedFoldersFiles(QString)));

    connect(mItemDelegate,SIGNAL(requireThumb(QFileInfo,QString)),mThumbnails,SLOT(addFileName(QFileInfo,QString)));
    connect(mThumbnails,SIGNAL(updateThumbnail(QString)),this,SLOT(iconThumbUpdate(QString)));

    connect(stackedWidget,SIGNAL(currentChanged(int)),this,SLOT(viewChangged(int)));
    connect(searchView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));

    //   connect(mSettings,SIGNAL(iconSizeChanged(int)),this,SLOT(setZoomIn(int)));
    connect(mActions,SIGNAL(zoomInChanged()),              this,SLOT(setZoomIn()));
    connect(mActions,SIGNAL(ZoomOutChanged()),             this,SLOT(setZoomOut()));
    connect(mActions,SIGNAL(newFileCreated(QString)),      this,SLOT(selectIndex(QString))) ;
    connect(mActions,SIGNAL(trashDeleteFile()),            this,SLOT(trashDeleteFiles())) ;
    connect(mActions,SIGNAL(trashRestoreFiles()),          this,SLOT(trashRestoreFiles())) ;
    connect(mActions,SIGNAL(trashClean()),                 mTrash,SLOT(cleanTrash())) ;

    connect(mSettings,SIGNAL(rootDecorationChanged(bool)), treeView,SLOT(setExpandable(bool)));
    connect(mSettings,SIGNAL(showThumbnailsChanged(bool)), mItemDelegate,SLOT(setTumbnail(bool))) ;
    connect(mSettings,SIGNAL(classicIconsChanged(bool)),   mItemDelegate,SLOT(setClassicIcons(bool))) ;
    connect(mSettings,SIGNAL(pdfThumbnailsChanged(bool)),  mItemDelegate,SLOT(setPdfTumbnail(bool))) ;
    connect(mSettings,SIGNAL(VideoThumbnailsChanged(bool)),mItemDelegate,SLOT(setVideoTumbnail(bool))) ;
    connect(mSettings,SIGNAL(doubleClickEditChanged(bool)),this,SLOT(setDoubleClickEdit(bool ))) ;

    connect(searchView,SIGNAL(setUrl(QString)),            this,SLOT(setUrl(QString)));
    connect(searchView,SIGNAL(searchingCanceled(QString)), this,SLOT(setUrlChange(QString)));
    connect(searchView,SIGNAL(showOpenwithDlg(QString)),   this,SLOT(showOpenwithDlg(QString)));
    connect(searchView,SIGNAL(fileSelected(QString)),      this,SIGNAL(selectedFoldersFiles(QString)));

    connect(this,SIGNAL(historyBackAvailable(bool)),       mActions,SIGNAL(backAvailable(bool))) ;
    connect(this,SIGNAL(historyForwardAvailable(bool)),    mActions,SIGNAL(forwardAvailable(bool))) ;
    connect(this,SIGNAL(selectedAvailabe(bool)),           mActions,SIGNAL(selectionAvialable(bool)));

    connect(this,SIGNAL(historyBackAvailable(bool)),       this,SLOT(setBackEnabled(bool)));
    connect(this,SIGNAL(historyForwardAvailable(bool)),    this,SLOT(setForwardEnabled(bool)));

    listView->setViewIconSize      (mSettings->viewIconSize());
    treeView->setTreeIconSize      (mSettings->treeIconSize());
    treeView->setRootIsDecorated   (mSettings->isRootDecorated());
    mItemDelegate->setTumbnail     (mSettings->showThumbnails());
    mItemDelegate->setPdfTumbnail  (mSettings->pdfThumbnails());
    mItemDelegate->setVideoTumbnail(mSettings->videoThumbnails());
    setDoubleClickEdit(mSettings->doubleClickEdit());
    setUrl(url);



}


//___________________________________________________________________
PageWidget::~PageWidget()
{

    searchView->stopSearch();
    searchView->close();
    // delete searchView;
    delete listView;
    delete treeView;
    delete mThumbnails;
   // delete trashView;
}
void PageWidget::closeAll()
{

    searchView->stopSearch();
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
    bool isTrash=false;
    if(m_dirPath==D_TRASH){isTrash=true;}


    switch (focusedWidget()) {
    case WListView:
        selectedPath=myModel->filePath(listView->currentIndex());
        info=myModel->fileInfo(listView->currentIndex());
       // count=listSelectionModel->selectedIndexes().count();
        count=selectedFiles().count();
         qDebug()<<"count"<<count;
        break;

    case WTreeView:
        selectedPath=myModel->filePath(treeView->currentIndex());
        info=myModel->fileInfo(treeView->currentIndex());
        // count=listSelectionModel->selectedIndexes().count();
        count=listSelectionModel->selectedRows(0).count();
        // count=selectedFiles().count();
        qDebug()<<"count"<<count;
        break;

    case WSearchView:
        selectedPath=searchView->selectedFile();
        info=QFileInfo(selectedPath);
        count=searchView->selectedFilesCount();
        isSearch=true;
          qDebug()<<"count"<<count;
        break;

    default:
        return;
    }

    QModelIndex idx=myModel->index(info.filePath());
    QString mim;

    if(idx.isValid())
        mim=idx.data(D_MMIM).toString();
    else
        mim=EMimIcon::mimeTyppe(info);

//![0] search  *****************************************************
    if(isSearch){
        if (count==0 ){return;}

        emit selectedAvailabe(true);
        mActions->pasteAction()->setEnabled(false);
        menu.addAction(mActions->openTerminalAction());
        menu.addAction(mActions->openInNewTabAction());
        if(info.isDir())
        {
            mActions->setSelectedDir(selectedPath);
            menu.addSeparator();
            //mActions->setcurPath(selectedPath);
        }else{
            mActions->setSelectedDir(info.path());
            if(mim!= "application/x-executable" )
                menu.addMenu(mActions->menuOpenWith(selectedPath, mim))  ;

        }

        //----------------------------------------------
        menu.addActions(mActions->menuEditePopup()->actions());
        //----------------------------------------------
        menu.addAction(mActions->propertiesAction());
        menu.exec(QCursor::pos());
        mActions->restorePath();

        return;
    }// search

    if(isTrash){

        menu.addActions(mActions->menuTrach()->actions());
         menu.exec(QCursor::pos());
          return;
    }
//![1]  no selected file ****************************************
    menu.addMenu(mActions->menuNew());//nonsearch
    if (count==0 ){

        menu.addMenu(mActions->menuNew());
        mActions->setSelectedDir(m_dirPath);
        menu.addMenu(mActions->menuService(QStringList()<<m_dirPath,"inode/directory"));
        menu.addAction(mActions->openInNewTabAction());
        menu.addAction(mActions->openTerminalAction());
        menu.addSeparator();
        menu.addActions(mActions->menuEditePopup()->actions());
        menu.addAction(mActions->propertiesAction());
        menu.exec(QCursor::pos());
         mActions->restorePath();
        return;

    }

//![2] else  selection avaliable************************************************
    if(count==1){
        if(info.isDir()) //dir
        {
            mActions->setSelectedDir(info.filePath());
            menu.addAction(mActions->openInNewTabAction());

        }else{ //files
            if(mim!= "application/x-executable" )
                menu.addMenu(mActions->menuOpenWith(selectedPath, mim))  ;
        }

        menu.addAction(mActions->openTerminalAction());
        qDebug()<<"count"<<count<<"mim"<<mim<<selectedFiles().at(0);

        menu.addMenu(mActions->menuService(selectedFiles(),mim));
    }/*else{ // count>1
        menu.addMenu(mActions->menuService(selectedFiles(),mim));
    }*/
    menu.addSeparator();
    // EDIT ----------------------------------------
    menu.addActions(mActions->menuEditePopup()->actions());
    // archive-------------------------------------
    if(QFileInfo(m_dirPath).isWritable()){
        if(count==1  && EMimIcon::isArchive(mim))
            menu.addAction(mActions->extractHereAction(info.filePath()));
        menu.addAction(mActions->AddArchiveAction(selectedFiles()));
        menu.addSeparator();
    }
    // PROPERTIES----------------------------------
    menu.addAction(mActions->propertiesAction());

    menu.exec(QCursor::pos());

    mActions->restorePath();

}

/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::slotItemActivated(QModelIndex index)
{


    if(selectedIndex().count()>0)
    {

        if(myModel->isDir( listSelectionModel->currentIndex()))
        {
            if(m_dirPath==D_TRASH)
                return;

            setUrl(myModel->filePath(index));
        }else{
           QString mim=index.data(D_MMIM).toString();
           qDebug()<<mim<<myModel->filePath(index);
           // QString mim=EMimIcon::mimeTyppe(myModel->fileInfo(index));
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

    if(!QFileInfo(m_dirPath).isWritable() && m_dirPath!=D_TRASH)
    {
        emit selectedAvailabe(false);
        mActions->copyAction()->setEnabled(listSelectionModel->selectedIndexes().count());
        mActions->pasteAction()->setEnabled(false);
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
        mItemDelegate->setTreeView(false);
        listView->setListViewMode(QListView::IconMode);
        listView->setViewIconSize( mSettings->viewIconSize());

        break;
    case CompactView:
        mItemDelegate->setTreeView(false);
        listView->setListViewMode(QListView::ListMode);
        listView->setViewIconSize( mSettings->viewIconSize());

        //               listView-> setFocus();
        //                stackedWidget->setCurrentIndex(WListView);
        break;
    case DetailView:
        mItemDelegate->setTreeView(true);
        //                treeView-> setFocus();
        //                stackedWidget->setCurrentIndex(WTreeView);
        break;

    }


    setUrlChange(m_dirPath);


}

void PageWidget::setDoubleClickEdit(bool arg)
{
    mDoubleClickEdit=arg;
listView->setdDoubleClickTriggers(arg);
treeView->setdDoubleClickTriggers(arg);
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
    if(m_dirPath==D_TRASH && url==D_SEARCH)return;



    QModelIndex i = myModel->index(url);
    if(url!=D_TRASH && url!=D_SEARCH){

        if(!i.isValid()){
            if(urlIsHidden(url)){
               // myModel->setFilter( QDir::AllEntries | QDir::System|QDir::NoDotAndDotDot|QDir::Hidden);
                qDebug()<<myModel->filter();

             //  emit selectedFoldersFiles(tr("this path is hidden please presse Ctrl+h"));
            }else{
                emit selectedFoldersFiles(tr("this path is no valid "));
                 return;
            }

        }

    }

    if(!m_dirPath.isEmpty() && m_dirPath!=D_SEARCH){
        mHistoryBack.push(m_dirPath);

        mHistoryForward.clear();
        emit  historyForwardAvailable(false);
        emit  historyBackAvailable(!mHistoryBack.isEmpty());
        //emit  urlBeginChanged(url);
    }

    setUrlChange(url);

}



/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::setUrlChange(const QString &url)
{

    QString oldPath=m_dirPath;

    QString curPath=url;
    m_dirPath=url;

  bool restorRoot=false;

    if(m_dirPath.isEmpty())
        m_dirPath=QDir::rootPath();

    if(curPath==D_SEARCH)
    {

        stackedWidget->setCurrentIndex(WSearchView);
        if(oldPath!=D_SEARCH)
            searchView->setPath(oldPath);

    }else{

        if(url==D_TRASH)
            curPath=Edir::trashFiles();

        QModelIndex i = myModel->index(curPath);
        if(!i.isValid()){
            if(urlIsHidden(curPath)){
                qDebug()<<"isHidden"<<curPath;
               // myModel->setFilter( QDir::AllEntries | QDir::System|QDir::NoDotAndDotDot|QDir::Hidden);

                myModel->setRootPath(curPath);
                restorRoot=true;
            }else{
                m_dirPath=oldPath;
                myModel->setRootPath(oldPath);
                emit selectedFoldersFiles(tr("this path is no valid "));
                return;
            }
        }

       // myModel->setRootPath(curPath);
        //-----------------------------------
        switch (mViewMode)
        {

        case IconView:
            stackedWidget->setCurrentIndex(WListView);
            listView->setRootPath(curPath);
            listView->setFocus();
            break;
        case CompactView:
            stackedWidget->setCurrentIndex(WListView);
            listView->setRootPath(curPath);
            listView->setFocus();
            break;
        case DetailView:
            stackedWidget->setCurrentIndex(WTreeView);
            treeView->setColumnHidden(D_COL_TYPE,    m_dirPath==D_TRASH);
            treeView->setColumnHidden(D_COL_DATE,    m_dirPath==D_TRASH);
            treeView->setColumnHidden(D_COL_TRASHED, m_dirPath!=D_TRASH);
            treeView->setColumnHidden(D_COL_ORIGPATH,m_dirPath!=D_TRASH);

            treeView->setRootPath(curPath);
            treeView->setFocus();
            break;


        }
        //-----------------------------------
        //  QApplication::restoreOverrideCursor();
    }

    emit urlChanged(m_dirPath);

    emit indexHasChanged(myModel->index(m_dirPath));

    if(restorRoot){

        myModel->setFilter(myModel->filter());
        //myModel->setFilter( QDir::AllEntries | QDir::System|QDir::NoDotAndDotDot);
       myModel->setRootPath(QDir::rootPath());

    }


}
/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::updateSignals()
{

    emit historyBackAvailable(mbackEnabled);
    emit historyForwardAvailable(mForwardEnabled);
    emit urlChanged(m_dirPath);
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

    setUrlChange(url);

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

    setUrlChange(url);

}

/**************************************************************************************
 *                             تغيير اسم الملف
 **************************************************************************************/
void PageWidget::renameFiles()
{
  if(  selectedFiles().count()>1){
   //  QStringList list=selectedFiles();

      DialogRenameFiles *dlg=new DialogRenameFiles(selectedFiles());
     if( dlg->exec())
         delete  dlg;

      return;
  }
    if(mDoubleClickEdit){

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
        return;
    }
qDebug()<<"is not double click";
    //------------------------------------------------DIALOG
    QString filePath;
    switch (focusedWidget()) {

    case WListView:
        filePath=listSelectionModel->currentIndex().data(D_MFPATH).toString();
        break;

    case WTreeView:
        filePath=listSelectionModel->currentIndex().data(D_MFPATH).toString();
        break;

    case WSearchView:
        searchView->selectedFile();
        break;

    default:
        break;

    }
    qDebug()<<"filepath"<<filePath;
    if(filePath.isEmpty())
        return;
    QFile file(filePath);
    QFileInfo info(filePath);
    if(!info.isWritable())
        return;
    QString baseName=info.fileName();

        bool ok;
           QString text = QInputDialog::getText(nullptr, tr("Rename file"),
                                                tr("file Name:"), QLineEdit::Normal,
                                                baseName, &ok);
           if (ok && !text.isEmpty())
               file.rename(info.path()+"/"+text);



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
       // trashView->selectAll();
        break;
    case WSearchView:
        searchView->searchTreeView()->selectAll();
        break;
    default:
        break;
    }

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

 }

/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::showProperties()
{

    QStringList list=selectedFiles();

    if(propertiesDlg){
        propertiesDlg=nullptr;
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
    QString mim;
    QModelIndex idx=myModel->index(fileName);
    if(idx.isValid())
        mim=idx.data(D_MMIM).toString();
    else
        mim=EMimIcon::mimeTyppe(QFileInfo(fileName));

    if(!openWithDlg)
        openWithDlg =new OpenWithDlg();


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
    openWithDlg=nullptr;
    delete openWithDlg;


}

int PageWidget::focusedWidget()
{
    if (focusWidget() == listView)return WListView;
    if (focusWidget() == treeView) return WTreeView;
    if (focusWidget() == searchView->searchTreeView()) return WSearchView;
  //  if (focusWidget() == trashView) return WTrashView;
    return -1;
}


void PageWidget::iconUpdate(QModelIndex index)
{
    if(!index.isValid())return;

    mItemDelegate->clearItemCache(myModel->filePath(index));

    if (focusWidget() == listView){

        listView->update(index);
    }
    if (focusWidget() == treeView){

        treeView->update(index);

    }

}

void PageWidget::iconThumbUpdate(const QString &file)
{
    QModelIndex idx=myModel->index(file);
   if(idx.isValid())
     iconUpdate(idx);

}

void PageWidget::selectIndex(const QString &file)
{
    QModelIndex idx=myModel->index(file);
   if(idx.isValid())
  listSelectionModel->setCurrentIndex(idx,QItemSelectionModel::Select);
}


void PageWidget::trashDeleteFiles()
{
    mTrash->deleteFiles(selectedFiles());

}
void PageWidget::trashRestoreFiles()
{
    mTrash->restorFiles(selectedFiles());

}
