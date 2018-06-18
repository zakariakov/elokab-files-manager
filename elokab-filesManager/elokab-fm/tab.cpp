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
//#ifdef DEBUG_APP
#include <QDebug>
//#endif


Thumbnail::Thumbnail(QFileInfo fi,bool preview, QModelIndex index, QObject *parent)
: QObject(parent)
{

    m_index=index;
    m_info=fi;
mPreview=preview;
        m_render = new QFutureWatcher< void >(this);

    connect(m_render, SIGNAL(finished()),
            SLOT(on_render_finished()));

    connect(this, SIGNAL(imageReady(QIcon))
            , SLOT(on_imageReady(QIcon)));

  //  qApp->processEvents();
    startRender();

}
Thumbnail::~Thumbnail()
{

}

void Thumbnail::on_render_finished()
{
    if(m_index.isValid())
    imageFiniched(m_index);
}

void Thumbnail::on_imageReady(QIcon icon)
{

        if(!m_render->isCanceled())
        {
            m_icon = icon;
        }

}

void Thumbnail::startRender()
{
    if(!m_render->isRunning())
    {
       // qApp->processEvents();
        m_render->setFuture(QtConcurrent::run(this, &Thumbnail::render));
    }
}

void Thumbnail::cancelRender()
{
    m_render->cancel();
    m_icon = QIcon();
}

//____________________________________________________________________
void Thumbnail::render()
{


    if(m_render->isCanceled()){ return; }


 QImage  image;

 QString fpath=m_info.filePath();

 if(fpath.isEmpty())return;
 if(!QFile::exists(fpath))return;



 QString mim= EMimIcon::mimeTyppe(m_info);
 if(mim.isEmpty())return;

 QIcon retIcon;//= iconProvider()->icon(fi);

 //DIR----------------------------
 if(m_info.isDir()){

     retIcon= EMimIcon::iconFolder(fpath);

 }
 //X-DESKTOP---------------------
 else if(mim.contains("x-desktop")){

     retIcon=EMimIcon::iconDesktopFile(fpath);


 }
 //IMAGE-------------------------
 else if(mPreview && mim.startsWith("image")  ){

     QPixmap pix;
     pix.loadFromData(EMimIcon::iconThambnail(fpath));
     retIcon=QIcon(pix);

 }
 //MIM---------------------------
 else{

     retIcon=EMimIcon::iconByMimType(mim,fpath);

 }


 if(m_info.isSymLink())
     retIcon= EMimIcon::iconSymLink(retIcon);

// if(!retIcon.isNull() ){
//     iconCach->insert(fpath,retIcon);

//     if(index(fpath).isValid()&& QFile::exists(fpath))
//         emit iconUpdate(m_index(fpath));
////qApp->processEvents();
// }


    if(m_render->isCanceled()){ return;  }


    emit imageReady(retIcon);
}

/*****************************************************************************************************
 *
 *
 *****************************************************************************************************/

MyFileSystemModel::MyFileSystemModel(IconProvider *iconProvider,QObject *parent):
    mIconProvider(iconProvider), QFileSystemModel(parent)
{
    qDebug()<<__FILE__<<__LINE__<<"MyFileSystemModel";
       iconCach=new QHash<QString,QIcon>;
    //    iconmimCach=new QHash<QString,QIcon>;

    //      iconDesktopCach=new QHash<QString,QIcon>;
    //    //imageCach=new QHash<QString,QIcon>;
    //    hashImages=new QHash<QString,QByteArray>;
       mThumbnails=new QHash< QModelIndex,Thumbnail* > ;
    //    hashIndex=new QHash<QString, QModelIndex > ;
    setRootPath("/");
    setNameFilterDisables(false);
    setReadOnly(false);
    setResolveSymlinks(true);
    setIconProvider(iconProvider);
    //openIconsCache

    //        QFile fileIcons(Edir::cachDir()+"/iconsCach.cache");
    //    qDebug()<<"iconCach"<<fileIcons.fileName();
    //        fileIcons.open(QIODevice::ReadOnly);
    //        QDataStream out(&fileIcons);
    //        out >> *iconCach;
    //        fileIcons.close();

    qDebug()<<__FILE__<<__LINE__<<"MyFileSystemModel end";
}

/*
QIcon iconHiden(QIcon icon)
{
    QPixmap pix(128,128);
    pix.fill(Qt::transparent);
    QPixmap pixIcon=(icon.pixmap(128));
    QPainter painter(&pix);
    painter.setOpacity(0.6);
    painter.drawPixmap(pix.rect(),pixIcon,pixIcon.rect());
    return QIcon((pix));
}



QVariant MyFileSystemModel::data(const QModelIndex &index, int role) const
{



    if (!index.isValid())  return QVariant();

    
       if (role == Qt::DecorationRole) {
        if(index.column() == 0) {
            if(fileName(index).isEmpty())return QFileSystemModel::data(index,role);
            if(fileName(index)==".")return QFileSystemModel::data(index,role);
            if(fileName(index)=="..")return QFileSystemModel::data(index,role);

            QIcon reticon=imageThumbnail(index);
            if(reticon.isNull())
                reticon=QIcon::fromTheme("unknown");

            if (index.isValid())
            return reticon;


        }

    }


        return QFileSystemModel::data(index,role);
        

}
*/
QIcon MyFileSystemModel::imageThumbnail( QModelIndex index)const
{


   if(mThumbnails->contains(index)){

        Thumbnail *tb=mThumbnails->value(index);
        if(tb){
            QIcon image= tb->icon();

            if(image.isNull())
                tb->startRender();

            return image;
        }

    }

    //qApp->processEvents();
    Thumbnail *tb=new Thumbnail(fileInfo(index),mPreview,index);
    mThumbnails->insert(index,tb);
     connect(tb,SIGNAL(imageFiniched(QModelIndex)),this,SIGNAL(iconUpdate(QModelIndex)));

    return tb->icon();



}

/*
void MyFileSystemModel::loadImage(QString path)const
{

    //images-----------------------------------------------

    if(!QFile::exists(path)){
        qDebug()<<"MyFileSystemModel::loadImage: no exist";
        return;
    }

    //QString path=minfo.filePath();

    if( !hashImages->contains(path)){

        QPixmap pix;
        pix.loadFromData(EMimIcon::iconThambnail(path));
        QIcon icon=QIcon(pix);

        // QIcon icon=EMimIcon::iconThambnail(path);
        if(!icon.isNull() && QFile::exists(path))
        {

            hashImages->insert(path,EMimIcon::iconThambnail(path));

             if(index(path).isValid())
                emit iconUpdate(index(path));

        }

    }

}
void MyFileSystemModel::geticon(QFileInfo fi)const
{
//bool sym=false;
//    if(fi.isSymLink()){
//        if(QFile::exists(fi.symLinkTarget())){
//            fi.setFile(fi.symLinkTarget());
//            sym=true;
//        }
//    }

    QString fpath=fi.filePath();

    if(fpath.isEmpty())return;
    if(!QFile::exists(fpath))return;
    if( iconCach->contains(fpath))  return;


    QString mim= EMimIcon::mimeTyppe(fi);
    if(mim.isEmpty())return;

    QIcon retIcon;//= iconProvider()->icon(fi);

    //DIR----------------------------
    if(fi.isDir()){

        retIcon= EMimIcon::iconFolder(fpath);

    }
    //X-DESKTOP---------------------
    else if(mim.contains("x-desktop")){

        retIcon=EMimIcon::iconDesktopFile(fpath);


    }
    //IMAGE-------------------------
    else if(mPreview && mim.startsWith("image")  ){

        QPixmap pix;
        pix.loadFromData(EMimIcon::iconThambnail(fpath));
        retIcon=QIcon(pix);

    }
    //MIM---------------------------
    else{
      if(iconmimCach->contains(mim))
          retIcon=iconmimCach->value(mim);
      else{
        retIcon=EMimIcon::iconByMimType(mim,fpath);
        iconmimCach->insert(mim,retIcon);
      }
    }


    if(fi.isSymLink())
        retIcon= EMimIcon::iconSymLink(retIcon);

    if(!retIcon.isNull() ){
        iconCach->insert(fpath,retIcon);

        if(index(fpath).isValid()&& QFile::exists(fpath))
            emit iconUpdate(index(fpath));
//qApp->processEvents();
    }


}
void MyFileSystemModel::loadIcon(QFileInfo fi,QString mim)const
{
    //---------------------------------------------------

    QString fpath=fi.filePath();

    if(!QFile::exists(fpath))
        return;

    if(fi.isSymLink()){
        if(QFile::exists(fi.symLinkTarget()))
            fi.setFile(fi.symLinkTarget());
    }


    if(mim.contains("x-desktop")){
        if(!iconDesktopCach->contains(fpath))
        {
            QIcon  retIcon=EMimIcon::iconDesktopFile(fpath);
            if(!retIcon.isNull() ){
                iconDesktopCach->insert(fpath,retIcon);

                if(index(fpath).isValid()&& QFile::exists(fpath)){
                    emit iconUpdate(index(fpath));
                    return;
                }
            }
        }
    }

    //icons---------------------------------------------
    if( !iconCach->contains(mim)){



     QIcon retIcon=EMimIcon::iconByMimType(mim,fpath);
        // QIcon retIcon=mIconProvider->icon(fi);

        if(!retIcon.isNull() ){
            iconCach->insert(mim,retIcon);


            if(index(fpath).isValid()&& QFile::exists(fpath))
                emit iconUpdate(index(fpath));

        }



    }




}

void MyFileSystemModel::refreshIcons(const QString &dir)
{
    qDebug()<<__FILE__<<__LINE__<<"refreshIcons Begin";
    //    iconCach->clear();
  setIconProvider(mIconProvider);

//    QDir mDir(dir);
//    QStringList mlist=mDir.entryList();
//    foreach (QString s, mlist) {
//        QString file=dir+"/"+s;
//        if(index(file).isValid()){
//            qDebug()<<"updat:"<<file;
//            QFileInfo fi(file);
//              setData(index(file),mIconProvider->icon(fi),Qt::DisplayRole);


//        }

//    }

    qDebug()<<__FILE__<<__LINE__<<"refreshIcons End";
}


void MyFileSystemModel::clearCache()
{


    QHash<QString, QIcon>::const_iterator r = iconCach->constBegin();
    QStringList listIcons;
    while (r != iconCach->constEnd()) {

        if(!QFile::exists(r.key())){

            listIcons.append(r.key());
        }
        ++r;
    }
    foreach (QString s, listIcons) {
        qDebug()<<"removing"<<s;
        iconCach->remove(s);
    }



    QFile file(Edir::cachDir()+"/iconsCach.cache");
    qDebug()<<"saving"<<file.fileName();
    QDataStream out(&file);

    file.open(QIODevice::WriteOnly);
    out.setDevice(&file);
    out << *iconCach;
    file.close();

}

void MyFileSystemModel::loadIcons(QModelIndexList indexes)
{
qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
//this->blockSignals(true);
    foreach (QModelIndex item, indexes)
    {
        if(!item.isValid())
            continue;
   //just wait for loading files
          //  QtConcurrent::run(this,&MyFileSystemModel::geticon,fileInfo(item));
//geticon(fileInfo(item));
   qApp->processEvents();

    }
  qApp->restoreOverrideCursor();
//  this->blockSignals(false);
}
*/
QVariant MyFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
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
//   int columnCount(const QModelIndex &parent) const;
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

#ifdef DEBUG_APP
    Messages::showMessage(Messages::TOP,"Tab::Tab()");
#endif
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
    //myModel->setIconProvider(mIconProvider);


    mimData=new QMimeData;


    connect(this,SIGNAL(urlHasChanged(QString)),this,SLOT(directoryChanged(QString)));
    connect(this,SIGNAL(urlHasChanged(QString)),this,SLOT(setCurTabText(QString)));
    connect(this,SIGNAL(urlHasChanged(QString)),mActions,SIGNAL(urlChanged(QString))) ;
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

    connect(myModel,SIGNAL(directoryLoaded(QString)),this,SLOT(loadIcons(QString)));
    connect(myModel,SIGNAL(dragDropFiles(bool,QString,QStringList)),this,SLOT(dragDropFiles(bool,QString,QStringList)));
   connect(myModel,SIGNAL(iconUpdate(QModelIndex)),this,SLOT(iconUpdate(QModelIndex)));
    connect(myModel,SIGNAL(rootPathChanged(QString)),this,SLOT(directoryChanged(QString)));
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

    connect(pageWidget,SIGNAL(urlHasChanged(QString)),this,SIGNAL(urlHasChanged(QString)));
    connect(pageWidget,SIGNAL(  selectedFoldersFiles(QString)),this,SIGNAL(  selectedFoldersFiles(QString)));
    connect(pageWidget,SIGNAL(  isLargeDirectory(QString)),this,SLOT(setLargeDirectory(QString)));

    QDir dir(mUrl);
    addTab(pageWidget,EMimIcon::iconFolder(mUrl),dir.dirName());
    // currentTabChanged(currentIndex());

    myModel->sort(mSettings->sortBy(),mSettings->sortOrder());
    setCurrentWidget(pageWidget);
    setCurrentIndex(currentIndex());
    qDebug()<<__FILE__<<__LINE__<<"addNewTab End";
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::setLargeDirectory(const QString &dir)
{
    qDebug()<<__FILE__<<__LINE__<<"setLargeDirectory";

    if(!listDirectory.contains(dir)){
        listDirectory.append(dir);
        myModel->setIconProvider(new QFileIconProvider());
        qApp->setOverrideCursor(QCursor(Qt::WaitCursor));

    }

    qDebug()<<__FILE__<<__LINE__<<"setLargeDirectory End";
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::dataFileChanged(const QString &dir)
{
    qDebug()<<"file changed>>"<<dir;
   // myModel->setRootPath()
}

void Tab::directoryChanged(const QString &dir)
{
  // qDebug()<<"directoryChanged"<<dir;
//    QDirIterator it(dir);
//     while (it.hasNext()) {
//         QString s=it.next();
//    qDebug()<<"mFileSystemWatcher" << s;
// mFileSystemWatcher->addPath(s);

//     }


           /*
     * اذا كان تحميل المصغرات غير مفعل فلا تحمل الصور في الذاكرة
     * لذا سيتم حفظ المسارات الغير محملة بالصور
     * وفي حالة تفعيل المصغرات والعودة الى المسار سيتم تحميلها
     *  لان المسارات المحملة سابقا لا تحمل من جديد في
     *  directoryLoaded
     *  ثم تحذف المسارات
    */

}

void Tab::loadIcons(const QString &currentPath)
{
    qDebug()<<__FILE__<<__LINE__<<"loadIcons()>>>>> loaded"<<currentPath;

    //     if(!listDirectory.contains(currentPath)){
    //         listDirectory.append(currentPath);
    //     }

    qApp->restoreOverrideCursor();
    if(myModel->iconProvider()!=mIconProvider){
        //if(listDirectory.contains(currentPath))
        qDebug()<<"dir>>>>>>>>>>>>>>>>>>>>> mIconProvider";
       myModel->setIconProvider(mIconProvider);
    }


    qDebug()<<__FILE__<<__LINE__<<"loadIcons End";
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

    myModel-> setIconProvider(mIconProvider);



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

    QApplication::setOverrideCursor(Qt::WaitCursor) ;

    for(int x = 0; x < myModel->rowCount(myModel->index(currentPath)); ++x){
        QModelIndex idx=myModel->index(x,0,myModel->index(currentPath));
        pageWidget->iconUpdate(idx);
    }

    pageWidget->urlChanged( pageWidget->dirPath());

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
    qDebug()<<__FILE__<<__LINE__<<"setCurTabText";
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
    qDebug()<<__FILE__<<__LINE__<<"currentTabChanged"<<QString::number(index);

    qDebug()<<"Tab::currentTabChanged()"<<index;
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
    if(pageWidget)
        pageWidget->setUrl(url);
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
    
#ifdef DEBUG_APP
    Messages::showMessage(Messages::NORMALE,"Tab::deleteFiles()",list.join(","));
#endif
    
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

#ifdef DEBUG_APP
    Messages::showMessage(Messages::NORMALE,"Tab::copyFiles()",pageWidget->selectedFiles().join(","));
#endif
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


#ifdef DEBUG_APP
    Messages::showMessage(Messages::NORMALE,"Tab::cutFiles()",pageWidget->selectedFiles().join(","));
#endif
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
#ifdef DEBUG_APP
    Messages::showMessage(Messages::NORMALE,"Tab::pastFiles()",list.join(","));
#endif
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
