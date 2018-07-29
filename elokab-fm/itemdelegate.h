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
    explicit ItemDelegate(bool modern);

    //!
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    //!
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;



signals:

      //! اشارة بان الملف بحاجة الى مصغرة
    void requireThumb(QFileInfo,QString)const;

public slots:

    //!  هل عرض التفصيل
    void setTreeView(bool arg)      {isTreeview=arg;}

    //!  تفعيل/تعطيل الايقونات الكلاسيكية
    void setClassicIcons(bool arg)  {isModernMode=!arg;}

    //!  تفعيل /تعطيل المصغرات
    void setTumbnail(bool thumb)    {mThumbnail=thumb;}

    //!  تفعيل/تعطيل مصغرات الكتب pdf
    void setPdfTumbnail(bool thumb) {mPdfThumbnail=thumb;}

    //! تفعيل/تعطيل مصعرات الفيديو
    void setVideoTumbnail(bool thumb){mVideoThumbnail=thumb;}

    //! مسح معلومات المسار الحالي
    void clearCurentPath(const QString &path);

    //!  مسح معلومات املف الحالي
    void clearItemCache(const QString &file);

private slots:
    //!  رسم الايقونة عند الوضع التفصيلي والقياسي
    void paintDetailView(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    //!  رسم الايقونة في حالة وضع الايقونات
    void paintIconView(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    //!  ...جلب حالة الايقونة بين مفعلة وعادية و
    static QIcon::Mode iconModeFromState(QStyle::State state);

    //!   جلب الايقونة
    QIcon decoration(const QModelIndex &index) const;

    //!   جلب ايقونة المصغرات
    QIcon iconThumbnails(const QString &file,const QString &type=QString())const;

private:

    QMap<QString ,QIcon>  *imageCache ;
    QMap<QString ,QIcon>  *iconCache ;
    QMap<QString ,QIcon>  *deskCache;


    QString thumbnailCache;

    QIcon mSymlinkIcon;

    bool  mThumbnail;
    bool  mPdfThumbnail;
    bool  mVideoThumbnail;
    bool  isTreeview;
    bool  isModernMode;



};

#endif // ITEMDELEGATE_H
