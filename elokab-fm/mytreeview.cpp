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

#include "mytreeview.h"
//#include "itemdelegate.h"
#include <QHeaderView>
#include<QDebug>
/*****************************************************************************************************
 *
 *****************************************************************************************************/

MyTreeView::MyTreeView(QFileSystemModel *model,QWidget *parent) :
     QTreeView(parent),mModel(model)
{
     setModel(mModel);
     this->columnViewportPosition(0);

     setSelectionMode(QAbstractItemView::ExtendedSelection);
     setDragDropMode(QAbstractItemView::DragDrop);
     setDefaultDropAction(Qt::MoveAction);
     setDropIndicatorShown(true);

     header()->setProperty("showSortIndicator", QVariant(true));
     setSortingEnabled(true);

     setExpandsOnDoubleClick(false);
         setAlternatingRowColors(true);
     this->setColumnWidth(0,300);
//         setItemDelegate(new ItemDelegate);
}

/**************************************************************************************
 *
 **************************************************************************************/
void MyTreeView::setTreeIconSize(int size)
{
     setIconSize(QSize(size, size));
}

/**************************************************************************************
 *
 **************************************************************************************/
QModelIndexList  MyTreeView::selectedIndexes() const
{
     return QTreeView::selectedIndexes();

}

/**************************************************************************************
 *
 **************************************************************************************/
QStringList MyTreeView::selectedFiles()
{
     QStringList list;

     foreach (QModelIndex indx, selectedIndexes()) {

          QString path=mModel->filePath(indx);
          if(!list.contains(path))
               list.append(path);
     }
     return list;

}

/**************************************************************************************
 *
 **************************************************************************************/
void MyTreeView::setRootPath(const QString &url)
{
     setRootIndex(mModel->index(url));
     collapseAll();

}
void MyTreeView::setdDoubleClickTriggers(bool arg)
{
    if(arg){
        setEditTriggers(QAbstractItemView::EditKeyPressed |
                        QAbstractItemView::SelectedClicked);
    }else{
        setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}
