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
#include "defines.h"
#include "tab.h"
//#include "filesactions.h"
//#include <EMimIcon>
#include "messages.h"
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QProcess>
#include <QToolButton>

#include <QtConcurrentRun>
#include <QImageReader>
#include <QDateTime>
//#ifdef DEBUG_APP
#include <QDebug>
#include <QMessageAuthenticationCode>
//#endif


/*****************************************************************************************************
 *
 *                                   TAB
 *
 *****************************************************************************************************/

Tab::Tab(Settings *setting, Actions *actions, QWidget *parent) :
    QTabWidget(parent),
    mSettings (setting),
    mActions(actions),
    mCute(0),
    pageWidget(0)
{

    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__);
    QToolButton *btn=new QToolButton(this);
    btn->setAutoRaise(true);
    btn->setDefaultAction(mActions->anewTabAction());
    setCornerWidget(btn,Qt::TopLeftCorner);
    setMovable(true);
    setTabBarAutoHide(true);
    mFileSystemWatcher=new QFileSystemWatcher;
    myModel = new MyFileSystemModel();


    mimData=new QMimeData;

    // connect(this,SIGNAL(urlChanged(QString)),this,SLOT(directoryChanged(QString)));
    //
    connect(this,SIGNAL(urlChanged(QString)),    this,SLOT(setCurTabText(QString)));
    connect(this,SIGNAL(urlChanged(QString)),    mActions,SLOT(setUrl(QString))) ;
    connect(this,SIGNAL(currentChanged(int)),    this,SLOT(currentTabChanged(int)));
    connect(this,SIGNAL(tabCloseRequested(int)), this,SLOT(oncloseRequested(int)));
    //
    connect(mActions,SIGNAL(sendNewTab(QString)),this,SLOT(addNewTab(QString)));
    connect(mActions,SIGNAL(sendCloseTab()),     this,SLOT(closeCurentTab()));
    connect(mActions,SIGNAL(sendGoUpDir()),      this,SLOT(goUp()));
    connect(mActions,SIGNAL(sendGoBackDir()),    this,SLOT(goBack()));
    connect(mActions,SIGNAL(sendGoForwardDir()), this,SLOT(goForward()));
    connect(mActions,SIGNAL(sendGoHomeDir()),    this,SLOT(goHome()));
    connect(mActions,SIGNAL(sendGoTrachDir()),   this,SLOT(goTrash()));
    connect(mActions,SIGNAL(sendGoSearch()),     this,SLOT(goSearch()));
    connect(mActions,SIGNAL(startEdit()),        this,SLOT(renameFiles())) ;
    connect(mActions,SIGNAL(startDelete()),      this,SLOT(deleteFiles())) ;
    connect(mActions,SIGNAL(startCopyFile()),    this,SLOT(copyFiles())) ;
    connect(mActions,SIGNAL(startCutFile()),     this,SLOT(cutFiles())) ;
    connect(mActions,SIGNAL(pasteClipboard()),   this,SLOT(pastFiles())) ;
    connect(mActions,SIGNAL(startMoveToTrash()), this,SLOT(moveFilesToTrash())) ;
    connect(mActions,SIGNAL(selectAll()),        this,SLOT(selectAll())) ;
    connect(mActions,SIGNAL(showProperties()),   this,SLOT(slotShowProperties())) ;
    connect(mActions,SIGNAL(showOpenwithDlg()),  this,SLOT(slotShowOpenwithDlg())) ;
    connect(mActions,SIGNAL(reloadIcons()),      this,SLOT(updateIcons())) ;
    connect(mActions,SIGNAL(sendNewPath(QString)),this,SLOT(setUrl(QString))) ;
    //
    connect(mSettings,SIGNAL(viewModeChanged(int)),       this,SLOT(setViewMode(int))) ;
    connect(mSettings,SIGNAL(showHiddenChanged(bool)),    this,SLOT(setHiddenFile(bool))) ;
    connect(mSettings,SIGNAL(showThumbnailsChanged(bool)),this,SLOT(setShowThumbnails(bool))) ;
    connect(mSettings,SIGNAL(sortingChanged()),           this,SLOT(setSorting()));
    //
    connect(myModel,SIGNAL(dragDropFiles(bool,QString,QStringList)),this,SLOT(dragDropFiles(bool,QString,QStringList)));
   connect(myModel,SIGNAL(directoryLoaded(QString)),this,SLOT(createThumbnail(QString)));
    // connect(myModel,SIGNAL(rootPathChanged(QString)),this,SLOT(createThumbnail(QString)));

    setHiddenFile(mSettings->showHidden());
    setShowThumbnails(mSettings->showThumbnails());
    myModel->sort(mSettings->sortBy(),mSettings->sortOrder());

    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__,"End");
}
/*****************************************************************************
 *
 ****************************************************************************/
Tab::~Tab()
{
    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__);


    for (int i = 0; i < count(); ++i) {

        PageWidget *pWidget= qobject_cast<PageWidget *>(widget(i));

        if(pWidget) pWidget->close();

    }// for
    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__,"End");
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::closeAll()
{
    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__);
    for (int i = 0; i < count(); ++i) {
        PageWidget *pWidget= qobject_cast<PageWidget *>(widget(i));
        if(pWidget) pWidget->closeAll();
    }
   Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__,"End");
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::addNewTab( const QString &url)
{
   Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__);
    QString mUrl=url;
    QFileInfo fi(mUrl);
    if(!fi.isDir()){
        mUrl=fi.path();

        EMimIcon::launchApplication(url);

    }

    pageWidget=new PageWidget(myModel,mSettings,mActions,mUrl/*,mIconProvider*/, this);

    // connect(pageWidget,SIGNAL(  isLargeDirectory(QString)),this,SLOT(setLargeDirectory(QString)));
    //  connect(pageWidget,SIGNAL(urlBeginChanged(QString)),this,SLOT(directoryChanged(QString)));

    QDir dir(mUrl);
    addTab(pageWidget,EMimIcon::iconFolder(mUrl),dir.dirName());
    // currentTabChanged(currentIndex());

    myModel->sort(mSettings->sortBy(),mSettings->sortOrder());
    setCurrentWidget(pageWidget);
    setCurrentIndex(currentIndex());

    connect(pageWidget,SIGNAL(urlChanged(QString)),this,SIGNAL(urlChanged(QString)));
    //connect(pageWidget,SIGNAL(urlChanged(QString)),    mActions,SLOT(setUrl(QString))) ;

    connect(pageWidget,SIGNAL(selectedFoldersFiles(QString)),this,SIGNAL(  selectedFoldersFiles(QString)));
    emit tabAdded(mUrl);

    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__,"End");
}

//void Tab::iconUpdate(QModelIndex index)
//{
//    qDebug()<<"Tab"<<0,__LINE__<<"iconUpdate";

//    if(index.isValid()){
//        if(pageWidget)
//            pageWidget->iconUpdate(index);
//    }

//    qDebug()<<"Tab"<<0,__LINE__<<"iconUpdate End";
//}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::setShowThumbnails(bool arg)
{
    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__);


    myModel->setPreview(arg);

    QApplication::setOverrideCursor(Qt::WaitCursor) ;



    mSowTumbnails=arg;

    updateIcons();

    QApplication::restoreOverrideCursor();

 Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__,"End");
}

/*****************************************************************************
 *
 *
 ****************************************************************************/


void Tab::updateIcons()
{
    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__);

    if(!pageWidget)return;

    QString currentPath=pageWidget->curentDir();

    QApplication::setOverrideCursor(Qt::WaitCursor) ;

    myModel->clearCache(currentPath);

    pageWidget->clearCurentPath();

    QDir dir(currentPath);

    QModelIndex idx=myModel->mkdir(myModel->index(currentPath),"...");
    myModel->rmdir(idx);
    //myModel->setRootPath(currentPath);
    //    QDirIterator it(currentPath ,QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Hidden, QDirIterator::NoIteratorFlags);

    //    while (it.hasNext()) {
    //        QString s=it.next();
    //       //  qDebug()<<"Tab"<<0,__LINE__<<s;
    //        QModelIndex idx=myModel->index(s);
    //        pageWidget->iconUpdate(idx);
    //    }
    // pageWidget->setUrl( pageWidget->dirPath());

    QApplication::restoreOverrideCursor();

    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__,"End");
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::closeCurentTab()
{
    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__);

    if(count()==1)return;

    int index=currentIndex();

    oncloseRequested( index);

    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__,"End");
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::oncloseRequested(int index)
{
    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__);

    if(count()==1)return;

    PageWidget *pWidget= qobject_cast<PageWidget *>(widget(index));

    if(pWidget)
    {
        QString path=pWidget->curentDir();
        mActions->addClosedTab(path);

        delete pWidget;
    }

    setTabsClosable(count()-1);

   Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__,"End");
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::setCurTabText(const QString &title)
{
    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__);

    QDir dir(title);
    QString name=dir.dirName();
    QIcon iconF;
    if(title==":/trash" ){
        iconF=  EIcon::fromTheme("user-trash","emptytrash");
        name=tr("Trash");
    }else
        iconF=  EMimIcon::iconFolder(dir.path());


    if(dir.path()==QDir::rootPath()){
        name=tr("root");
    }

    if(dir.path()==QDir::homePath()){
        name=tr("Home");
    }

    setTabText(currentIndex(),name);
    setTabIcon(currentIndex(),iconF);
   Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__,"End");
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::currentTabChanged(int index)
{
    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__);

    setTabsClosable(count()-1);

    pageWidget= qobject_cast<PageWidget *>(widget(index));
    if(pageWidget)
        pageWidget->updateSignals();

    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__,"End");
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::setViewMode(int mode)
{
    if(pageWidget)
        pageWidget->setViewMode(mode);
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::setUrl(const QString &url)
{
    if(pageWidget){
        // QtConcurrent::run(this, &Tab::creatThumb,url);
        QString path=pageWidget->curentDir();

        if(url==path) return;
        pageWidget->setUrl(url);
    }
}


/*****************************************************************************
 *  انشاء مصغرات اذا لم تكن موجودة
 *  عند فتح اي مجلد والبحث عن الصور اذا كانت كوجودة
 ****************************************************************************/

void Tab::createThumbnail(const QString &dir)
{
    Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__);
qDebug()<<"createThumbnail"<<dir;

    if(!pageWidget )return;
    //  QtConcurrent::run(this, &Tab::creatThumb,dir);
    //  QSettings setting("elokab","thumbnails");
    QImageReader reader;
    QList<QByteArray> list=reader.supportedMimeTypes();
//qDebug()<<reader.supportedImageFormats();
    QDirIterator it(dir ,QDir::Files
                    |QDir::NoDotAndDotDot
                    |QDir::Hidden, QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {

        QFileInfo fi( it.next());
        //  QString mim=EMimIcon::mimeTyppe(fi);
        QModelIndex idx=myModel->index(fi.filePath());
        QString mim;

        if(idx.isValid())
            mim=idx.data(_MMIM).toString();
        else
            continue;

        if(mim.startsWith("image")&& list.contains(mim.toLatin1())){

            QFuture<void> f1 =QtConcurrent::run(this, &Tab::saveImageThumb,fi);
            f1.waitForFinished();

            //QtConcurrent::run(this, &ItemDelegate::saveImageThumb,fi,fileThumbnail);

        }

        qApp->processEvents();
    }

 Messages::debugMe(0,__LINE__,"Tab",__FUNCTION__,"End");
}

/*****************************************************************************
 *  انشاء مصغرات اذا لم تكن موجودة
 *  عند فتح اي مجلد والبحث عن الصور اذا كانت كوجودة
 ****************************************************************************/

void Tab::saveImageThumb(const QFileInfo &fi)
{

    QImageReader reader(fi.filePath());
    if(!reader.canRead())
        return;

    if(qMax(reader.size().width(),reader.size().height())<=128){
     return;
    }

    QMessageAuthenticationCode code(QCryptographicHash::Md5);
    code.addData(fi.filePath().toLatin1());

    QString thumbnail=Edir::thumbnaileCachDir();
    QString fileThumbnail=thumbnail+"/"+code.result().toHex();

    //qDebug()<<"creatthumb"<<fi.filePath();
    if(QFile::exists(fileThumbnail)){

        reader.setFileName(fileThumbnail);
        if(reader.canRead()){
            QString  fModified=reader.text("DATETIME");
            if(fModified== fi.lastModified().toString("dd MM yyyy hh:mm:ss"))
                return;
        }

    }

    QImage image;
    if( image.load(fi.filePath()))
    {
       qDebug()<<"saveImageThumb"<<__LINE__<<fi.filePath();
        image= image.scaled(QSize(128,128),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        image.setText("DATETIME",fi.lastModified().toString("dd MM yyyy hh:mm:ss"));
        QByteArray format="jpg";
        if(image.hasAlphaChannel())
            format="png";

        if(image.save(fileThumbnail,format,50))   {
            //        QSettings setting("elokab","thumbnails");
            //        setting.setValue(fi.filePath(),QFileInfo(fileThumbnail).fileName());
            qApp->processEvents();
            QModelIndex idx=myModel->index(fi.filePath());
            if(idx.isValid() && pageWidget){
                pageWidget->iconUpdate(idx);

            }

        }// image.save

    }
}



/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::goForward()
{
    if(pageWidget)
        pageWidget->goForward();
}
/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::goBack()
{
    if(pageWidget)
        pageWidget->goBack();
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::goUp()
{
    if(!pageWidget)return;
    QDir dir(pageWidget->curentDir());
    if(!dir.cdUp())return;
    QString url=dir.absolutePath();
    pageWidget->setUrl(url);
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::goHome()
{
    if(pageWidget)
        pageWidget->setUrl(QDir::homePath());
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::goTrash()
{
    if(pageWidget)
        pageWidget->setUrl(":/trash");
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::goSearch()
{
    if(pageWidget)
        pageWidget->setUrl(":/search");
}

/**************************************************************************************
 *
 *
 **************************************************************************************/
void Tab::setHiddenFile(bool visible)
{
    //listSelectionModel->clear();
    if (visible)
        myModel->setFilter( QDir::AllEntries | QDir::System|QDir::NoDotAndDotDot|QDir::Hidden);
    else
        myModel->setFilter( QDir::AllEntries | QDir::System |QDir::NoDotAndDotDot );

}

/**************************************************************************************
 *
 *
 **************************************************************************************/
void Tab::setModelFiltrer(const QString& nf)
{

#ifdef DEBUG_APP
    Messages::showMessage(Messages::NORMALE,"Tab::setModelFiltrer()",nf);
#endif
    //myModel->setNameFilterDisables(true);
    //     QStringList list;
    //     list<<"*"+nf+"*";
    myModel->setNameFilters(QStringList()<<nf);
    QStringList lis=myModel->nameFilters();
    qDebug()<<lis;
}

/**************************************************************************************
 *
 *
 **************************************************************************************/
void Tab::setSorting()
{

    myModel->sort(mSettings->sortBy(),mSettings->sortOrder());

}

/**************************************************************************************
 *
 *
 **************************************************************************************/
void Tab::renameFiles()
{
    if(pageWidget)
        pageWidget->renameFiles();
}

/**************************************************************************************
 *
 *
 **************************************************************************************/
void Tab::deleteFiles()
{
    if(!pageWidget)return;


    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Delete Files."));
    msgBox.setText(tr("Do you want to delete all selected files?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setIcon(QMessageBox::Question);


    int ret = msgBox.exec();
    if(ret== QMessageBox::Cancel)
        return;

    QStringList list;
    list= pageWidget->selectedFiles();
    
    if(list.count()>0){
        // myModel->blockSignals(true);
        QString prog=QApplication::applicationDirPath()+"/elokab-fa";
        QProcess p;
        // p.startDetached(prog,QStringList()<<"-rm"<<list.join(","),QApplication::applicationDirPath());
        p.start(prog,QStringList()<<"-rm"<<list.join(","));
        p.waitForFinished();

    }

}

/**************************************************************************************
 *
 **************************************************************************************/
void Tab::copyFiles()
{
    if(!pageWidget)return;

    QList<QUrl>listUrl;
    foreach (QString s, pageWidget->selectedFiles()) {
        qDebug()<<"Tab::copyFiles()"<<s;
        listUrl<< QUrl(s);

    }

    QMimeData * mimData=new QMimeData;
    mimData->setUrls(listUrl);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimData);

    // QApplication::clipboard()->setMimeData(myModel->mimeData(pageWidget->selectedIndex()));

    mCute=false;
}

/**************************************************************************************
 *
 *
 **************************************************************************************/
void Tab::cutFiles()
{
    if(!pageWidget)return;

    mCute=true;
    QList<QUrl>listUrl;
    foreach (QString s, pageWidget->selectedFiles()) {
        listUrl<< QUrl(s);
    }

    QMimeData * mimData=new QMimeData;
    mimData->setUrls(listUrl);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimData);

}

/**************************************************************************************
 *
 *
 **************************************************************************************/
void Tab::pastFiles()
{
    if(!pageWidget)return;


    QStringList list;
    //   const QClipboard *clipboard = QApplication::clipboard();
    //  QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();
    if(mimeData->hasUrls()){

        foreach (QUrl url, mimeData->urls()) {
            qDebug()<<"pastFiles list"<<url.toString()<<url.toLocalFile();
            list.append(url.toString());

        }
    }

    if(list.count()>0){

        QString prog=QApplication::applicationDirPath()+"/elokab-fa";
        QProcess p;
        QString s=list.join(",");
        qDebug()<<"pastFiles s"<<s;

        if(mCute)
            p.startDetached(prog,QStringList()<<"-m"<<s<<pageWidget->dirPath(),
                            QApplication::applicationDirPath());
        else
            p.startDetached(prog,QStringList()<<"-p"<<s<<pageWidget->dirPath(),
                            QApplication::applicationDirPath());
    }

    if(mCute){

        QMimeData   * mimData=new QMimeData;
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setMimeData(mimData);

        //  QApplication::clipboard()->setMimeData(QMimeData());

    }

    mCute=false;
}

/**************************************************************************************
 *
 *
 **************************************************************************************/
void Tab::moveFilesToTrash()
{
    if(!pageWidget)return;

    QStringList list;
    list= pageWidget->selectedFiles();
    if(list.count()>0){
        QString prog=QApplication::applicationDirPath()+"/elokab-fa";
        QProcess p;
        p.startDetached(prog,QStringList()<<"-mt"<<list.join(","),
                        QApplication::applicationDirPath());

    }
    //  pageWidget->moveFilesToTrash();
    mCute=false;


}

/**************************************************************************************
 *
 *
 **************************************************************************************/
void Tab::selectAll()
{
    if(!pageWidget)return;
    pageWidget->selectAll();
}

/**************************************************************************************
 *
 *
 **************************************************************************************/
void Tab::slotShowOpenwithDlg()
{
    if(!pageWidget)return;

    pageWidget->showOpenwithDlg(pageWidget->selectedFile());
}

/**************************************************************************************
 *
 *
 **************************************************************************************/
void Tab::slotShowProperties()
{
    if(!pageWidget)return;

    pageWidget->showProperties();
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::dragDropFiles(bool copy,QString path, QStringList list)
{
    if(mSettings->confirmDragDrop()){

        QString action=tr("Move");
        if(copy)action=tr("Copy");

        QMessageBox msgBox;

        msgBox.setWindowTitle(tr("%1 Files.").arg(action));
        //msgBox.setTextFormat(Qt::PlainText);

        msgBox.setText(tr("Do you want to %1 all selected files.").arg(action));
        msgBox.setInformativeText(tr("To %1 ?").arg(path));

        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.setIcon(QMessageBox::Question);

        int ret = msgBox.exec();
        if(ret== QMessageBox::No)
            return;

    }

    //qDebug()<<"Tab"<<0,__LINE__<<"dragDropFiles";
    QString prog=QApplication::applicationDirPath()+"/elokab-fa";
    QProcess p;
    if(copy)
        p.startDetached(prog,
                        QStringList()<<"-p"<<list.join(",")<<path,
                        QApplication::applicationDirPath());
    else
        p.startDetached(prog,
                        QStringList()<<"-m"<<list.join(",")<<path,
                        QApplication::applicationDirPath());

    //qDebug()<<"Tab"<<0,__LINE__<<"dragDropFiles End";
}
