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

#include "dialogactions.h"
#include "ui_dialogactions.h"
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QAction>
#include <QDebug>
DialogActions::DialogActions(QWidget *parent) :
     QDialog(parent),
     ui(new Ui::DialogActions)
{
     ui->setupUi(this);

     if(this->layoutDirection()==Qt::RightToLeft)
     {
          ui->toolButtonRemove->setArrowType(Qt::RightArrow);
          ui->toolButtonAdd->setArrowType(Qt::LeftArrow);
     }

     connect(ui->treeWidgetOrig,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(slotItemPressed(QListWidgetItem*)));
     connect(ui->treeWidgetDest,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(slotItemPressed(QListWidgetItem*)));
     connect(ui->treeWidgetOrig,SIGNAL(itemSelectionChanged()),this,SLOT(sourceItemSelectionChanged()));
     connect(ui->treeWidgetDest,SIGNAL(itemSelectionChanged()),this,SLOT(destItemSelectionChanged()));

     connect(ui->toolButtonAdd,SIGNAL(clicked()),this,SLOT(slotAddAction()));
     connect(ui->toolButtonRemove,SIGNAL(clicked()),this,SLOT(slotRemoveAction()));
     connect(ui->toolButtonUp,SIGNAL(clicked()),this,SLOT(slotActionUp()));
     connect(ui->toolButtonDown,SIGNAL(clicked()),this,SLOT(slotActionDown()));

}

/*************************************************************************************************
 *
 ************************************************************************************************/
DialogActions::~DialogActions()
{
     qDebug()<<"DialogActions deleted";
     delete ui;
}

/*************************************************************************************************
 *
 ************************************************************************************************/
void DialogActions::slotItemPressed(QListWidgetItem* item)
{
     if(item->data(Qt::UserRole).toString()=="Separator")
          ui->treeWidgetOrig->setDefaultDropAction(Qt::CopyAction);
     else
          ui->treeWidgetOrig->setDefaultDropAction(Qt::MoveAction);

}

/*************************************************************************************************
 *
 ************************************************************************************************/
void DialogActions::slotAddAction()
{

     //-------------------------------------
     QListWidgetItem *curItem=ui->treeWidgetOrig->currentItem();
     QListWidgetItem *curItemDest=ui->treeWidgetDest->currentItem();

     if(!curItem)
          return;

     QListWidgetItem *item=new QListWidgetItem;
     item=curItem->clone();
     if(curItemDest)
     {
          int ind=ui->treeWidgetDest->row(curItemDest)+1;
          ui->treeWidgetDest->insertItem( ind,item);
     }else{
          ui->treeWidgetDest->addItem(item);
     }

     if(curItem->data(Qt::UserRole).toString()!="Separator")
     {
          int ind=ui->treeWidgetOrig->row(curItem);
          ui->treeWidgetOrig->takeItem(ind);
          ui->treeWidgetDest->setCurrentItem(item);
     }
}

/*************************************************************************************************
 *
 ************************************************************************************************/
void DialogActions::slotRemoveAction()
{

     QListWidgetItem *curItem=ui->treeWidgetDest->currentItem();
     if(!curItem)return;

     if(curItem->data(Qt::UserRole).toString()!="Separator")
     {
          QListWidgetItem *item=new QListWidgetItem;
          item=curItem->clone();
          ui->treeWidgetOrig->addItem(item);
          ui->treeWidgetOrig->setCurrentItem(item);
     }

     int index= ui->treeWidgetDest->row(curItem);
     ui->treeWidgetDest->takeItem(index);

}

/*************************************************************************************************
 *
 ************************************************************************************************/
void DialogActions::on_buttonBox_accepted()
{
     mListAvailableActions.clear();
     for(int i=0;i<ui->treeWidgetDest->count();i++)
     {
          //  listActToAdd.append(ui->treeWidgetDest->item(i)->data(Qt::UserRole));
          mListAvailableActions.append(ui->treeWidgetDest->item(i)->data(Qt::UserRole).toString());
          //  qDebug()<<ui->treeWidgetDest->item(i)->data(Qt::ToolTipRole).toString();
     }
}

/*************************************************************************************************
 *
 ************************************************************************************************/
/*
void DialogActions::setListActions(QList<QAction *>listActions,QList<QVariant >listToAdd)
{

     QListWidgetItem *itemSep=new QListWidgetItem;
     itemSep->setText(tr("----- Separator -----"));
     itemSep->setData(Qt::UserRole,1000);
     ui->treeWidgetOrig->addItem(itemSep);

     for(int i=0;i<listActions.count();i++)
     {

          if(!listToAdd.contains(i))
          {
               if(listActions.at(i)->isSeparator())
                    continue;

               QListWidgetItem *item=new QListWidgetItem;
               item->setText(listActions.at(i)->text());
               item->setIcon(listActions.at(i)->icon());
               item->setData(Qt::UserRole,i);
               item->setData(Qt::ToolTipRole,listActions.at(i)->objectName());

               ui->treeWidgetOrig->addItem(item);

          }

     }

     for(int i=0;i<listToAdd.count();i++){
          QListWidgetItem *item=new QListWidgetItem;

          int indx=listToAdd.at(i).toInt();

          if(indx==1000)
          {
               item->setText(tr("----- Separator -----"));
               item->setData(Qt::UserRole,1000);
          }else{
               item->setText( listActions.at(indx)->text());
               item->setIcon(listActions.at(indx)->icon());
               item->setData(Qt::UserRole,indx);
               item->setData(Qt::ToolTipRole,listActions.at(indx)->objectName());
qDebug()<<listActions.at(indx)->objectName();
          }

          ui->treeWidgetDest->addItem(item);
     }


}
*/
void DialogActions::setListActions(QList<QAction *>listActions,QStringList listToAdd)
{
     QListWidgetItem *itemSep=new QListWidgetItem;
     itemSep->setText(tr("----- Separator -----"));
     itemSep->setData(Qt::UserRole,"Separator");
     ui->treeWidgetOrig->addItem(itemSep);

     foreach (QAction *act, listActions)
     {
          QString name=act->objectName();

          if(name.isEmpty())
               continue;

          if(!listToAdd.contains(name))
          {

               QListWidgetItem *item=new QListWidgetItem;
               item->setText(act->text());
               item->setIcon(act->icon());
               //item->setData(Qt::UserRole,i);
               item->setData(Qt::UserRole,act->objectName());

               ui->treeWidgetOrig->addItem(item);
          }

     }

     foreach (QString s, listToAdd)
     {

          if(s=="Separator")
          {
               QListWidgetItem *item=new QListWidgetItem;
               item->setText(tr("----- Separator -----"));
               //item->setData(Qt::UserRole,1000);
               item->setData(Qt::UserRole,"Separator");
               ui->treeWidgetDest->addItem(item);
               continue;
          }
          foreach (QAction *act, listActions)
          {
               QString name=act->objectName();
               if(name.isEmpty())
                    continue;
               if(name==s){
                    QListWidgetItem *item=new QListWidgetItem;
                    item->setText(act->text());
                    item->setIcon(act->icon());
                    item->setData(Qt::UserRole,act->objectName());
                    ui->treeWidgetDest->addItem(item);
               }
          }


     }


}

/*************************************************************************************************
 *
 ************************************************************************************************/
void DialogActions::destItemSelectionChanged()
{
     QListWidgetItem *item=ui->treeWidgetDest->currentItem();

     if(item)
     {
          ui->toolButtonRemove->setEnabled(true);
          ui->toolButtonDown->setEnabled(true);
          ui->toolButtonUp->setEnabled(true);
     }else{
          ui->toolButtonRemove->setEnabled(false);
          ui->toolButtonDown->setEnabled(false);
          ui->toolButtonUp->setEnabled(false);
     }
}

/*************************************************************************************************
 *
 ************************************************************************************************/
void DialogActions::sourceItemSelectionChanged()
{
     QListWidgetItem *item=ui->treeWidgetOrig->currentItem();

     if(item)
          ui->toolButtonAdd->setEnabled(true);
     else
          ui->toolButtonAdd->setEnabled(false);

}

/*************************************************************************************************
 *
 ************************************************************************************************/
void DialogActions::slotActionUp()
{
     QListWidgetItem *curItem=ui->treeWidgetDest->currentItem();
     if(!curItem)return;

     int index= ui->treeWidgetDest->row(curItem);
     if(index==0)return;

     QListWidgetItem *item=new QListWidgetItem;
     item=curItem->clone();

     ui->treeWidgetDest->takeItem(index);
     ui->treeWidgetDest->insertItem(index-1,item);
     ui->treeWidgetDest->setCurrentItem(item);
}

/*************************************************************************************************
 *
 ************************************************************************************************/
void DialogActions::slotActionDown()
{

     QListWidgetItem *curItem=ui->treeWidgetDest->currentItem();
     if(!curItem)return;

     int index= ui->treeWidgetDest->row(curItem);
     if(index== ui->treeWidgetDest->count()-1)return;

     QListWidgetItem *item=new QListWidgetItem;
     item=curItem->clone();

     ui->treeWidgetDest->takeItem(index);
     ui->treeWidgetDest->insertItem(index+1,item);
     ui->treeWidgetDest->setCurrentItem(item);
}
