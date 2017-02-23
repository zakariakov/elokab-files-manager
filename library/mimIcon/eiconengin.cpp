#include "eiconengin.h"
#include <QPainter>
#include <QDebug>
#include <QDirIterator>
#include <QSettings>
QString themePath(const QString &path)
{
    foreach (QString p, QIcon::themeSearchPaths())
    {
        QDir dirS;
        if(dirS.exists(p+"/"+path))
            return  p+"/"+path;

    }
    return path;
}

QStringList parents(const QString &path)
{
    QFile  themeIndex(themePath(path) + QLatin1String("/index.theme"));
    QStringList listParents;
#ifndef QT_NO_SETTINGS

    if (themeIndex.exists()) {
        const QSettings indexReader(themeIndex.fileName(), QSettings::IniFormat);
         // Parent themes provide fallbacks for missing icons
        listParents = indexReader.value(
                QLatin1String("Icon Theme/Inherits")).toStringList();
    }

#endif //QT_NO_SETTINGS

    // Ensure that all themes fall back to hicolor
    if (!listParents.contains(QLatin1String("hicolor")))
        listParents.append(QLatin1String("hicolor"));


    return listParents;
}



EIconEngin::EIconEngin(const QString &iconName,const QString& themName):
    m_iconName(iconName),isfallback(true)
{
    if(themName.isEmpty()){
        m_themName=QIcon::themeName();
          m_icon=QIcon::fromTheme(m_iconName);
       //   m_availableSizes=m_icon.availableSizes();
    }else{

        m_themName=themName;
        iconFromThemePath(m_themName);

    }

}

void EIconEngin::paint(QPainter *painter, const QRect &rect,
                             QIcon::Mode mode, QIcon::State state)
{
    QSize size=m_icon.actualSize(rect.size());
    painter->drawPixmap(rect, pixmap(size,mode,state));
}

QPixmap EIconEngin::pixmap(const QSize &size, QIcon::Mode mode,
                                 QIcon::State state)
{
    ensureLoaded();
    QPixmap pix= m_icon.pixmap(size,mode,state);

    //TODO: fix this icon
     if(pix.isNull())
         pix=QIcon::fromTheme("unknown",QIcon(":/icons/unknown")).pixmap(size,mode,state);

    if(size!=pix.size())
        pix=pix.scaled(size);

    return pix;

}

void EIconEngin::ensureLoaded()
{
   /*****************************************************
    *اذا كانت السمة موجودة سيتم استخدام السمة
    **اما اذا لم تكن هناك سمة سيتم لستخدام الاحتياطية
    *اذا تغيرت السمة سينظر اذا وجدت سيتم انشاء احتياطية
    *واستخدام الحالية وهكذا
    ****************************************************/

    if(!QIcon::hasThemeIcon(m_iconName))
    {
        if(!isfallback)
        {
            //  qDebug()<<"!isfallback"<<m_themName;
           //   m_availableSizes.clear();
              m_icon=QIcon();
            iconFromThemePath((m_themName));
            if(m_icon.availableSizes().isEmpty()){
                foreach (QString s, parents(m_themName)) {
                    iconFromThemePath(s);
                    if(!m_icon.availableSizes().isEmpty())
                        break;
                }
            }


            isfallback=true;
        }
    }else
    {
        if(isfallback)
        {
                m_icon=QIcon::fromTheme(m_iconName);
               // m_availableSizes=m_icon.availableSizes();
                m_themName=QIcon::themeName();
                isfallback=false;

       }

    }

}

void EIconEngin::iconFromThemePath(const QString &theme)
{

   QString path=themePath(theme);

    addIconFile(path);

    QDirIterator it(path,QDir::AllDirs |
                     QDir::NoDotAndDotDot , QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        it.next();
        addIconFile(it.filePath());
        //        qDebug()<<curPath;

    }

}
void EIconEngin::addIconFile(const QString &curPath)
{
    QStringList listStr;
    listStr<<m_iconName+".png" <<m_iconName+".xpm"
          <<m_iconName+".svg"<<m_iconName+".svgz";

    QStringList files = QDir(curPath).entryList(QStringList(listStr),
                                                QDir::Files | QDir::NoDotAndDotDot);

    foreach (QString s, files) {
        QString f=  QDir(curPath).absoluteFilePath(s);
        QSize size=QPixmap(f).size();
        if(!size.isNull()){
         //  qDebug()<<f<<QPixmap(f).size();
           m_icon.addFile(f,size);
           //  m_icon.addPixmap(QPixmap(f));
           // m_availableSizes.append(size);
        }
    }

}
//-------------------------------------------------------------------

//--------------------------------------------------

void EIconEngin::virtual_hook(int id, void *data)
{
//ensureLoaded();
    switch (id) {
    case QIconEngine::AvailableSizesHook:
    {
        QIconEngine::AvailableSizesArgument &arg
                = *reinterpret_cast<QIconEngine::AvailableSizesArgument*>(data);
        //            const QList<QIconDirInfo> directoryKey = iconLoaderInstance()->theme().keyList();
        //  arg.sizes.clear();



        arg.sizes=m_icon.availableSizes();//m_availableSizes;

    }
        break;
#if QT_VERSION >= 0x040700
    case QIconEngine::IconNameHook:
    {
        QString &name = *reinterpret_cast<QString*>(data);
        name = m_iconName;
    }
        break;
#else
#warning QIconEngineV2::IconNameHook is ignored due Qt version. Upgrade to 4.7.x
#endif
    default:
        QIconEngine::virtual_hook(id, data);
    }
}
