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

#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H
#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

#include <QFileInfo>


class ItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:

    //!
    explicit ItemDelegate();

    //!
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    //!
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;



signals:

    //!
   // void imageHasThumb(const QString &path)const;

public slots:

    //!
    void setTreeView(bool arg){isTreeview=arg;}

    //!
    void setTumbnail(bool thumb){mThumbnail=thumb;}

    //!
    void clearCurentPath(const QString &path);

    //!
    void clearItemCache(const QString &file);

private:
    //!
    void paintDetailView(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    //!
    void paintIconView(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QIcon mSymlinkIcon;
    QSize mItemSize;
    bool  mThumbnail;

    QMap<QString ,QIcon>  *imageCache ;
    QMap<QString ,QIcon>  *iconCache ;
    QMap<QString ,QIcon>  *folderCache;
    QMap<QString ,QIcon>  *deskCache;

    QStringList *listThumb;
    QString thumbnail;
bool isTreeview;
    //!
    static QIcon::Mode iconModeFromState(QStyle::State state);

    //!
    QIcon decoration(const QModelIndex &index) const;

    //!
    void saveImageThumb(const QFileInfo &fi,const QString &fileThumbnail)const;

    //!
    QIcon iconThumbnails(const QString &file)const;

};

#endif // ITEMDELEGATE_H
