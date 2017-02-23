#include "eicon.h"
#include <QFileInfo>
#include <QDirIterator>
#include <QDebug>
#include <QSettings>
#include <QCache>
#include <QtCore/QCoreApplication>
#include <QApplication>
//#include "qiconfix/qiconloader_p.h"
//#include "eiconengin.h"
QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(EIcon, EIconInstance)
void EIcon::seTemeName(const QString  &name)
{
    QIcon::setThemeName(name);
}

EIcon *EIcon::instance()
{
   return EIconInstance();
}
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
//______________________________________________________________________________________
QIcon EIcon::fromTheme(const QString  &iconName, const QString &fallback)
{


    QIcon iconF;


    ///------file Name--if exists
    if(QFile::exists(iconName)&& !iconName.isEmpty()){

        QString sufix=QFileInfo(iconName).suffix();
        if(sufix.toLower()=="png"
                ||sufix.toLower()=="svg"
                ||sufix.toLower()=="xpm"
                ||sufix.toLower()=="svgz"){
            iconF=QIcon(iconName);
            if(!iconF.isNull())
                return iconF;
        }
    }


    if(iconName.isEmpty())
        return QIcon::fromTheme("unknown");

    if(instance()->cachIcon.contains(iconName))
            return instance()->cachIcon.value(iconName);

    else if(QIcon::hasThemeIcon(iconName))
            iconF =  QIcon::fromTheme(iconName);


     else if(hasPathIcon(iconName,THEM_PIXMAP))
            iconF =iconFromThemePath(iconName,THEM_PIXMAP);


    else if(!fallback.isEmpty() && QIcon::hasThemeIcon(fallback))
        iconF =  QIcon::fromTheme(fallback);

    else if(hasPathIcon(iconName,THEM_BACK))
        iconF =  iconFromThemePath(iconName,themePath(THEM_BACK));


    if(!iconF.availableSizes().isEmpty()){
        instance()->cachIcon.insert(iconF.name(),iconF);
        return iconF;
    }

    iconF=QIcon(":/icons/"+iconName);
    if(!iconF.pixmap(128).isNull())  return iconF;

    iconF=QIcon(":/icons/"+fallback);
    if(!iconF.pixmap(128).isNull())   return iconF;

    return QIcon::fromTheme(APP_EXE);

  }
//-------------------------------------------------------------------
bool EIcon::hasPathIcon(const QString &name,const QString &path)
{

    QString mPath=path;

    foreach (QString p, QIcon::themeSearchPaths())
    {
        QDir dirS(p);
        if(dirS.exists(p+"/"+path))
        {
            mPath=  p+"/"+path;
            break;
        }

    }

    return searchIcon(name,mPath);

}

bool EIcon::searchIcon(const QString &name,const QString &path)
{
    QStringList files = QDir(path).entryList(QStringList(name+".*"),
                        QDir::Files | QDir::NoDotAndDotDot);

    if(files.count()>0)
        return true;

    QDirIterator it( path,QDir::AllDirs |QDir::NoDotAndDotDot ,
                     QDirIterator::Subdirectories);

    while(it.hasNext())
    {
       // qDebug()<<"it.filePath()"<<it.filePath();
        it.next();
        if(hasPathIcon(name,it.filePath()))
            return true;
    }

    return false;
}
//______________________________________________________________________________________

QIcon EIcon::iconFromThemePath(const QString &m_iconName,const QString &path)
{
QIcon m_icon ;

    addIconFile(m_icon,m_iconName,path);

    QDirIterator it(path,QDir::AllDirs |
                     QDir::NoDotAndDotDot , QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        it.next();
        addIconFile(m_icon,m_iconName,it.filePath());
        //        qDebug()<<curPath;

    }
return m_icon;
}
void EIcon::addIconFile(QIcon m_icon,const QString m_iconName,const QString &curPath)
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

QT_END_NAMESPACE
