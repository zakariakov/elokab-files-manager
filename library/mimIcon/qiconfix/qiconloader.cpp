/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2
 */
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
//END_COMMON_COPYRIGHT_HEADER

#ifndef QT_NO_ICON
#include "qiconloader_p.h"

//#include "qt/qapplication_p.h"
//#include <qt/qicon_p.h>
//#include <qt/qguiplatformplugin_p.h>
#include <QDebug>
//#include <QtGui/QIconEnginePlugin>
#include <QtGui/QPixmapCache>
//#include <QtGui/QIconEngine>
//#include <QtGui/QStyleOption>
//#include <QtCore/QList>
//#include <QtCore/QHash>
//#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtGui/QPainter>
//#include <QtGui/QApplication>
//#include <QtCore/QLatin1Literal>
#include <QDirIterator>
//#ifdef Q_WS_MAC
//#include <private/qt_cocoa_helpers_mac_p.h>
//#endif

//#ifdef Q_WS_X11
//#include "qt/qt_x11_p.h"
//#endif


#if QT_VERSION < 0x040700
#include <limits.h>
#endif

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QIconLoader, iconLoaderInstance)

/* Theme to use in last resort, if the theme does not have the icon, neither the parents  */
static QString fallbackTheme()
{
#ifdef Q_WS_X11
//    if (X11->desktopEnvironment == DE_GNOME) {
//        return QLatin1String("gnome");
//    } else if (X11->desktopEnvironment == DE_KDE) {
//        return X11->desktopVersion >= 4
//            ? QString::fromLatin1("oxygen")
//            : QString::fromLatin1("crystalsvg");
//    } else {
//        return QLatin1String("hicolor");
//    }
#endif
     return QLatin1String("ELokab");
}

QIconLoader::QIconLoader() :
        m_themeKey(1), m_supportsSvg(false), m_initialized(false)
{
}

// We lazily initialize the loader to make static icons
// work. Though we do not officially support this.
void QIconLoader::ensureInitialized()
{
    if (!m_initialized) {
        m_initialized = true;

//        Q_ASSERT(qApp);

        m_systemTheme = QIcon::themeName();

#ifndef QT_NO_LIBRARY
//        QFactoryLoader iconFactoryLoader(QIconEngineFactoryInterfaceV2_iid,
//                                         QLatin1String("/iconengines"),
//                                         Qt::CaseInsensitive);
//        if (iconFactoryLoader.keys().contains(QLatin1String("svg")))
            m_supportsSvg = true;
#endif //QT_NO_LIBRARY
    }
}

QIconLoader *QIconLoader::instance()
{
   return iconLoaderInstance();
}

// Queries the system theme and invalidates existing
// icons if the theme has changed.
void QIconLoader::updateSystemTheme()
{

    // Only change if this is not explicitly set by the user


        QString theme = QIcon::themeName();//qt_guiPlatformPlugin()->systemIconThemeName();
        if (theme.isEmpty())
            theme = fallbackTheme();
        if (theme != m_systemTheme) {
            m_systemTheme = theme;
            invalidateKey();
        }
//        qDebug()<<"m_systemTheme"<<m_systemTheme;
}

void QIconLoader::setThemeName(const QString &themeName)
{

    if (themeName.isEmpty())
        m_userTheme = fallbackTheme();
else
    m_userTheme = themeName;

    invalidateKey();
}

void QIconLoader::setThemeSearchPath(const QStringList &searchPaths)
{
    m_iconDirs = searchPaths;
    themeList.clear();
    invalidateKey();
}

QStringList QIconLoader::themeSearchPaths() const
{

    if (m_iconDirs.isEmpty())
    {
        m_iconDirs = QIcon::themeSearchPaths();//qt_guiPlatformPlugin()->iconThemeSearchPaths();
        // Always add resource directory as search path

        m_iconDirs.append(QLatin1String(":/icons"));
    }
    return m_iconDirs;
}


QIconTheme::QIconTheme(const QString &themeName)
        : m_valid(false)
{

    QFile themeIndex;

    QList <QIconDirInfo> keyList;
    QStringList iconDirs = QIcon::themeSearchPaths();
    for ( int i = 0 ; i < iconDirs.size() ; ++i) {
        QDir iconDir(iconDirs[i]);

        QString themeDir = iconDir.path() + QLatin1Char('/') + themeName;
//          qDebug()<<"QIconTheme"<<themeDir;
        themeIndex.setFileName(themeDir + QLatin1String("/index.theme"));

        if (themeIndex.exists()) {
           //  qDebug()<<"QIconTheme"<<themeIndex.fileName();
            m_contentDir = themeDir;
            m_valid = true;

            QStringList themeSearchPaths = QIcon::themeSearchPaths();

            foreach (QString path, themeSearchPaths)
            {
                if (!path.startsWith(':') && QFileInfo(path).isDir())
                    m_contentDirs.append(path + QLatin1Char('/') + themeName);

            }

            break;
        }
    }


#ifndef QT_NO_SETTINGS
    if (themeIndex.exists()) {
        const QSettings indexReader(themeIndex.fileName(), QSettings::IniFormat);
        QStringListIterator keyIterator(indexReader.allKeys());
        while (keyIterator.hasNext()) {

            const QString key = keyIterator.next();
            if (key.endsWith(QLatin1String("/Size"))) {
                // Note the QSettings ini-format does not accept
                // slashes in key names, hence we have to cheat
                if (int size = indexReader.value(key).toInt()) {
                    QString directoryKey = key.left(key.size() - 5);
                    QIconDirInfo dirInfo(directoryKey);
                    dirInfo.size = size;
                    QString type = indexReader.value(directoryKey +
                                                     QLatin1String("/Type")
                                                     ).toString();

                    if (type == QLatin1String("Fixed"))
                        dirInfo.type = QIconDirInfo::Fixed;
                    else if (type == QLatin1String("Scalable"))
                        dirInfo.type = QIconDirInfo::Scalable;
                    else
                        dirInfo.type = QIconDirInfo::Threshold;

                    dirInfo.threshold = indexReader.value(directoryKey +
                                                        QLatin1String("/Threshold"),
                                                        2).toInt();

                    dirInfo.minSize = indexReader.value(directoryKey +
                                                         QLatin1String("/MinSize"),
                                                         size).toInt();

                    dirInfo.maxSize = indexReader.value(directoryKey +
                                                        QLatin1String("/MaxSize"),
                                                        size).toInt();
                    m_keyList.append(dirInfo);
                }
            }
        }

        // Parent themes provide fallbacks for missing icons
        m_parents = indexReader.value(
                QLatin1String("Icon Theme/Inherits")).toStringList();

        // Ensure a default platform fallback for all themes
        if (m_parents.isEmpty())
            m_parents.append(QIcon::themeName());//fallbackTheme());

        // Ensure that all themes fall back to hicolor
        if (!m_parents.contains(QLatin1String("hicolor")))
            m_parents.append(QLatin1String("hicolor"));

        // Ensure that all themes fall back to Elokab
           if (!m_parents.contains(fallbackTheme()))
               m_parents.append(fallbackTheme());

    }
#endif //QT_NO_SETTINGS
}



QThemeIconEntries QIconLoader::findIconHelper(const QString &themeName,
                                 const QString &iconName,
                                 QStringList &visited) const
{
    QThemeIconEntries m_entries;
    Q_ASSERT(!themeName.isEmpty());

    // Used to protect against potential recursions
    visited << themeName;

    QIconTheme theme = themeList.value(themeName);
    if (!theme.isValid()) {
        theme = QIconTheme(themeName);
        if (!theme.isValid())
       theme = QIconTheme(QIcon::themeName());//fallbackTheme());
        themeList.insert(themeName, theme);
    }

    QStringList contentDirs = theme.contentDirs();

    QList<QIconDirInfo> subDirs = theme.keyList();

    const QString svgext(QLatin1String(".svg"));
    const QString pngext(QLatin1String(".png"));
    const QString xpmext(QLatin1String(".xpm"));
    const QString svgzext(QLatin1String(".svgz"));

    // Add all relevant files
    for (int i = 0; i < subDirs.size() ; ++i)
    {
        const QIconDirInfo &dirInfo = subDirs.at(i);

        QString subdir = dirInfo.path;

        foreach (QString contentDir, contentDirs)
        {
            // qDebug()<<contentDir + '/' + subdir<< themeName;
            QDir currentDir(contentDir + '/' + subdir);


            if (currentDir.exists(iconName + pngext))
            {


         if(!QPixmap(currentDir.filePath(iconName + pngext) ).size().isEmpty()){
                PixmapEntry *iconEntry = new PixmapEntry;
                iconEntry->dir = dirInfo;
                iconEntry->filename = currentDir.filePath(iconName + pngext);
                // Notice we ensure that pixmap entries always come before
                // scalable to preserve search order afterwards
                 m_entries.prepend(iconEntry);
                break;
             }
            }
            else if (m_supportsSvg &&
                     currentDir.exists(iconName + svgext))
            {
                if(!QPixmap(currentDir.filePath(iconName + svgext)).size().isEmpty()){
                    ScalableEntry *iconEntry = new ScalableEntry;

                    iconEntry->dir = dirInfo;
                    iconEntry->filename = currentDir.filePath(iconName + svgext);
                    // qDebug()<<"isempty"<<iconEntry->svgIcon.availableSizes().isEmpty();
                    m_entries.append(iconEntry);
                    break;

                }

            }
            else if (m_supportsSvg &&
                     currentDir.exists(iconName + svgzext))
            {
                ScalableEntry *iconEntry = new ScalableEntry;

                iconEntry->dir = dirInfo;
                iconEntry->filename = currentDir.filePath(iconName + svgzext);
                m_entries.append(iconEntry);
                break;

            }
            else if (currentDir.exists(iconName + xpmext))
            {
                PixmapEntry *iconEntry = new PixmapEntry;
                iconEntry->dir = dirInfo;
                iconEntry->filename = currentDir.filePath(iconName + xpmext);
                // Notice we ensure that pixmap entries always come before
                // scalable to preserve search order afterwards
                m_entries.append(iconEntry);
                break;
            }

        }
    }

       //    look in Icon Theme/Inherits
    if (m_entries.isEmpty()) {
        const QStringList parents = theme.parents();

        // Search recursively through inherited themes
        for (int i = 0 ; i < parents.size() ; ++i) {

            const QString parentTheme = parents.at(i).trimmed();

            if (!visited.contains(parentTheme)) {

                m_entries = findIconHelper(parentTheme, iconName, visited);

            }
            if (!m_entries.isEmpty()) // success
                break;
        }
    }


    /*********************************************************************
    Author: Kaitlin Rupert <kaitlin.rupert@intel.com>
    Date: Aug 12, 2010
    Description: Make it so that the QIcon loader honors /usr/share/pixmaps
                 directory.  This is a valid directory per the Freedesktop.org
                 icon theme specification.
    Bug: https://bugreports.qt.nokia.com/browse/QTBUG-12874
     *********************************************************************/

#ifdef Q_OS_LINUX
    //    Freedesktop standard says to look in /usr/share/pixmaps last

    if (m_entries.isEmpty()) {
        QString currentDir(QLatin1String("/usr/share/pixmaps"));
         m_entries=entries(currentDir,iconName,m_entries);
         if(m_entries.isEmpty()){
             QDirIterator it(currentDir
                             ,QDir::AllDirs |QDir::NoSymLinks | QDir::NoDotAndDotDot
                             , QDirIterator::Subdirectories);
             while(it.hasNext())
             {
                 it.next();
                 const QString pixmaps(it.filePath());
                 m_entries=entries(pixmaps,iconName,m_entries);
                 if(!m_entries.isEmpty())
                     break;

             }
         }

    }

    //    Freedesktop standard says to look in $HOME/.local/share/icons last
    if (m_entries.isEmpty()) {
        QDirIterator it( QDir::homePath()+QLatin1String("/.local/share/icons")
                         ,QDir::AllDirs |QDir::NoSymLinks | QDir::NoDotAndDotDot
                         , QDirIterator::Subdirectories);
        while(it.hasNext())
        {
            it.next();
            const QString pixmaps(it.filePath());
            m_entries=entries(pixmaps,iconName,m_entries);
            if(!m_entries.isEmpty())
                break;
        }
    }

#endif

    return m_entries;
}

QThemeIconEntries QIconLoader::entries(const QString &path,
                              const QString &iconName
                              ,QThemeIconEntries m_entries) const

{

    QDir currentDir(path);

    QStringList list;
    list<<iconName + ".svg"
        <<iconName + ".svgz"
        <<iconName + ".png"
        <<iconName + ".xpm";
      QStringList  files = currentDir.entryList(list,QDir::Files | QDir::NoDotAndDotDot);
      foreach (QString s, files) {
//          qDebug()<<currentDir.filePath(s);;
          if(m_supportsSvg && (s.endsWith(".svg")||s.endsWith(".svgz")))
          {
//          qDebug()<<     QPixmap(currentDir.filePath(s) ).size().isEmpty();
              ScalableEntry *iconEntry = new ScalableEntry;
//              if(!iconEntry->svgIcon.isNull())
//              {
                  iconEntry->dir = path;
                  iconEntry->filename = currentDir.filePath(s);
                  m_entries.append(iconEntry);

//              }else delete iconEntry;


          }else if(s.endsWith(".png")||s.endsWith(".xpm")){
              PixmapEntry *iconEntry = new PixmapEntry;
              // if(!iconEntry->basePixmap.isNull()){
              iconEntry->dir = path;
              iconEntry->filename = currentDir.filePath(s);;
              m_entries.prepend(iconEntry);
              //              }else
              //                   delete iconEntry;

          }

      }

   /*
    const QString svgext(QLatin1String(".svg"));
    const QString pngext(QLatin1String(".png"));
    const QString xpmext(QLatin1String(".xpm"));
    const QString svgzext(QLatin1String(".svgz"));

        if (currentDir.exists(iconName + pngext))
    {
      //  qDebug()<<path+"/"+iconName + pngext;
        PixmapEntry *iconEntry = new PixmapEntry;
        iconEntry->dir = path;
        iconEntry->filename = currentDir.filePath(iconName + pngext);

        // Notice we ensure that pixmap entries always come before
        // scalable to preserve search order afterwards
      m_entries.prepend(iconEntry);

    }
    else if (m_supportsSvg &&
             currentDir.exists(iconName + svgext))
    {
            ScalableEntry *iconEntry = new ScalableEntry;
            if(!iconEntry->svgIcon.isNull()){
            iconEntry->dir = path;
            iconEntry->filename = currentDir.filePath(iconName + svgext);
           // qDebug()<<"isempty"<<iconEntry->svgIcon.availableSizes().isEmpty();
            m_entries.append(iconEntry);

            }else{
                delete iconEntry;
            }


    }
    else if (m_supportsSvg &&
             currentDir.exists(iconName + svgzext))
    {
        ScalableEntry *iconEntry = new ScalableEntry;

        iconEntry->dir = path;
        iconEntry->filename = currentDir.filePath(iconName + svgzext);
        m_entries.append(iconEntry);

    }
    else if (currentDir.exists(iconName + xpmext))
    {
        PixmapEntry *iconEntry = new PixmapEntry;
        iconEntry->dir = path;
        iconEntry->filename = currentDir.filePath(iconName + xpmext);
        // Notice we ensure that pixmap entries always come before
        // scalable to preserve search order afterwards
        m_entries.append(iconEntry);

    }
      */
    return m_entries;
}


QThemeIconEntries QIconLoader::loadIcon(const QString &themeName,const QString &name) const
{
  //   qDebug()<<"QThemeIconEntries QIconLoader::loadIcon"<<themeName()<<name;

//     if(themeName.isEmpty())
//         themeName=QIcon::themeName();

     if (!themeName.isEmpty()) {
        QStringList visited;
        return findIconHelper(themeName, name, visited);
    }

    return QThemeIconEntries();
}


// -------- Icon Loader Engine -------- //


QIconLoaderEngineFixed::QIconLoaderEngineFixed(const QString& iconName,
                                               const QString &fillBack)
    : m_iconName(iconName),m_iconFallBack(fillBack), m_key(0)
{
}

QIconLoaderEngineFixed::~QIconLoaderEngineFixed()
{
    while (!m_entries.isEmpty())
        delete m_entries.takeLast();
    Q_ASSERT(m_entries.size() == 0);
}

QIconLoaderEngineFixed::QIconLoaderEngineFixed(const QIconLoaderEngineFixed &other)
        : QIconEngine(other),
        m_iconName(other.m_iconName),
        m_key(0)
{
}

QIconEngine *QIconLoaderEngineFixed::clone() const
{
    return new QIconLoaderEngineFixed(*this);
}

bool QIconLoaderEngineFixed::read(QDataStream &in) {
    in >> m_iconName;
    return true;
}

bool QIconLoaderEngineFixed::write(QDataStream &out) const
{
    out << m_iconName;
    return true;
}

bool QIconLoaderEngineFixed::hasIcon() const
{

    return !(m_entries.isEmpty());
}

// Lazily load the icon
void QIconLoaderEngineFixed::ensureLoaded()
{

    iconLoaderInstance()->ensureInitialized();

 if (!(iconLoaderInstance()->themeKey() == m_key)) {

        while (!m_entries.isEmpty())
            delete m_entries.takeLast();

        Q_ASSERT(m_entries.size() == 0);


        QString themeName=iconLoaderInstance()->themeName();
        QThemeIconEntries entries=iconLoaderInstance()->loadIcon(themeName,m_iconName);

        if(!entries.isEmpty()){

            m_entries = entries;
            m_key = iconLoaderInstance()->themeKey();
            m_themName=themeName;
        //    qDebug()<<"from theme="<<m_themName<<m_iconName;
            return;
        }
        /****************************************************
         * اذا لم يجد اليقونة سيتم الابقاء على الايقونة السابقة
         * بتفس اسم السمة حتى اذا تغيرت السمة تبقى بنفس الاسم
         ****************************************************/
         QString theme=m_themName;

         entries=iconLoaderInstance()->loadIcon(theme,m_iconName);
         if(!entries.isEmpty()){
             m_themName=theme;
             m_entries = entries;
             m_key= iconLoaderInstance()->themeKey();
          //   qDebug()<<"frome Old theme="<<m_themName<<m_iconName;
             return;
         }
         entries=iconLoaderInstance()->loadIcon(themeName,m_iconFallBack);
         if(!entries.isEmpty()){
             m_entries = entries;
             m_key = iconLoaderInstance()->themeKey();
             m_themName=themeName;
       //      qDebug()<<"from theme="<<m_themName<<m_iconName;
             return;
         }
   }

}

void QIconLoaderEngineFixed::paint(QPainter *painter, const QRect &rect,
                             QIcon::Mode mode, QIcon::State state)
{
    QSize pixmapSize = rect.size();
#if defined(Q_WS_MAC)
    pixmapSize *= qt_mac_get_scalefactor();
#endif
    painter->drawPixmap(rect, pixmap(pixmapSize, mode, state));
}

/*
 * This algorithm is defined by the freedesktop spec:
 * http://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
 */
static bool directoryMatchesSize(const QIconDirInfo &dir, int iconsize)
{
//      /*qDebug*/()<<"directoryMatchesSize"<<iconsize;

    if (dir.type == QIconDirInfo::Fixed) {
        return dir.size == iconsize;

    } else if (dir.type == QIconDirInfo::Scalable) {
        return dir.size <= dir.maxSize &&
                iconsize >= dir.minSize;

    } else if (dir.type == QIconDirInfo::Threshold) {
        return iconsize >= dir.size - dir.threshold &&
                iconsize <= dir.size + dir.threshold;
    }

    Q_ASSERT(1); // Not a valid value
    return false;
}

/*
 * This algorithm is defined by the freedesktop spec:
 * http://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
 */
static int directorySizeDistance(const QIconDirInfo &dir, int iconsize)
{
    // qDebug()<<"directorySizeDistance"<<iconsize<<dir.path;
    if (dir.type == QIconDirInfo::Fixed) {
        return qAbs(dir.size - iconsize);

    } else if (dir.type == QIconDirInfo::Scalable) {
        if (iconsize < dir.minSize)
            return dir.minSize - iconsize;
        else if (iconsize > dir.maxSize)
            return iconsize - dir.maxSize;
        else
            return 0;

    } else if (dir.type == QIconDirInfo::Threshold) {
        if (iconsize < dir.size - dir.threshold)
            return dir.minSize - iconsize;
        else if (iconsize > dir.size + dir.threshold)
            return iconsize - dir.maxSize;
        else return 0;
    }

    Q_ASSERT(1); // Not a valid value
    return INT_MAX;
}

QIconLoaderEngineEntry *QIconLoaderEngineFixed::entryForSize(const QSize &size)
{

    int iconsize = qMin(size.width(), size.height());

    // Note that m_entries are sorted so that png-files
    // come first

    // Search for exact matches first
    for (int i = 0; i < m_entries.count(); ++i) {
        QIconLoaderEngineEntry *entry = m_entries.at(i);
        if (directoryMatchesSize(entry->dir, iconsize)) {
            return entry;
        }
    }

    // Find the minimum distance icon
    int minimalSize = INT_MAX;
    QIconLoaderEngineEntry *closestMatch = 0;
    for (int i = 0; i < m_entries.count(); ++i) {
        QIconLoaderEngineEntry *entry = m_entries.at(i);
        int distance = directorySizeDistance(entry->dir, iconsize);
          // qDebug()<<":entryForSize"<<iconsize<<m_iconName<<distance;
        if (distance < minimalSize) {
            minimalSize  = distance;
            closestMatch = entry;
        }
    }
    return closestMatch;
}

/*
 * Returns the actual icon size. For scalable svg's this is equivalent
 * to the requested size. Otherwise the closest match is returned but
 * we can never return a bigger size than the requested size.
 *
 */
QSize QIconLoaderEngineFixed::actualSize(const QSize &size, QIcon::Mode mode,
                                   QIcon::State state)
{
//TODO:fix  ensureLoaded();
   ensureLoaded();

    QIconLoaderEngineEntry *entry = entryForSize(size);
    if (entry) {
        const QIconDirInfo &dir = entry->dir;
        if (dir.type == QIconDirInfo::Scalable)
        {
            return size;
        }
        else {
            if (dir.size == 0)
            {
                entry->dir.size = QPixmap(entry->filename).size().width();
                entry->dir.minSize = dir.size;
                entry->dir.maxSize = dir.size;
            }
//            int result = qMin<int>(dir.size, qMin(size.width(), size.height()));
//            return QSize(result, result);
        }
    }
    return QIconEngine::actualSize(size, mode, state);
}

QPixmap PixmapEntry::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(state);
Q_UNUSED(mode);
    // Ensure that basePixmap is lazily initialized before generating the
    // key, otherwise the cache key is not unique

    if (basePixmap.isNull())
        basePixmap.load(filename);

/*

    QString key = QString("$qt_theme_%1%2%3%4%5")
                    .arg(basePixmap.cacheKey(),     16, 16, QChar('0'))
                    .arg(mode,                      8,  16, QChar('0'))
                    .arg(qApp->palette().cacheKey(),16, 16, QChar('0'))
                    .arg(size.width(),        8,  16, QChar('0'))
                    .arg(size.height(),       8,  16, QChar('0'));

    QPixmap cachedPixmap;
    if (QPixmapCache::find(key, &cachedPixmap)) {
//        if(cachedPixmap.size()!=size)
//             cachedPixmap = cachedPixmap.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//        qDebug()<<filename<<" cachedPixmap "<<"SIZE:"<<size <<"ORIGE:" <<basePixmap.size()
//                <<"ACTUAL:"<<cachedPixmap.size()  ;
        return cachedPixmap;
    } else {
       cachedPixmap=basePixmap;
        if(cachedPixmap.size()!=size)
        cachedPixmap = cachedPixmap.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QStyleOption opt(0);
        opt.palette = qApp->palette();
        cachedPixmap = qApp->style()->generatedIconPixmap(mode, cachedPixmap, &opt);
        QPixmapCache::insert(key, cachedPixmap);


//        qDebug()<<filename<<"SIZE:"<<size <<"ORIGE:" <<basePixmap.size()
//                <<"ACTUAL:"<<cachedPixmap.size()  ;

    }
    */

     if(basePixmap.size()!=size)
     basePixmap = basePixmap.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
 return basePixmap;
  //  return cachedPixmap;
}


QPixmap ScalableEntry::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{

    if (svgIcon.isNull())
        svgIcon = QIcon(filename);

    // Simply reuse svg icon engine
    return svgIcon.pixmap(size, mode, state);
}


QPixmap QIconLoaderEngineFixed::pixmap(const QSize &size, QIcon::Mode mode,
                                 QIcon::State state)
{
//TODO:fix  ensureLoaded();
   ensureLoaded();

     QIconLoaderEngineEntry *entry = entryForSize(size);


    if (entry){

        return entry->pixmap(size, mode, state);
}
    return QPixmap();
}

QString QIconLoaderEngineFixed::key() const
{
    return QLatin1String("QIconLoaderEngineFixed");


}

void QIconLoaderEngineFixed::virtual_hook(int id, void *data)
{
   ensureLoaded();
qDebug()<<" ensureLoaded() virtual_hook==================="<<m_iconName;
    switch (id) {
    case QIconEngine::AvailableSizesHook:
        {
            QIconEngine::AvailableSizesArgument &arg
                    = *reinterpret_cast<QIconEngine::AvailableSizesArgument*>(data);
            const QList<QIconDirInfo> directoryKey = iconLoaderInstance()->theme().keyList();
            arg.sizes.clear();

            // Gets all sizes from the DirectoryInfo entries
            for (int i = 0 ; i < m_entries.size() ; ++i) {
                int size = m_entries.at(i)->dir.size;
                arg.sizes.append(QSize(size, size));
            }
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

QT_END_NAMESPACE

#endif //QT_NO_ICON
