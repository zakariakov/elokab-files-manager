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
     //    int zoom=48;
     setModel(mModel);
//QFont font;
//font.setPointSize(12);
//font.setBold(true);
//setFont(font);
//     connect(mModel,SIGNAL(fileRenamed(QString,QString,QString)),this,SLOT(setFileRenamed(QString,QString,QString)));
    setContentsMargins(5,5,5,5);
     setViewMode(QListView::IconMode);
     setResizeMode(QListView::Adjust);
     setSelectionMode(QAbstractItemView::ExtendedSelection);
     setSelectionRectVisible(true);
     setWrapping(true);
    // setMouseTracking(true);
     setEditTriggers(QAbstractItemView::EditKeyPressed |
                     QAbstractItemView::SelectedClicked);

     setDragDropMode(QAbstractItemView::DragDrop);
     setDefaultDropAction(Qt::MoveAction);
     setDropIndicatorShown(true);
    // setSpacing(5);
     setWordWrap(true);
     connect(this,SIGNAL(entered(QModelIndex)),this,SLOT(onItemEntered(QModelIndex)));
     connect(this,SIGNAL(viewportEntered()),this,SLOT(viewEntered()));
//setItemDelegate(new ItemDelegate);
}

/**************************************************************************************
 *
 **************************************************************************************/
void MyListView::setListViewMode(ViewMode mode)
{
     switch (mode) {
          case QListView::IconMode:
               setViewMode(QListView::IconMode);
          case QListView::ListMode:

             setViewMode(QListView::ListMode);

               break;

     }
}

/**************************************************************************************
 *
 **************************************************************************************/
void   MyListView::setViewIconSize(int size)
{
  //  16<<22<<24<<32<<48<<64<<72<<96<<128
    QFontMetrics fm(this->font());

  int defW=64;
   int defH=5+(fm.height()*2);
         if(size<=22)defW=6+106;
    else if(size<=24)defW=6+104;
    else if(size<=32)defW=6+96;
    else if(size<=48)defW=6+80;
    else if(size<=64)defW=6+64;
    else if(size<=72)defW=6+56;
    else if(size<=96)defW=6+32;
    else if(size<=128)defW=16;



     if(viewMode()==QListView::IconMode){
          setGridSize(QSize(size +defW,size+defH));
     } else
          setGridSize(QSize());

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
     setToolTip(QString());
     QModelIndex index=   this->selectionModel()->currentIndex();
     if(index.isValid())
//          emit fileEntered("file://"+mModel->filePath(index));
//     else
          emit fileEntered("");
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
