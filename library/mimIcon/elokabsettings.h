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

#ifndef ELOKABSETTINGS_H
#define ELOKABSETTINGS_H

#include <QSettings>
#include <QFileSystemWatcher>
class ElokabSettings : public QSettings
{
    Q_OBJECT

public:
    explicit ElokabSettings(QObject *parent = nullptr);
static const ElokabSettings *ESettings();

signals:
    /// Signal emitted when the icon theme has changed.
    void iconThemeChanged();
    /// Signal emitted when the widget theme has changed.
    void widgetStyleChanged();
    /// Signal emitted when the themes has changed.
    void ThemesChanged();
    /// Signal emitted when the shortcuts has changed.
    void shortcutsKeysChanged();
    /// Signal emitted when the docksetting has changed.
    void dockThemesChanged();
    /// Signal emitted when the panelsetting has changed.
    void panelThemesChanged();
    /// Signal emitted when the desktopsetting has changed.
    void desktopThemeChanged();
    /// Signal emitted when the keysetting has changed.
    void keyMapChanged();


public slots:
    QString loadIconThems();
    QString loadStyleWidget();
private:
    QFileSystemWatcher *m_fileWatcher;
    QString m_iconTheme;
    QString m_styleWidget;
    QString m_Themes;
    QString m_DockThemes;
    QString m_PanelThemes;
    QString m_DesktopThemes;
    QString m_KeyMap;
    QString m_Shortcut;
private slots:
    void fileChanged();
    QString loadtheme();
    QString loadDocktheme();
    QString loadPaneltheme();
    QString loadDesktoptheme();
    QString loadKeyMap();
    QString loadShortcut();
};

#endif // ELOKABSETTINGS_H


