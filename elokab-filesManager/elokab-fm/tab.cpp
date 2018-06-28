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

#include "tab.h"
//#include "filesactions.h"
//#include <EMimIcon>
#include "messages.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QMessageBox>
#include <QUrl>
#include <QProcess>
#include <QToolButton>
#include <QPainter>
#include <QtConcurrentRun>
#include <QImageReader>
#include <QDateTime>
//#ifdef DEBUG_APP
#include <QDebug>
//#endif

/*****************************************************************************************************
 *
 *
 *****************************************************************************************************/

MyFileSystemModel::MyFileSystemModel(IconProvider *iconProvider,QObject *parent):
    mIconProvider(iconProvider), QFileSystemModel(parent)
{
    qDebug()<<__FILE__<<__LINE__<<"MyFileSystemModel";
       iconCach=new QHash<QString,QIcon>;
    setRootPath("/");
    setNameFilterDisables(false);
    setReadOnly(false);
    setResolveSymlinks(true);
   // setIconProvider(iconProvider);

    qDebug()<<__FILE__<<__LINE__<<"MyFileSystemModel end";
}

//--------------------------------------------------------------
QVariant MyFileSystemModel::data(const QModelIndex &index, int role) const
{



    if (!index.isValid())  return QVariant();


    if(index.column()==0 && role == Qt::ToolTipRole){
            return fileName(index);
    }


//    if(index.column()==0 && role == Qt::StatusTipRole){
//            return filePath(index);
//    }

    //تحميل نوع الملف بالغة النظام
    if(index.column()==2 && role == Qt::DisplayRole){
            return mIconProvider->type(fileInfo(index));
    }


      return QFileSystemModel::data(index,role);
        

}


//--------------------------------------------------------------
QVariant MyFileSystemModel::headerData(int section,
                                       Qt::Orientation orientation,
                                       int role) const
{

    if(orientation==Qt::Horizontal)
        if(role == Qt::DisplayRole)
            switch(section)
            {
            case 0: return tr("Name");
            case 1: return tr("Size");
            case 2: return tr("Type");
            case 3: return tr("Date Modified");

            default: return QVariant();
            }

    return QVariant();

}

//--------------------------------------------------------------
bool MyFileSystemModel::dropMimeData(const QMimeData * data,
                                     Qt::DropAction action,
                                     int row,
                                     int column,
                                     const QModelIndex & parent )
{
    Q_UNUSED ( action )
    Q_UNUSED ( row )
    Q_UNUSED ( column )

    qDebug()<<__FILE__<<__LINE__<<"dropMimeData Begin";
    if(isDir(parent))
    {
        QList<QUrl> files = data->urls();
        QStringList filesList;

        if(QFileInfo(files.at(0).path()).canonicalPath() == filePath(parent))
        {
            qDebug()<<"return canonicalPath()"<<QFileInfo(files.at(0).path()).canonicalPath()  ;
            return false;

        }

        foreach(QUrl item, files)
            filesList.append(item.path());

        Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();

        if(mods == Qt::ControlModifier)
            emit dragDropFiles(true, filePath(parent), filesList);
        else
            emit dragDropFiles(false, filePath(parent), filesList);

        //  qDebug()<<data<< filePath(parent)<<filesList;

    }
    qDebug()<<__FILE__<<__LINE__<<"dropMimeData End";
    return false;
}


/*****************************************************************************************************
 *
 *                                   TAB
 *
 *****************************************************************************************************/

Tab::Tab(Settings *setting, IconProvider *iconProvider, Actions *actions, QWidget *parent) :
    QTabWidget(parent),
    mSettings (setting),
    mIconProvider(iconProvider),
    mActions(actions),
    mCute(0),
    pageWidget(0)
{

    qDebug()<<__FILE__<<__LINE__<<"Tab Begin";
    QToolButton *btn=new QToolButton(this);
    btn->setAutoRaise(true);
    btn->setDefaultAction(mActions->actNewTab);
    setCornerWidget(btn,Qt::TopLeftCorner);
    setMovable(true);
    setTabBarAutoHide(true);
     mFileSystemWatcher=new QFileSystemWatcher;
    myModel = new MyFileSystemModel(mIconProvider);
    //Todo --------------------myModel
    //myModel= new QFileSystemModel;
    //myModel->setRootPath("/");
  // myModel->setIconProvider(mIconProvider);


    mimData=new QMimeData;


   // connect(this,SIGNAL(urlChanged(QString)),this,SLOT(directoryChanged(QString)));
    connect(this,SIGNAL(urlChanged(QString)),this,SLOT(setCurTabText(QString)));
    connect(this,SIGNAL(urlChanged(QString)),mActions,SIGNAL(urlChanged(QString))) ;
    connect(this,SIGNAL(currentChanged(int)),this,SLOT(currentTabChanged(int)));
    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(oncloseRequested(int)));

    connect(mActions,SIGNAL(sendNewTab(QString)),this,SLOT(addNewTab(QString)));
    connect(mActions,SIGNAL(sendCloseTab()),this,SLOT(closeCurentTab()));
    connect(mActions,SIGNAL(sendGoUpDir()),this,SLOT(goUp()));
    connect(mActions,SIGNAL(sendGoBackDir()),this,SLOT(goBack()));
    connect(mActions,SIGNAL(sendGoForwardDir()),this,SLOT(goForward()));
    connect(mActions,SIGNAL(sendGoHomeDir()),this,SLOT(goHome()));
    connect(mActions,SIGNAL(sendGoTrachDir()),this,SLOT(goTrash()));
    connect(mActions,SIGNAL(sendGoSearch()),this,SLOT(goSearch()));
    connect(mActions,SIGNAL(startEdit()),this,SLOT(renameFiles())) ;
    connect(mActions,SIGNAL(startDelete()),this,SLOT(deleteFiles())) ;
    connect(mActions,SIGNAL(startCopyFile()),this,SLOT(copyFiles())) ;
    connect(mActions,SIGNAL(startCutFile()),this,SLOT(cutFiles())) ;
    connect(mActions,SIGNAL(pasteClipboard()),this,SLOT(pastFiles())) ;
    connect(mActions,SIGNAL(startMoveToTrash()),this,SLOT(moveFilesToTrash())) ;
    connect(mActions,SIGNAL(selectAll()),this,SLOT(selectAll())) ;
    connect(mActions,SIGNAL(showProperties()),this,SLOT(slotShowProperties())) ;
    connect(mActions,SIGNAL(showOpenwithDlg()),this,SLOT(slotShowOpenwithDlg())) ;
    connect(mActions,SIGNAL(reloadIcons()),this,SLOT(updateIcons())) ;

    connect(mActions,SIGNAL(sendNewPath(QString)),this,SLOT(setUrl(QString))) ;
    connect(mSettings,SIGNAL(viewModeChanged(int)),this,SLOT(setViewMode(int))) ;
    connect(mSettings,SIGNAL(showHiddenChanged(bool)),this,SLOT(setHiddenFile(bool))) ;
    connect(mSettings,SIGNAL(showThumbnailsChanged(bool)),this,SLOT(setShowThumbnails(bool))) ;
   // connect(mSettings,SIGNAL(showStandardIconsChanged(bool)),this,SLOT(setStandardIcons(bool))) ;
    connect(mSettings,SIGNAL(sortingChanged()),this,SLOT(setSorting()));

 //   connect(myModel,SIGNAL(directoryLoaded(QString)),this,SLOT(loadIcons(QString)));
    connect(myModel,SIGNAL(dragDropFiles(bool,QString,QStringList)),this,SLOT(dragDropFiles(bool,QString,QStringList)));
   //connect(myModel,SIGNAL(iconUpdate(QModelIndex)),this,SLOT(iconUpdate(QModelIndex)));
 //  connect(myModel,SIGNAL(directoryLoaded(QString)),this,SLOT(directoryChanged(QString)));
   // connect(mFileSystemWatcher,SIGNAL(fileChanged(QString)),this,SLOT(dataFileChanged(QString)));

    setHiddenFile(mSettings->showHidden());
    setShowThumbnails(mSettings->showThumbnails());
    myModel->sort(mSettings->sortBy(),mSettings->sortOrder());

    qDebug()<<__FILE__<<__LINE__<<"Tab End";
}
/*****************************************************************************
 *
 ****************************************************************************/
Tab::~Tab()
{
    qDebug()<<__FILE__<<__LINE__<<"~Tab";
    //  myModel->clearCache();
    mIconProvider->saveCacheIcons();

    for (int i = 0; i < count(); ++i) {
        PageWidget *pWidget= qobject_cast<PageWidget *>(widget(i));

        if(pWidget)
        {

            pWidget->close();

        }
    }
    qDebug()<<__FILE__<<__LINE__<<"~Tab End";
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::closeAll()
{
    qDebug()<<__FILE__<<__LINE__<<"closeAll";
    for (int i = 0; i < count(); ++i) {
        PageWidget *pWidget= qobject_cast<PageWidget *>(widget(i));

        if(pWidget)
        {

            pWidget->closeAll();

        }
    }
    qDebug()<<__FILE__<<__LINE__<<"closeAll end";
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::addNewTab( const QString &url)
{
    qDebug()<<__FILE__<<__LINE__<<"addNewTab";
    QString mUrl=url;
    QFileInfo fi(mUrl);
    if(!fi.isDir()){
        mUrl=fi.path();

        EMimIcon::launchApplication(url);

    }

    pageWidget=new PageWidget(myModel,mSettings,mActions,mUrl,mIconProvider, this);

     // connect(pageWidget,SIGNAL(  isLargeDirectory(QString)),this,SLOT(setLargeDirectory(QString)));
  //  connect(pageWidget,SIGNAL(urlBeginChanged(QString)),this,SLOT(directoryChanged(QString)));

    QDir dir(mUrl);
    addTab(pageWidget,EMimIcon::iconFolder(mUrl),dir.dirName());
    // currentTabChanged(currentIndex());

    myModel->sort(mSettings->sortBy(),mSettings->sortOrder());
    setCurrentWidget(pageWidget);
    setCurrentIndex(currentIndex());

    connect(pageWidget,SIGNAL(urlChanged(QString)),this,SIGNAL(urlChanged(QString)));
    connect(pageWidget,SIGNAL(selectedFoldersFiles(QString)),this,SIGNAL(  selectedFoldersFiles(QString)));
emit tabAdded(mUrl);
    qDebug()<<__FILE__<<__LINE__<<"addNewTab End";
}

void Tab::iconUpdate(QModelIndex index)
{
    qDebug()<<__FILE__<<__LINE__<<"iconUpdate";

    if(index.isValid()){
        if(pageWidget)
            pageWidget->iconUpdate(index);
    }

    qDebug()<<__FILE__<<__LINE__<<"iconUpdate End";
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::setShowThumbnails(bool arg)
{
    qDebug()<<__FILE__<<__LINE__<<"setShowThumbnails";

    mIconProvider->setShowThambnails(arg);

    myModel->setPreview(arg);

    QApplication::setOverrideCursor(Qt::WaitCursor) ;

  //  myModel-> setIconProvider(mIconProvider);

    mSowTumbnails=arg;

    updateIcons();

    QApplication::restoreOverrideCursor();

    qDebug()<<__FILE__<<__LINE__<<"setShowThumbnails End";
}

/*****************************************************************************
 *
 *
 ****************************************************************************/


void Tab::updateIcons()
{
    qDebug()<<__FILE__<<__LINE__<<"updateIcons";

    if(!pageWidget)return;

      QString currentPath=pageWidget->curentDir();

     QtConcurrent::run(this, &Tab::creatThumb,currentPath);

    QApplication::setOverrideCursor(Qt::WaitCursor) ;

    for(int x = 0; x < myModel->rowCount(myModel->index(currentPath)); ++x){
        QModelIndex idx=myModel->index(x,0,myModel->index(currentPath));
        pageWidget->iconUpdate(idx);
    }

   // pageWidget->setUrl( pageWidget->dirPath());

    QApplication::restoreOverrideCursor();


    qDebug()<<__FILE__<<__LINE__<<"updateIcons End";
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::closeCurentTab()
{
    qDebug()<<__FILE__<<__LINE__<<"closeCurentTab";

    if(count()==1)return;

    int index=currentIndex();

    oncloseRequested( index);

    qDebug()<<__FILE__<<__LINE__<<"closeCurentTab End";
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::oncloseRequested(int index)
{
    qDebug()<<__FILE__<<__LINE__<<"oncloseRequested";

    if(count()==1)return;

    PageWidget *pWidget= qobject_cast<PageWidget *>(widget(index));

    if(pWidget)
    {
        QString path=pWidget->curentDir();
        mActions->addClosedTab(path);

        delete pWidget;
    }

    setTabsClosable(count()-1);

    qDebug()<<__FILE__<<__LINE__<<"oncloseRequested End";
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::setCurTabText(const QString &title)
{
    qDebug()<<__FILE__<<__LINE__<<"setCurTabText"<<title;

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
    qDebug()<<__FILE__<<__LINE__<<"setCurTabText End";
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::currentTabChanged(int index)
{
    qDebug()<<__FILE__<<__LINE__<<"currentTabChanged"<<index;

      setTabsClosable(count()-1);

    pageWidget= qobject_cast<PageWidget *>(widget(index));
    if(pageWidget)
        pageWidget->updateSignals();

    qDebug()<<__FILE__<<__LINE__<<"currentTabChanged End";

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


void Tab::directoryChanged(const QString &dir)
{
    qDebug()<<__FILE__<<__LINE__<<"directoryChanged"<<dir;

       if(pageWidget ){
          //  QtConcurrent::run(this, &Tab::creatThumb,dir);
        }

    qDebug()<<__FILE__<<__LINE__<<"directoryChanged END";
}

/*****************************************************************************
 *  انشاء مصغرات اذا لم تكن موجودة
 *  عند فتح اي مجلد والبحث عن الصور اذا كانت كوجودة
 ****************************************************************************/

void Tab::creatThumb(const QString &path)
{

    qDebug()<<__FILE__<<__LINE__<<"creatThumb"<<path;
    QString thumbnail=Edir::cachDir()+"/thambnail";
    QDir dir(thumbnail);
    dir.mkpath(thumbnail);
    QImage pix;
    QImageReader reader;
//    qDebug()<<reader.supportedImageFormats();
//    qDebug()<<reader.supportedMimeTypes();

    QList<QByteArray> list=reader.supportedMimeTypes();

    QSettings setting("elokab","thambnail");
    QDirIterator it(path ,QDir::Files|QDir::Hidden, QDirIterator::NoIteratorFlags);

    while (it.hasNext()) {
        QString file= it.next();
        QFileInfo fi(file);
        QString mim=EMimIcon::mimeTyppe(fi);

        if(mim.startsWith("image")&& list.contains(mim.toLatin1())){

            //-------------------------------------
            QByteArray text=file.toUtf8();

            QString fileThumbnail=thumbnail+"/"+text.toHex();

            if(QFile::exists(fileThumbnail)){
               reader.setFileName(fileThumbnail);
                QString  fModified=reader.text("DATETIME");
                qDebug()<<"exist"<<fModified<<file;
                //QString fModified=setting.value(file).toString();
                if(fModified== fi.lastModified().toString("dd MM yyyy hh:mm:ss"))
                    continue;

            }
 reader.setFileName(file);

            //  pix.load((file));
            if(qMax(reader.size().width(),reader.size().height())>128){
 qDebug()<<"created"<<reader.size()<<file;
                if(fi.absolutePath()!=thumbnail){

                    //  pix= QImage(128,128,QImage::Format_ARGB32);
                    pix.load(file);
                    pix.setText("DATETIME",fi.lastModified().toString("dd MM yyyy hh:mm:ss"));
                    pix= pix.scaled(QSize(128,128),Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    pix.setText("DATETIME",fi.lastModified().toString("dd MM yyyy hh:mm:ss"));

                    // setting.setValue(file,fi.lastModified().toString("dd MM yyyy hh:mm:ss"));

                    pix.save(fileThumbnail,"png",50);
                    QModelIndex idx=myModel->index(file);

                    if(idx.isValid() && pageWidget){
                        pageWidget->iconUpdate(idx);
                        qApp->processEvents();
                    }

                }
            }

            pix=QImage();


            //---------------------------------------------------------------
        }

    }
    qDebug()<<__FILE__<<__LINE__<<"creatThumb END";

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
    msgBox.setText("Delete Files.");
    msgBox.setInformativeText("Do you want to delete all selected files?");
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
        // myModel->blockSignals(false);
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

    //mimData->clear();

    QMimeData   * mimData=new QMimeData;
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

    QMimeData   * mimData=new QMimeData;
    mimData->setUrls(listUrl);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimData);



    //  QApplication::clipboard()->setMimeData(myModel->mimeData(pageWidget->selectedIndex()));

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
            //for qt4
            // list.append(url.toString());
            //for qt5
            list.append(url.toString());

        }
    }

    if(list.count()>0){

        QString prog=QApplication::applicationDirPath()+"/elokab-fa";
        QProcess p;
        QString s=list.join(",");
        qDebug()<<"pastFiles s"<<s;

        if(mCute)
            p.startDetached(prog,QStringList()<<"-m"<<s<<pageWidget->dirPath(),QApplication::applicationDirPath());

        else
            p.startDetached(prog,QStringList()<<"-p"<<s<<pageWidget->dirPath(),QApplication::applicationDirPath());


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
        p.startDetached(prog,QStringList()<<"-mt"<<list.join(","),QApplication::applicationDirPath());

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
    qDebug()<<__FILE__<<__LINE__<<"dragDropFiles";
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

    qDebug()<<__FILE__<<__LINE__<<"dragDropFiles End";
}
