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

#include "mylistview.h"
//#include "itemdelegate.h"
//#include <EMimIcon>
#include <QDebug>

/*****************************************************************************************************
 *
 *****************************************************************************************************/

MyListView::MyListView(QFileSystemModel *model, Actions *action,QWidget *parent) :
     QListView(parent),mModel(model),mActions(action)
{

    setModel(mModel);
    setContentsMargins(3,3,3,3);
    setSpacing(5);
    setResizeMode(QListView::Adjust);

    setWrapping(true);
    //  setFlow(QListView::LeftToRight);

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionRectVisible(true);

    //  setMovement(QListView::Snap);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::MoveAction);
    setDropIndicatorShown(true);

    setMouseTracking(true);
    setWordWrap(false);
}
void MyListView::setdDoubleClickTriggers(bool arg)
{
    if(arg){
        setEditTriggers(QAbstractItemView::EditKeyPressed |
                        QAbstractItemView::SelectedClicked);
    }else{
        setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void MyListView::startDrag(Qt::DropActions supportedActions) {
//    if(movement() != Static) {
//        QListView::startDrag(supportedActions);
//    }
//    else {
        QAbstractItemView::startDrag(supportedActions);
//    }
    qDebug("startDrag");
}
void MyListView::dragEnterEvent(QDragEnterEvent* event) {

//    if(movement() != Static) {
//        QListView::dragEnterEvent(event);
//    }
//    else {
        QAbstractItemView::dragEnterEvent(event);
//   / }
    qDebug("dragEnterEvent");

            //static_cast<FolderView*>(parent())->childDragEnterEvent(event);
}

void MyListView::dragLeaveEvent(QDragLeaveEvent* e) {
//    if(movement() != Static) {
//        QListView::dragLeaveEvent(e);
//    }
//    else {
        QAbstractItemView::dragLeaveEvent(e);
//    }
     qDebug("dragLeaveEvent");

  //  static_cast<FolderView*>(parent())->childDragLeaveEvent(e);
}
void MyListView::dragMoveEvent(QDragMoveEvent* e) {
//    if(movement() != Static) {
//        QListView::dragMoveEvent(e);
//    }
//    else {
        QAbstractItemView::dragMoveEvent(e);
//    }
     //qDebug("dragMoveEvent");
  //  static_cast<FolderView*>(parent())->childDragMoveEvent(e);
}

void MyListView::dropEvent(QDropEvent* e) {

   // static_cast<FolderView*>(parent())->childDropEvent(e);

//    if(movement() != Static) {
//        QListView::dropEvent(e);
//    }
//    else {
        QAbstractItemView::dropEvent(e);
//    }
   qDebug("dropEvent");
}
/**************************************************************************************
 *
 **************************************************************************************/
void MyListView::setListViewMode(ViewMode mode)
{
    clearPropertyFlags();
    //setFlow(QListView::LeftToRight);

    setContentsMargins(3,3,3,3);
    setSpacing(5);

    if(mode==QListView::ListMode){
        setFlow(QListView::LeftToRight);
        setMovement(QListView::Snap);
        //
        //        setWrapping(true);
    }//else{

    setResizeMode(QListView::Adjust);
    setViewMode(mode);
    setWrapping(true);
    // setMouseTracking(true);
}

/**************************************************************************************
 *
 **************************************************************************************/
void   MyListView::setViewIconSize(int size)
{

  //  16<<22<<24<<32<<48<<64<<72<<96<<128
  //  QFontMetrics fm(this->font());

//  int defW=64;
//   int defH=(fm.height()+6);
//         if(size<=22)defW=64;
//    else if(size<=24)defW=62;
//    else if(size<=32)defW=60;
//    else if(size<=48)defW=58;
//    else if(size<=64)defW=46;
//    else if(size<=72)defW=34;
//    else if(size<=96)defW=30;
//    else if(size<=128)defW=16;

//     if(viewMode()==QListView::IconMode){
//          setGridSize(QSize(size +defW,size+defH));
//     } else
//          setGridSize(QSize());

 setIconSize(QSize(size, size));

}

/**************************************************************************************
 *
 **************************************************************************************/
QModelIndexList  MyListView::selectedIndexes() const
{
     return QListView::selectedIndexes();
}
/**************************************************************************************
 *
 **************************************************************************************/
QStringList MyListView::selectedFiles()
{
     QStringList list;
     foreach (QModelIndex indx, selectedIndexes()) {
          list.append(mModel->filePath(indx));
     }
     return list;

}
/**************************************************************************************
 *
 **************************************************************************************/
void MyListView::setRootPath(const QString &url)
{
     //  mModel->blockSignals(true);
     setRootIndex(mModel->index(url));
     //   mModel->blockSignals(false);
}

/**************************************************************************************
 *
 **************************************************************************************/
void MyListView::onItemEntered(QModelIndex index)
{
     // setStatusTip(mModel->filePath(index));
    // setToolTip(mModel->fileName(index));
     emit fileEntered("file://"+mModel->filePath(index));
     //  emit fileEntered("fileName://"+mModel->fileName(index));


}

void MyListView::viewEntered()
{
//     setToolTip(QString());
//     QModelIndex index=   this->selectionModel()->currentIndex();
//     if(index.isValid())
//          emit fileEntered("file://"+mModel->filePath(index));
//     else
//          emit fileEntered("");
}

/**************************************************************************************
 * TEMP
 **************************************************************************************/
void MyListView::startEdit()
{



}
/**************************************************************************************
 *
 **************************************************************************************/
void MyListView::setFileRenamed(QString path,QString old,QString newN)
{
     qDebug()<<path<<old<<newN;
}
