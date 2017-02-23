#include "eiconengin.h"
#include <QPainter>
#include <QDebug>
#include <QDirIterator>
EIconEngin::EIconEngin(const QString &iconName):
    m_iconName(iconName),isfallback(true)
{

   m_icon=QIcon::fromTheme(iconName);

}

//_____________________________________________________________________________
void EIconEngin::paint(QPainter *painter, const QRect &rect,
                             QIcon::Mode mode, QIcon::State state)
{
    QSize size=m_icon.actualSize(rect.size());
    painter->drawPixmap(rect, pixmap(size,mode,state));
}

//_____________________________________________________________________________
QPixmap EIconEngin::pixmap(const QSize &size, QIcon::Mode mode,
                                 QIcon::State state)
{
    ensureLoaded();
    QPixmap pix= m_icon.pixmap(size,mode,state);
    //TODO: fix this icon
    if(pix.isNull())
        pix=QIcon::fromTheme("unknown").pixmap(size,mode,state);

    if(size!=pix.size())
        pix=pix.scaled(size);

    return pix;
}

//_____________________________________________________________________________
void EIconEngin::ensureLoaded()
{
    /*****************************************************
     *اذا كانت السمة موجودة سيتم استخدام السمة
     **اما اذا لم تكن هناك سمة سيتم لستخدام الاحتياطية
     *اذا تغيرت السمة سينظر اذا وجدت سيتم انشاء احتياطية
     *واستخدام الحالية وهكذا
     ****************************************************/

    if(!QIcon::hasThemeIcon(m_iconName)){
        if(!isfallback){
            restoreIconThemeBack();
            isfallback=true;
        }
    }else{
        if(isfallback){
            m_icon=QIcon::fromTheme(m_iconName);
            themName=QIcon::themeName();
            isfallback=false;
        }
    }
}

//______________________________________________________________________________
void EIconEngin::restoreIconThemeBack()
{
    // qDebug()<<themBackPath();
    m_icon=QIcon();
    QStringList listStr;
    listStr<<m_iconName+".png"
           <<m_iconName+".svg"
           <<m_iconName+".svgz"
           <<m_iconName+".xpm";

    QDirIterator it( themBackPath(),QDir::AllDirs |QDir::NoSymLinks |
                     QDir::NoDotAndDotDot , QDirIterator::Subdirectories);
    while(it.hasNext())
    {

        it.next();
        QString path=it.filePath();

        QStringList files = QDir(path).entryList(QStringList(listStr),
                             QDir::Files | QDir::NoDotAndDotDot);

        foreach (QString s, files) {
            QString f=  QDir(path).absoluteFilePath(s);
            m_icon.addFile(f/*,QPixmap(f).size()*/);

        }

    }

}

//___________________________________________________________________________
QString EIconEngin::themBackPath()
{

    QString themNamePath;
    foreach (QString p, QIcon::themeSearchPaths()) {
        QDir dirS(p);
        if(dirS.exists(themName)){
            themNamePath=p+"/"+themName;
            break;
        }
    }
    qDebug()<<themNamePath;
    return themNamePath;

}

//______________________________________________________________________________
void EIconEngin::virtual_hook(int id, void *data)
{
ensureLoaded();
    switch (id) {
    case QIconEngineV2::AvailableSizesHook:
        {
            QIconEngineV2::AvailableSizesArgument &arg
                    = *reinterpret_cast<QIconEngineV2::AvailableSizesArgument*>(data);
//            const QList<QIconDirInfo> directoryKey = iconLoaderInstance()->theme().keyList();
            arg.sizes.clear();

            // Gets all sizes from the DirectoryInfo entries

                arg.sizes=m_icon.availableSizes();

        }
        break;
#if QT_VERSION >= 0x040700
    case QIconEngineV2::IconNameHook:
        {
            QString &name = *reinterpret_cast<QString*>(data);
            name = m_iconName;
        }
        break;
#else
#warning QIconEngineV2::IconNameHook is ignored due Qt version. Upgrade to 4.7.x
#endif
    default:
        QIconEngineV2::virtual_hook(id, data);
    }
}
