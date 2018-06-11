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
/*****************************************************************************************************
 *
 *
 *****************************************************************************************************/

MyFileSystemModel::MyFileSystemModel(IconProvider *iconProvider,QObject *parent):
    mIconProvider(iconProvider), QFileSystemModel(parent)
{
    iconCach=new QHash<QString,QIcon>;
      iconDesktopCach=new QHash<QString,QIcon>;
    //imageCach=new QHash<QString,QIcon>;
    hashImages=new QHash<QString,QByteArray>;

    setRootPath("/");
    setNameFilterDisables(false);
    setReadOnly(false);
    setResolveSymlinks(true);

}


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


   //  QFileInfo fi=fileInfo(index);



    if (role == Qt::ToolTipRole){
        if (!index.isValid())  return QVariant();

        return fileName(index);
    }

    if (role == Qt::StatusTipRole)
    {
        if (!index.isValid())  return QVariant();

        return filePath(index);
    }

    if (role == Qt::DisplayRole){
        if (!index.isValid()) return QVariant();

        if(index.column() == 2) {
            return mIconProvider->type(fileInfo(index));
        }
    }

    if (role == Qt::DecorationRole) {

        if (!index.isValid()) return QVariant();

        if(index.column() == 0) {



            QString mim= EMimIcon::mimeTyppe(fileInfo(index));
            QIcon reticon;//= iconProvider()->icon(fi);

            //DIR---------------------------
            if(fileInfo(index).isDir()){

                reticon= EMimIcon::iconFolder(filePath(index));

            }
            //IMAGE-------------------------
            else if(mPreview && mim.startsWith("image")  ){
                if(hashImages->contains(filePath(index))){
                    QPixmap pix;
                    pix.loadFromData(hashImages->value(filePath(index)));
                    if(!pix.isNull())
                        reticon=QIcon(pix);

                }else{
                    QtConcurrent::run(this,&MyFileSystemModel::loadImage,filePath(index));
//                    reticon=EMimIcon::iconByMimType(mim,filePath(index));
//                    if(!reticon.isNull())
//                        iconCach->insert(mim,reticon);
                }

            }
            //DX-DESKTOP---------------------
            else if(mim.contains("x-desktop")){
                if(iconDesktopCach->contains(filePath(index)))
                    reticon=iconDesktopCach->value(filePath(index));
                else
                    QtConcurrent::run(this,&MyFileSystemModel::loadIcon,fileInfo(index),mim);

            }
            //MIM---------------------------
            else{
                if(iconCach->contains(mim)){
                    reticon= iconCach->value(mim);
                }else{
                    QtConcurrent::run(this,&MyFileSystemModel::loadIcon,fileInfo(index),mim);
                }

            }

            if(reticon.isNull())
                reticon= iconCach->value(mim);

            if(reticon.isNull())
                reticon= mIconProvider->iconStandard(fileInfo(index));

            if(reticon.isNull())
                reticon= iconProvider()->icon(fileInfo(index));

            if(fileInfo(index).isSymLink())
                reticon= EMimIcon::iconSymLink(reticon);


            if(fileInfo(index).isHidden())
                reticon= iconHiden(reticon);


            return reticon;

        }

    }

    if (!index.isValid()) return QVariant();


    return QFileSystemModel::data(index,role);
}


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

        if(!retIcon.isNull() ){
            iconCach->insert(mim,retIcon);


            if(index(fpath).isValid()&& QFile::exists(fpath))
                emit iconUpdate(index(fpath));

        }



    }




}
void MyFileSystemModel::refreshIcons(const QString &dir)
{
    //    iconCach->clear();
    //    return;

    QHash<QString, QByteArray>::const_iterator i = hashImages->constBegin();
    QStringList list;
    while (i != hashImages->constEnd()) {
        //   cout << i.key() << ": " << i.value() << endl;

        QFileInfo fi(i.key());
        qDebug()<<i.key()<<dir;
        if(fi.absoluteDir().path()==dir)
            list.append(i.key());
        ++i;
    }
    foreach (QString s, list) {
        hashImages->remove(s);
    }

    QHash<QString, QIcon>::const_iterator r = iconCach->constBegin();
    QStringList listIcons;
    while (r != iconCach->constEnd()) {
        //   cout << i.key() << ": " << i.value() << endl;

        QFileInfo fi(r.key());
        qDebug()<<r.key()<<dir;
        if(fi.absoluteDir().path()==dir)
            listIcons.append(r.key());
        ++r;
    }
    foreach (QString s, listIcons) {
        iconCach->remove(s);
    }

}

void MyFileSystemModel::clearCache()
{
    hashImages->clear();
    iconCach->clear();
}

void MyFileSystemModel::loadIcons(QModelIndexList indexes)
{
qApp->setOverrideCursor(QCursor(Qt::WaitCursor));

    foreach (QModelIndex item, indexes)
    {
        if(!item.isValid())
            continue;
   //just wait for loading filles
   qApp->processEvents();

    }
  qApp->restoreOverrideCursor();
}

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

    if(isDir(parent))
    {
        QList<QUrl> files = data->urls();
        QStringList filesList;


        if(QFileInfo(files.at(0).path()).canonicalPath() == filePath(parent))
        {
            qDebug()<<"canonicalPath()"<<QFileInfo(files.at(0).path()).canonicalPath()  ;
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

    QToolButton *btn=new QToolButton(this);
    btn->setAutoRaise(true);
    btn->setDefaultAction(mActions->actNewTab);
    setCornerWidget(btn,Qt::TopLeftCorner);
    setMovable(true);
    setTabBarAutoHide(true);
    myModel = new MyFileSystemModel(mIconProvider);


    mimData=new QMimeData;
    mDefaultprovider=new  DefaultIconProvider;


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
    connect(mActions,SIGNAL(reloadIcons()),this,SLOT(refreshIcons())) ;
    connect(mActions,SIGNAL(sendNewPath(QString)),this,SLOT(setUrl(QString))) ;
    connect(mSettings,SIGNAL(viewModeChanged(int)),this,SLOT(setViewMode(int))) ;
    connect(mSettings,SIGNAL(showHiddenChanged(bool)),this,SLOT(setHiddenFile(bool))) ;
    connect(mSettings,SIGNAL(showThumbnailsChanged(bool)),this,SLOT(setShowThambnails(bool))) ;
    connect(mSettings,SIGNAL(showStandardIconsChanged(bool)),this,SLOT(setStandardIcons(bool))) ;
    connect(mSettings,SIGNAL(sortingChanged()),this,SLOT(setSorting()));

    connect(myModel,SIGNAL(directoryLoaded(QString)),this,SLOT(loadIcons(QString)));
    connect(myModel,SIGNAL(dragDropFiles(bool,QString,QStringList)),this,SLOT(dragDropFiles(bool,QString,QStringList)));
    connect(myModel,SIGNAL(iconUpdate(QModelIndex)),this,SLOT(iconUpdate(QModelIndex)));

    setHiddenFile(mSettings->showHidden());
    setShowThambnails(mSettings->showThumbnails());
    myModel->sort(mSettings->sortBy(),mSettings->sortOrder());

#ifdef DEBUG_APP
    Messages::showMessage(Messages::END,"Tab::Tab()");
#endif
}
/*****************************************************************************
 *
 ****************************************************************************/
Tab::~Tab()
{
#ifdef DEBUG_APP
    Messages::showMessage(Messages::TOP,"Tab::~Tab()");
#endif
    for (int i = 0; i < count(); ++i) {
        PageWidget *pWidget= qobject_cast<PageWidget *>(widget(i));

        if(pWidget)
        {

            pWidget->close();

        }
    }
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::closeAll()
{
    for (int i = 0; i < count(); ++i) {
        PageWidget *pWidget= qobject_cast<PageWidget *>(widget(i));

        if(pWidget)
        {

            pWidget->closeAll();

        }
    }
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::addNewTab( const QString &url)
{

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
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::setLargeDirectory(const QString &dir)
{

#ifdef DEBUG_APP
    Messages::showMessage(Messages::NORMALE,"Tab::setLargeDirectory()",dir);
#endif
    if(!listDirectory.contains(dir)){
        //  myModel->setIconProvider(mDefaultprovider);

    }
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::directoryChanged(const QString &dir)
{
    /*
     * اذا كان تحميل المصغرات غير مفعل فلا تحمل الصور في الذاكرة
     * لذا سيتم حفظ المسارات الغير محملة بالصور
     * وفي حالة تفعيل المصغرات والعودة الى المسار سيتم تحميلها
     *  لان المسارات المحملة سابقا لا تحمل من جديد في
     *  directoryLoaded
     *  ثم تحذف المسارات
    */
    if(!mSettings->showThumbnails()){

        if(!listDirectory.contains(dir))
            listDirectory.append(dir);

    }else{

        if(listDirectory.contains(dir)){

            listDirectory.removeOne(dir);
            loadIcons(dir);
            // mSowTumbnails=arg;
            //  myModel->clearCache();
            // myModel->setRootPath("/");

        }
    }

}

void Tab::loadIcons(const QString &currentPath)
{
     qDebug()<<"dir>>>>>>>>>>>>>>>>>>>>> loaded"<<currentPath;
    QModelIndexList indexes;

    for(int x = 0; x < myModel->rowCount(myModel->index(currentPath)); ++x)
        indexes.append(myModel->index(x,0,myModel->index(currentPath)));

myModel->loadIcons(indexes);
 //TODO FIX QtConcurrent
 // QtConcurrent::run(myModel,&MyFileSystemModel::loadIcons,indexes);

}
void Tab::iconUpdate(QModelIndex index)
{
    if(index.isValid()){
           if(pageWidget)
            pageWidget->iconUpdate(index);
    }

}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::setShowThambnails(bool arg)
{


    mIconProvider->setShowThambnails(arg);
    myModel->setPreview(arg);

    refreshIcons();
    //myModel->setIconProvider(mIconProvider);

}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::setStandardIcons(bool arg)
{
    mIconProvider->setStandardIcon(arg);

    //  myModel->setIconProvider(mIconProvider);

}

void Tab::refreshIcons()
{
    if(!pageWidget)return;

    QString currentPath=pageWidget->curentDir();
    QApplication::setOverrideCursor(Qt::WaitCursor) ;
    myModel->refreshIcons(currentPath);
    loadIcons(currentPath);
    for(int x = 0; x < myModel->rowCount(myModel->index(currentPath)); ++x)
        pageWidget->iconUpdate(myModel->index(x,0,myModel->index(currentPath)));

    pageWidget->urlChanged( pageWidget->dirPath());

    QApplication::restoreOverrideCursor();



}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::closeCurentTab()
{
    if(count()==1)return;

    int index=currentIndex();
    oncloseRequested( index);

}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::oncloseRequested(int index)
{
    if(count()==1)return;

    PageWidget *pWidget= qobject_cast<PageWidget *>(widget(index));

    if(pWidget)
    {
        QString path=pWidget->curentDir();
        mActions->addClosedTab(path);

        delete pWidget;
    }

    setTabsClosable(count()-1);
}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::setCurTabText(const QString &title)
{
    qDebug()<<title;
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

}

/*****************************************************************************
 *
 *
 ****************************************************************************/
void Tab::currentTabChanged(int index)
{
#ifdef DEBUG_APP
#endif
    Messages::showMessage(Messages::NORMALE,"Tab::currentTabChanged()",QString::number(index));
    qDebug()<<"Tab::currentTabChanged()"<<index;
    setTabsClosable(count()-1);

    pageWidget= qobject_cast<PageWidget *>(widget(index));
    if(pageWidget)
        pageWidget->updateSignals();

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
    //  myModel->setIconProvider(mIconProvider);
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
    // myModel->blockSignals(true);

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

    // myModel->blockSignals(false);
}
