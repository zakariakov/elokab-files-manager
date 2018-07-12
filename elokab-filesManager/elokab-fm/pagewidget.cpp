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
/*****************************************************************************************************
 *
 *****************************************************************************************************/
PageWidget::PageWidget(QFileSystemModel *model,
                       Settings *setting,
                       Actions *action,
                       const QString &url,
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

    mItemDelegate=new ItemDelegate;
   // connect(mItemDelegate,SIGNAL(imageHasThumb(QString)),this,SLOT(iconThumbUpdate(QString)));
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


    QWidget *pageTrash = new QWidget();
    QVBoxLayout *vLayoutTrash = new QVBoxLayout(pageTrash);
    vLayoutTrash->setSpacing(6);
    vLayoutTrash->setContentsMargins(0,0,0,0);
    trashView = new TrashView(mActions,pageTrash);

    vLayoutTrash->addWidget(trashView);
    stackedWidget->addWidget(pageTrash);

    searchView = new SearchView(this);
    stackedWidget->addWidget(searchView);
    connect(searchView,SIGNAL(searchingCanceled(QString)),this,SLOT(setUrlChange(QString)));
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
   // connect(mActions,SIGNAL(reloadIcons()),this,SLOT(clearIcons())) ;

    connect(mSettings,SIGNAL(rootDecorationChanged(bool)),treeView,SLOT(setExpandable(bool)));
    connect(mSettings,SIGNAL(showThumbnailsChanged(bool)),mItemDelegate,SLOT(setTumbnail(bool))) ;

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
    mItemDelegate->setTumbnail(mSettings->showThumbnails());
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
bool isArchive(const QString &mim)
{
    QStringList listArchive;
    listArchive<<"application/x-7z-compressed"<<"application/x-7z-compressed-tar"
              <<"application/x-ace"<<"application/x-alz"<<"application/x-ar"
             <<"application/x-arj"<<"application/x-bzip"<<"application/x-bzip-compressed-tar"
            <<"application/x-bzip1"<<"application/x-bzip1-compressed-tar"
           <<"application/x-cabinet"<<"application/x-cd-image"<<"application/x-compress"
          <<"application/x-compressed-tar"<<"application/x-cpio"<<"application/x-deb"
         <<"application/x-ear"<<"application/x-ms-dos-executable"<<"application/x-gtar"
        <<"application/x-gzip"<<"application/x-gzpostscript"<<"application/x-java-archive"
       <<"application/x-lha"<<"application/x-lhz"<<"application/x-lrzip"
      <<"application/x-lrzip-compressed-tar"<<"application/x-lzma"
      <<"application/x-lz4"<<"application/x-lzip"<<"application/x-lzip-compressed-tar"
     <<"application/x-lzma-compressed-tar"<<"application/x-lzop"<<"application/x-lz4-compressed-tar"
    <<"application/x-lzop-compressed-tar"<<"application/x-ms-wim"
    <<"application/x-rar"<<"application/x-rar-compressed"
    <<"application/x-rpm"<<"application/x-source-rpm"<<"application/x-rzip"
    <<"application/x-rzip-compressed-tar"<<"application/x-tar"
    <<"application/x-tarz"<<"application/x-stuffit"<<"application/x-war"
    <<"application/x-xz"<<"application/x-xz-compressed-tar"
    <<"application/x-zip"<<"application/x-zip-compressed"
    <<"application/x-zoo"<<"application/zip"<<"application/x-archive"
    <<"application/vnd.ms-cab-compressed"<<"application/vnd.debian.binary-package"
    <<"application/gzip"<<"application/vnd.rar";

    if(listArchive.contains(mim))
        return true;
    return false;
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
       // count=listSelectionModel->selectedIndexes().count();
         count=listSelectionModel->selectedRows(0).count();
        qDebug()<<"count"<<count;
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

    QModelIndex idx=myModel->index(info.filePath());
    QString mim;

    //qDebug()<<"absoluteFilePath"<<info.absoluteFilePath();
    //qDebug()<<"filePath"<<info.filePath();
    //qDebug()<<"path"<<info.path();

    if(idx.isValid())
        mim=idx.data(_MMIM).toString();
    else
        mim=EMimIcon::mimeTyppe(info);

    if(!isSearch)
        menu.addMenu(mActions->menuNew());//nonsearch

    if (count==0 ){
        if (isSearch) return;

        menu.addMenu(mActions->menuService(QStringList()<<m_dirPath,"inode/directory"));
        menu.addAction(mActions->openInNewTabAction(m_dirPath));
        menu.addAction(mActions->openTerminalAction(m_dirPath));
        menu.addSeparator();
        menu.addActions(mActions->menuViewfile()->actions());
        menu.addAction(mActions->propertiesAction());
        menu.exec(QCursor::pos());
        return;

    }



    if(isSearch){
        emit selectedAvailabe(true);
        mActions->pasteAction()->setEnabled(false);
        if(info.isDir())
        {

            menu.addAction(mActions->openTerminalAction(selectedPath));
            menu.addAction(mActions->openInNewTabAction(info.filePath()));
            menu.addSeparator();
            //mActions->setcurPath(selectedPath);


        }else{

            menu.addAction(mActions->openTerminalAction(info.path()));
            if(mim!= "application/x-executable" )
                menu.addMenu(mActions->menuOpenWith(selectedPath, mim))  ;

        }

    }// search
    else  //non search
    {

        if(count==1){
            if(info.isDir())
            {
                //   menu.addAction(mActions->actOpenInTerminal);
                menu.addAction(mActions->openInNewTabAction(info.filePath()));
                menu.addAction(mActions->openTerminalAction(selectedPath));
                // mActions->setcurPath(selectedPath);

            }else{
                // mActions->setcurPath(m_dirPath);
                if(mim!= "application/x-executable" )
                    menu.addMenu(mActions->menuOpenWith(selectedPath, mim))  ;

            }
            menu.addMenu(mActions->menuService(QStringList()<<m_dirPath,mim));
            menu.addSeparator();



        }else{
            qDebug()<<"selectedFiles="<<selectedFiles();

            menu.addMenu(mActions->menuService(selectedFiles(),mim));
            menu.addSeparator();
        }


    }

    menu.addSeparator();


    //----------------------------------------------
    menu.addActions(mActions->menuViewfile()->actions());

    if(QFileInfo(m_dirPath).isWritable()){

        if(count==1  && isArchive(mim))
            menu.addAction(mActions->extractHereAction(info.filePath()));

        menu.addAction(mActions->AddArchiveAction(selectedFiles()));
        menu.addSeparator();

    }

    menu.addAction(mActions->propertiesAction());

    menu.exec(QCursor::pos());

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
           QString mim=index.data(_MMIM).toString();
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

    if(!QFileInfo(m_dirPath).isWritable())
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
            if(urlIsHidden(url)){
               emit selectedFoldersFiles(tr("this path is hidden please presse Ctrl+h"));
            }else{
                emit selectedFoldersFiles(tr("this path is no valid "));
            }
             return;
        }
    }

    if(!m_dirPath.isEmpty() && m_dirPath!=":/search"){
        mHistoryBack.push(m_dirPath);

        mHistoryForward.clear();
        emit  historyForwardAvailable(false);
        emit  historyBackAvailable(!mHistoryBack.isEmpty());
        //emit  urlBeginChanged(url);
    }

    setUrlChange(url);


    //     if(url!=":/search")
    //          mHistory.append(url);
    //     if(mHistory.count()>12)
    //          mHistory.removeFirst();

    //     mHindex=mHistory.count()-1;




}



/**************************************************************************************
 *
 **************************************************************************************/
void PageWidget::setUrlChange(const QString &url)
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


    emit urlChanged(m_dirPath);

    emit indexHasChanged(i);

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
    QString mim;
    QModelIndex idx=myModel->index(fileName);
    if(idx.isValid())
        mim=idx.data(_MMIM).toString();
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

    mItemDelegate->clearItemCache(myModel->filePath(index));

    if (focusWidget() == listView){

        listView->update(index);
    }
    if (focusWidget() == treeView){

        treeView->update(index);

    }

}

//TODO RMOVE THIS
//void PageWidget::iconThumbUpdate(const QString &fileName)
//{
//    if(!QFile::exists(fileName))return;

//    QModelIndex idx=myModel->index(fileName);
//    if(idx.isValid())
//        iconUpdate(idx);
//}


