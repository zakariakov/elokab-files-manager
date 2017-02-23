/* Copyright © 2006-2007 Fredrik Höglund <fredrik@kde.org>
 * (c)GPL2 (c)GPL3
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License version 2 or at your option version 3 as published 
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
/*
 * additional code: Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 */
#include "itemdelegate.h"
#include <QItemDelegate>

#include <QApplication>
#include <QModelIndex>
#include <QDebug>
#include <QPainter>



#include <QPaintDevice>
namespace {
    const int decorationMargin = 2;
}


///////////////////////////////////////////////////////////////////////////////
ItemDelegate::ItemDelegate(QObject *parent) : QItemDelegate(parent)
{
m_cache=new  QHash< QModelIndex, QPixmap >;
}

 void ItemDelegate::clear()
 {
  m_cache->clear();
 }

ItemDelegate::~ItemDelegate()
{
    m_cache->clear();
}

QString ItemDelegate::firstLine(const QModelIndex &index) const
{
    if (index.isValid()) return index.model()->data(index, Qt::DisplayRole).toString();
    return QString();
}


QIcon ItemDelegate::decoration(const QModelIndex &index) const
{
    if (index.isValid())
      return  QIcon("/home/yousef/kde.png");
      //  return qvariant_cast<QIcon>(index.model()->data(index, Qt::DecorationRole));
    return QIcon();


}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   return QItemDelegate::sizeHint(option,index);
}

QPalette::ColorRole ItemDelegate::foregroundRole(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    if (option.state & QStyle::State_Selected) return QPalette::HighlightedText;
    return QPalette::Text;


}


void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
QItemDelegate::paint(painter,option,index);
return;

    if (!index.isValid()) return;
    painter->save();

    QFont normalfont = option.font;
    QFont boldfont = normalfont;
    boldfont.setBold(true);

    QString firstRow = firstLine(index);

    QPixmap pixmap;
    if(m_cache->contains(index))
    {
        pixmap = m_cache->value(index);
    }else{
        QPixmap pixmap =decoration(index).pixmap(option.decorationSize);;
        if(!pixmap.isNull())
            m_cache->insert(index, pixmap);
    }

    QColor textcol = option.palette.color(foregroundRole(option, index));

    // Draw the background
    QStyleOptionViewItemV4 opt = option;
    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
    int x,y;int rWidth=option.rect.width();
    // Draw the icon

    x = option.rect.left()+rWidth/2-(pixmap.width())/2;
    //        x = option.rect.left()+(option.decorationSize.width()-pixmap.width()+decorationMargin)/2;

    y = option.rect.top()+4;


    QRect pixmapRect = QStyle::visualRect(option.direction, option.rect, QRect(x, y, pixmap.width(), pixmap.height()));
    painter->drawPixmap(pixmapRect.x(), pixmapRect.y(), pixmap);

    // Draw the text
    QFontMetrics fm1(boldfont);


    int y1;
    painter->setPen(textcol);

    if(fm1.width(firstRow)>=rWidth-decorationMargin){

        firstRow=fm1.elidedText(firstRow,Qt::ElideRight,rWidth-decorationMargin);

    }

    x = option.rect.left()+(rWidth)/2-(fm1.width(firstRow)/2);
    y1 = option.rect.top()+option.decorationSize.height()+4;

    QRect firstRowRect = QStyle::visualRect(option.direction, option.rect, QRect(x, y1, fm1.width(firstRow), fm1.lineSpacing()));
    // First line

    painter->drawText(firstRowRect, Qt::AlignCenter, firstRow);

    painter->restore();

}


