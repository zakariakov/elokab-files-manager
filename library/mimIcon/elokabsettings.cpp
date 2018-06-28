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

#include "elokabsettings.h"
//#include "eicon.h"
#include<QIcon>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QMutex>
ElokabSettings::ElokabSettings(QObject *parent) :
    QSettings("elokab","elokabsettings", parent)

{

    QStringList listThemePath=QIcon::themeSearchPaths();
    if(!listThemePath.contains(QDir::homePath()+"/.icons"))
    listThemePath.append(QDir::homePath()+"/.icons");
    if(!listThemePath.contains("/usr/share/icons"))
    listThemePath.append("/usr/share/icons");
//    if(!listThemePath.contains(QDir::homePath()+"/.local/share/icons"))
//    listThemePath.append(QDir::homePath()+"/.local/share/icons");
    listThemePath.removeDuplicates();

    QIcon::setThemeSearchPaths(listThemePath);

    m_fileWatcher=new QFileSystemWatcher;
    m_fileWatcher->addPath(this->fileName());
    connect(m_fileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged()));
    fileChanged();

}

const ElokabSettings *ElokabSettings::ESettings()
{
    static QMutex mutex;
    static ElokabSettings *instance = 0;
    if (!instance)
    {
        mutex.lock();

        if (!instance)
            instance = new ElokabSettings();

        mutex.unlock();
    }

    return instance;
}

void ElokabSettings::fileChanged()
{
    sync();
    QString iconThem=loadIconThems();
    if(iconThem!=m_iconTheme){
        m_iconTheme=iconThem;
        QIcon::setThemeName(m_iconTheme);
       // EIcon::instance()->seTemeName(m_iconTheme);
       emit iconThemeChanged();

    }

    QString styleTheme=loadStyleWidget();
    if(m_styleWidget!=styleTheme){
        m_styleWidget=styleTheme;
        emit widgetStyleChanged();
    }

    QString Theme=loadtheme();
    if(m_Themes!=Theme){
        m_Themes=Theme;
        emit ThemesChanged();

    }

    QString DockTheme=loadDocktheme();
    if(m_DockThemes!=DockTheme){
        m_DockThemes=DockTheme;
        emit dockThemesChanged();

    }

    QString panelTheme=loadPaneltheme();
    if(m_PanelThemes!=panelTheme){
        m_PanelThemes=panelTheme;
        emit panelThemesChanged();

    }

    QString desktopTheme=loadDesktoptheme();
    if(m_DesktopThemes!=desktopTheme){
        m_DesktopThemes=desktopTheme;
        emit desktopThemeChanged();

    }


    QString keyMap=loadKeyMap();
    if(m_KeyMap!=keyMap){
        m_KeyMap=keyMap;
        emit keyMapChanged();

    }

    QString shortcut=loadShortcut();
    if(m_Shortcut!=shortcut){
        m_Shortcut=shortcut;
         emit shortcutsKeysChanged();

    }


}


QString ElokabSettings::loadIconThems()
{
sync();
    this->beginGroup("Themes");
    QString icnThem=  this->value("iconsTheme",QIcon::themeName()).toString();
     this->endGroup();
    if(icnThem=="hicolor"||icnThem.isEmpty()){

        QStringList failback;
        failback << "Elokab"<< "oxygen"<< "Mint-X"<< "Humanity"<< "Tango"<< "Prudence-icon"<< "elementary"<< "gnome";

        QDir dir("/usr/share/icons/");
        foreach (QString s, failback)
        {
            if (dir.exists(s))
            {
                icnThem=s;

                break;
            }
        }
    }
    if(icnThem.isEmpty())
            icnThem="hicolor";


    return icnThem;
}

QString ElokabSettings::loadStyleWidget()
{
  this->beginGroup("Style");
    QString style=this->value("Name",trUtf8("default")).toString();
  this->endGroup();
    return style;
}

QString ElokabSettings::loadtheme()
{
    sync();
  this->beginGroup("Themes");
    QString Theme=this->value("ThemeName",trUtf8("default")).toString();
  this->endGroup();

    return Theme;
}
QString ElokabSettings::loadDocktheme()
{
    sync();
  this->beginGroup("AppChangeSetting");
    QString Theme=this->value("DockTheme").toString();
  this->endGroup();

    return Theme;
}
QString ElokabSettings::loadPaneltheme()
{
    sync();
  this->beginGroup("AppChangeSetting");
    QString Theme=this->value("PanelTheme").toString();
  this->endGroup();

    return Theme;
}

QString ElokabSettings::loadDesktoptheme()
{
    sync();
  this->beginGroup("AppChangeSetting");
    QString Theme=this->value("DesktopTheme").toString();
  this->endGroup();

    return Theme;
}

QString ElokabSettings::loadKeyMap()
{
    sync();
  this->beginGroup("KeyMap");
   QString arg=this->value("KeyChanged").toString();
  this->endGroup();

    return arg;
}
QString ElokabSettings::loadShortcut()
{
    sync();
  this->beginGroup("Shortcut");
   QString arg=this->value("ShortcutChanged").toString();
  this->endGroup();

    return arg;
}
