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

#include "searchview.h"
#include "ui_searchview.h"
#include <EMimIcon>
#include <QDir>
#include <QProgressDialog>
#include <QTextStream>
#include <QDirIterator>
#include <QDebug>
#include <QPainter>
#include <QFileSystemWatcher>
#include <QLineEdit>

//______________________________________________________________________________________
SearchView::SearchView(QWidget *parent) :
     QWidget(parent),hasEdetor(0),
     ui(new Ui::SearchView)
{

     ui->setupUi(this);
     mSysWatcher=new QFileSystemWatcher;
     connect(mSysWatcher,SIGNAL(fileChanged(QString)),this,SLOT(fileChanged(QString)));
     ui->toolButtonClear->setIcon(EIcon::fromTheme("edit-clear-list",("unknown")));
     ui->toolButtonClear->setToolTip(tr( "clear text"));
     ui->toolButtonClose->setIcon(EIcon::fromTheme("dialog-close",("unknown")));
     ui->toolButtonClose->setToolTip(tr("Quit searching"));
     ui->toolButtonStopFind->setIcon(EIcon::fromTheme("process-stop",("unknown")));
     ui->toolButtonStopFind->setToolTip(tr("Stop searching Ctrl+S"));
     ui->toolButtonStopFind->setStatusTip(tr("Stop searching Ctrl+S"));

     ui->labelAnim->setPixmap(QPixmap(":/icons/process-idle"));

     connect(ui->toolButtonClose,SIGNAL(clicked()),SLOT(cancelSearch()));
     connect(ui->comboBox,SIGNAL(activated(QString)),SLOT(goSearch(QString)));
     connect(ui->toolButtonStopFind,SIGNAL(clicked()),SLOT(stopSearch()));
     connect(ui->treeWidget,SIGNAL(itemActivated(QTreeWidgetItem*,int)),this,SLOT(lanchApp(QTreeWidgetItem*,int)));

     mTimer=new QTimer;
     connect(mTimer,SIGNAL(timeout()),this,SLOT(updateAimation()));
     setStyleSheet("QFrame #frameFind{background-color:palette(base);}");
     ui->treeWidget->setEditTriggers(QAbstractItemView::EditKeyPressed |
                                     QAbstractItemView::SelectedClicked);


}

//______________________________________________________________________________________
void SearchView::updateAimation()
{
     mCurY++;
     if(mCurY>7)
          mCurY=0;

     QPainter p;
     QPixmap pix(22,22);
     pix.fill(Qt::transparent);
     QPixmap pixOrig(":/icons/process-working");
     p.begin(&pix);
     p.drawPixmap(0,0,22,22,pixOrig,0,mCurY*22,22,22);
     p.end();
     ui->labelAnim->setPixmap(pix);

}

//______________________________________________________________________________________
void SearchView::closeEvent(QCloseEvent *)
{

     qDebug()<<"SearchView::QCloseEvent";
     mStop=true;

}

//______________________________________________________________________________________
SearchView::~SearchView()
{
     qDebug()<<"SearchView closed";
     mStop=true;
     delete ui;
}

//______________________________________________________________________________________
void SearchView::cancelSearch()
{
     mStop=true;
     mTimer->stop();
     emit searchingCanceled(m_path);
     ui->treeWidget->clear();
}

//______________________________________________________________________________________
QTreeWidget *SearchView::searchTreeView()
{
     return ui->treeWidget;
}

//______________________________________________________________________________________
void SearchView::stopSearch()
{
     mStop=true;
     mTimer->stop();
     ui->widgetProgress->setVisible(false);
     emit fileSelected(tr("%1 file(s) found")
                       .arg(QString::number(mNfiles))) ;
}

//______________________________________________________________________________________
void SearchView::setPath(const QString &path)
{
     ui->widgetProgress->setVisible(false);
     //  ui->lineEditFind->clear();
     ui->comboBox->currentText().clear();
     ui->labelInfo->clear();
     ui->treeWidget->clear();
     m_path=path;
     mCurY=0;
     mNfiles=0;
     // ui->lineEditFind->setFocus();
     ui->comboBox->setFocus();

}

//______________________________________________________________________________________
void SearchView::goSearch(const QString &str)
{
     mStop=true;
     mCurY=0;
     mNfiles=0;
     ui->treeWidget->clear();
     ui->widgetProgress->setVisible(true);
     mTimer->start(500);
     mStop=false;




     //  if(ui->checkBoxSubDir->isChecked()){
     searchCurdir(m_path,str);
     searchAllSubdir(m_path,str);
     // } else
     //     searchCurdir(m_path,ui->lineEditFind->text());

     stopSearch();

}

//______________________________________________________________________________________
void SearchView::searchAllSubdir(const QString &path,const QString &text)
{
     QDirIterator it( path,QDir::AllDirs |
                      QDir::NoDotAndDotDot |
                      QDir::NoSymLinks |
                      QDir::Hidden, QDirIterator::Subdirectories);

     while(it.hasNext())
     {

          if(mStop)  return;

          it.next();
          QString f=it.filePath();
          qApp->processEvents();
          ui->labelInfo->setText(tr("%1 file(s) found. Searching in folder  %2")
                                 .arg(QString::number(mNfiles)) .arg(it.fileName()));

          qApp->processEvents();
          searchCurdir(f,text);

     }
}

//______________________________________________________________________________________
void SearchView::searchCurdir(const QString &path,const QString &text)
{

     QString str =text;
     mCurrentDir = QDir(path);
     QStringList files;

     if (str.isEmpty())
          str = "*";

     if(ui->radioButtonNamed->isChecked())
     {
          files = mCurrentDir.entryList(QStringList(str),QDir::AllEntries | QDir::NoDotAndDotDot);
     }else{
          files = mCurrentDir.entryList(QStringList("*"),QDir::Files | QDir::NoDotAndDotDot);
          files = findFiles(files, str);
     }

     addFiles(files);
}

//______________________________________________________________________________________
QStringList SearchView::findFiles(const QStringList &files, const QString &text)
{

     QStringList foundFiles;
     QStringList plainTextList;
     plainTextList << "application/javascript"
                   << "application/x-javascript"
                   << "application/xml"
                   << "application/x-csh'"
                   << "application/x-sh"
                   << "application/x-shellscript"
                   << "application/javascript"
                   << "application/x-perl"
                   << "application/x-php"
                   << "application/x-ruby";

     for (int i = 0; i < files.size(); ++i)
     {
          qApp->processEvents();
          if (mStop)
               break;

          QFile file(mCurrentDir.absoluteFilePath(files[i]));
          QFileInfo fi(file.fileName());
          QString mim=EMimIcon::mimeTyppe(fi);

          if(mim.contains("text")||plainTextList.contains(mim))
          {
               ui->labelInfo->setText(tr("%1 file(s) found. Searching in file  %2")
                                      .arg(QString::number(mNfiles)) .arg(fi.fileName()));

               if (file.open(QIODevice::ReadOnly))
               {
                    QString line;
                    QTextStream in(&file);
                    while (!in.atEnd())
                    {
                         if (mStop) break;
                         line = in.readLine();
                         if (line.contains(text)) {
                              foundFiles << files[i];
                              break;
                         }
                    }
               }
          }
     }
     return foundFiles;
}

//______________________________________________________________________________________
void SearchView::addFiles(const QStringList &files)
{

     foreach (QString s, files) {
          if(mStop)  return;

          qApp->processEvents();

          mNfiles++;

          QFileInfo fi(mCurrentDir.absoluteFilePath(s));
          QTreeWidgetItem *item=new QTreeWidgetItem(ui->treeWidget);
          item->setText(0,fi.fileName());
          item->setText(1,EMimIcon::formatSize(fi.size()));
          item->setText(2,fi.path());
          item->setIcon(0,EMimIcon::icon(fi));
          item->setData(0,Qt::UserRole,fi.absoluteFilePath());
          //  sysWatcher=new QFileSystemWatcher(QStringList()<<fi.absoluteFilePath());
          mSysWatcher->addPath(fi.absoluteFilePath());
          //connect(sysWatcher,SIGNAL(fileChanged(QString)),this,SLOT(fileChanged(QString)));
          ui->labelInfo->setText(tr("%1 file(s) found")
                                 .arg(QString::number(mNfiles))) ;

     }
     ui->treeWidget->resizeColumnToContents(0);
}

//______________________________________________________________________________________
void  SearchView::fileChanged(QString file)
{

     if(!QFile::exists(file)){
          for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
               QTreeWidgetItem *item=ui->treeWidget->topLevelItem(i);
               if(item->data(0,Qt::UserRole).toString()==file){
                    ui->treeWidget->takeTopLevelItem(i);
                    mNfiles--;
               }
          }
          mSysWatcher->removePath(file);
     }
}

//______________________________________________________________________________________
QString  SearchView::selectedFile()
{
     QTreeWidgetItem *item=ui->treeWidget->currentItem();
     if(!item)return "";
     qDebug()<<item->data(0,Qt::UserRole).toString();
     return item->data(0,Qt::UserRole).toString();
}

//______________________________________________________________________________________
QStringList  SearchView::selectedFiles()
{
     QStringList list;
     foreach (QTreeWidgetItem *item, ui->treeWidget->selectedItems()) {
          list.append(item->data(0,Qt::UserRole).toString());
     }
     return list;
}

//______________________________________________________________________________________
int SearchView::selectedFilesCount()
{

     qDebug()<<"count"<<ui->treeWidget->selectedItems().count();
     return ui->treeWidget->selectedItems().count();

}

//______________________________________________________________________________________
void SearchView::lanchApp(QTreeWidgetItem* item,int)
{
     QFileInfo info(item->data(0,Qt::UserRole).toString());
     if(info.isDir())
     {
          emit  setUrl(info.absoluteFilePath());
     }else{

          QString mim=EMimIcon::mimeTyppe(info);
          if (EMimIcon::launchApp(info.absoluteFilePath(),mim)==false)
               emit  showOpenwithDlg(info.absoluteFilePath());

     }

}

//______________________________________________________________________________________
void SearchView::editCurentItem()
{

     QTreeWidgetItem *current=ui->treeWidget->currentItem();
     if(current){
          QString data=current->data(0,Qt::UserRole).toString();
          QFileInfo fi(data);
          if(fi.isWritable()){
               mCurrentEditable=current->text(0);
               hasEdetor=true;
               ui->treeWidget->openPersistentEditor(current);
          }
     }

}

//______________________________________________________________________________________
void SearchView::on_treeWidget_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *previous)
{
     if(!hasEdetor) return;

     if(mCurrentEditable.isEmpty()) return;

     if(previous){
          ui->treeWidget->closePersistentEditor(previous,0);

          QString newText=previous->text(0);

          if(newText!=mCurrentEditable){
               QFileInfo fi(previous->data(0,Qt::UserRole).toString());
               QString path=fi.path();

               if(QFile::rename(fi.absoluteFilePath(),path+"/"+newText)){
                    previous->setData(0,Qt::UserRole,path+"/"+newText);

                    mSysWatcher->removePath(fi.absoluteFilePath());
                    mSysWatcher->addPath(path+"/"+newText);
               }else{

                    previous->setText(0,mCurrentEditable);
               }
          }
     }

     mCurrentEditable="";
     hasEdetor=false;

}

//______________________________________________________________________________________
void SearchView::on_treeWidget_itemSelectionChanged()
{
     QList< QTreeWidgetItem *>list=ui->treeWidget->selectedItems();
     if(list.count()==0){
          emit fileSelected(tr("%1 file(s) found")
                            .arg(QString::number(mNfiles))) ;
     }else if(list.count()==1){
          QTreeWidgetItem *item=list.at(0);
          emit fileSelected("file://"+item->data(0,Qt::UserRole).toString());
     }else{
          emit fileSelected(tr("%1 file(s) selected")
                            .arg(list.count())) ;
     }


}

