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
     setEditTriggers(QAbstractItemView::EditKeyPressed |
                     QAbstractItemView::SelectedClicked);

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
     //    resizeColumnToContents(0);
}
