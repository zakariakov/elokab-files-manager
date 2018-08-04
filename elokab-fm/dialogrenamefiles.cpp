#include "dialogrenamefiles.h"
#include "ui_dialogrenamefiles.h"
#include <QPushButton>
#include <QDebug>
#include <QFileInfo>

DialogRenameFiles::DialogRenameFiles(QStringList list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRenameFiles)
{
    ui->setupUi(this);
    btnPreview=new QPushButton;
    btnPreview->setText(tr("Preview"));
     connect(btnPreview,SIGNAL(clicked()),this,SLOT(preview()));

    btnUndo=new QPushButton;
    btnUndo->setText(tr("Undo Change"));
     connect(btnUndo,SIGNAL(clicked()),this,SLOT(undoChange()));
    btnUndo->setIcon(QIcon::fromTheme("edit-undo",QIcon::fromTheme("undo")));

    ui->buttonBox->addButton(btnUndo,QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(btnPreview,QDialogButtonBox::ActionRole);

    btnUndo->setVisible(false);
ui->widgetNumKeep->setEnabled(false);
    list.sort();
    foreach (QString file, list) {
        QFileInfo info(file);
        QString fileName=info.fileName();
        QTreeWidgetItem *item=new QTreeWidgetItem(ui->treeWidget);
        item->setText(0,fileName);
        item->setData(0,Qt::UserRole,file);
        if(info.isDir())
            item->setIcon(0,QIcon::fromTheme("folder"));
        else
            item->setIcon(0,QIcon::fromTheme("unknown",QIcon::fromTheme("application-x-zerosize")));

        item->setCheckState(0,Qt::Checked);
    }


    ui->treeWidget->setColumnWidth(0,300);
}

DialogRenameFiles::~DialogRenameFiles()
{
    delete ui;
}

QString getSufix(QFileInfo info)
{
    QString suf=info.completeSuffix();
     qDebug()<<suf;
    if(info.isHidden()){
QString fileName=info.fileName();
        if(fileName.count('.')==1)
            suf=QString();
        else
            suf=suf.section('.',1,-1);

    }
        qDebug()<<"suf"<<suf;
        return  suf;
}
QString getBaseName(QFileInfo info)
{
     QString base=info.baseName();
     if(info.isHidden()){

     QString suf= getSufix(info);
     base=info.fileName();
     base=base.remove(suf);

     }
      qDebug()<<"base"<<base;
     return base;
}

void DialogRenameFiles::preview()
{
    //  qDebug()<<__FUNCTION__<<mList;
    int count=ui->treeWidget->topLevelItemCount();

    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem *item=ui->treeWidget->topLevelItem(i);
        item->setText(1,QString());
    }

    switch (ui->tabWidget->currentIndex()) {
    case 0:
        qDebug()<<__FUNCTION__<<"0";
        replaceString();
        break;
    case 1:
        qDebug()<<__FUNCTION__<<"1";
        insertString();
        break;
    case 2:
        qDebug()<<__FUNCTION__<<"2";
        insertNumiric();
        break;
    default:
        break;
    }

}



void DialogRenameFiles::replaceString()
{
    bool ignore=ui->checkBoxIgnoreSuf->isChecked();

    QString textReplace=ui->lineEditReplace->text();

    if(textReplace.isEmpty())return;

    QString textWith=ui->lineEditReplaceWith->text();

    int count=ui->treeWidget->topLevelItemCount();

    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem *item=ui->treeWidget->topLevelItem(i);
       if(item->checkState(0)==Qt::Unchecked)
           continue;

        QFileInfo fi=item->data(0,Qt::UserRole).toString();

        QString orig;
        if(ignore) orig=item->text(0);
        else  orig=getBaseName(fi);

        QString suf=getSufix(fi);
        if(!suf.isEmpty())
            suf="."+suf;

        if(orig.contains(textReplace)){
            QString str=orig.replace(textReplace,textWith);
            if(ignore) item->setText(1,orig);

            else item->setText(1,orig+suf);
        }

    }
}

void DialogRenameFiles::insertString()
{
    // bool ignore=ui->checkBoxIgnoreSuf->isChecked();
    bool insertBeforeSuf=ui->radioButtonInsertBefore->isChecked();
    bool insertAtPos=ui->radioButtonAtPos->isChecked();

    QString text=ui->lineEditInsert->text();
    if(text.isEmpty())return;

    int count=ui->treeWidget->topLevelItemCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem *item=ui->treeWidget->topLevelItem(i);
        if(item->checkState(0)==Qt::Unchecked)
            continue;

        QFileInfo fi=item->data(0,Qt::UserRole).toString();

        if(insertAtPos){
            QString orig=item->text(0);
            int i=ui->spinBoxInsert->value();
            if(i>orig.length()) i=orig.length();
            orig= orig.insert(i,text);
            item->setText(1,orig);
            continue;
        }

        if(insertBeforeSuf){
            QString  base=getBaseName(fi);
            QString suf=getSufix(fi);
            base+=text;
            if(!suf.isEmpty())
                base+="."+ suf;
            item->setText(1,base);

        }else{
            QString str= item->text(0);
            str+=text;
            item->setText(1,str);
        }



    }


}

void DialogRenameFiles::insertNumiric()
{
  //  bool ignore=ui->checkBoxIgnoreSuf->isChecked();
    bool keep=ui->checkBoxKeep->isChecked();
    bool insetNumBefore=ui->radioButtonNumBefore->isChecked();
      bool insetNumAt=ui->radioButtonNumAt->isChecked();

    int count=ui->treeWidget->topLevelItemCount();
    int n=2;
    int at=ui->spinBoxNumeric->value();
    if(count+at >99)n=3;
    for (int i = 0; i < count; ++i) {

        QTreeWidgetItem *item=ui->treeWidget->topLevelItem(i);
        if(item->checkState(0)==Qt::Unchecked)
            continue;

        QFileInfo fi=item->data(0,Qt::UserRole).toString();

        QString sufix=getSufix(fi);

        QString num=   QString::number(i+at);
        QString str=ui->lineEditNumPrefix->text();
        str+=num.rightJustified(n,'0');
        str+=ui->lineEditNumSufix->text();

        // ignore curent text
        if(!keep){
             item->setText(1,str);
             continue;
        }

        // insert Num At value
        if(insetNumAt){

          QString orig=item->text(0);
          int i=ui->spinBoxNumAt->value();
          if(i>orig.length())
              i=orig.length();

          orig= orig.insert(i,str);
          item->setText(1,orig);
          continue;

        }
         //insert Num Before suffix
        if(insetNumBefore){
            QString  base=getBaseName(fi);
            QString suf=getSufix(fi);
            base+=str;
            if(!suf.isEmpty())
                base+="."+ suf;
            item->setText(1,base);

        }
         //insert Num After suffix
        else{
            QString orig= item->text(0);
            orig+=str;
            item->setText(1,orig);
        }



    }


}

void DialogRenameFiles::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->buttonRole(button)==QDialogButtonBox::ApplyRole){
        qDebug()<<"aply---- ";
        preview();
        applyChange();
    }
}

void DialogRenameFiles::applyChange()
{
    int count=ui->treeWidget->topLevelItemCount();

    for (int i = 0; i < count; ++i) {
       QTreeWidgetItem *item=ui->treeWidget->topLevelItem(i);
       if(item->checkState(0)==Qt::Unchecked)
           continue;

       QString newName=item->text(1);
       if(newName.isEmpty())continue;
       QString oldFilePath=item->data(0,Qt::UserRole).toString();
       QFileInfo info(oldFilePath);
       QString path=info.path();

       if(  QFile::rename(oldFilePath,path+"/"+newName)){
           item->setText(0,newName);
           item->setData(0,Qt::UserRole,path+"/"+newName);
           item->setData(1,Qt::UserRole,oldFilePath);
           item->setText(1,QString());
          // ui->toolButtonUndo->setEnabled(true);
             btnUndo->setVisible(true);
       }

    }
}

void DialogRenameFiles::undoChange()
{
    int count=ui->treeWidget->topLevelItemCount();

    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem *item=ui->treeWidget->topLevelItem(i);

        if(item->checkState(0)==Qt::Unchecked)
            continue;

        QString oldFilePath=item->data(1,Qt::UserRole).toString();
        QString curFilePath=item->data(0,Qt::UserRole).toString();
        if(oldFilePath.isEmpty())continue;
        QFileInfo fi(oldFilePath);

        if(  QFile::rename(curFilePath,oldFilePath)){
            item->setText(0,fi.fileName());
            item->setData(0,Qt::UserRole,oldFilePath);
            item->setData(1,Qt::UserRole,oldFilePath);
            item->setText(1,QString());
            // ui->toolButtonUndo->setEnabled(false);
            btnUndo->setVisible(false);
        }

    }
}

void DialogRenameFiles::on_checkBox_toggled(bool checked)
{
//author!=tr("Unknown") ? author:QString()
    Qt::CheckState state=Qt::Unchecked;
    if(checked) state=Qt::Checked;


    int count=ui->treeWidget->topLevelItemCount();

    for (int i = 0; i < count; ++i) {
       QTreeWidgetItem *item=ui->treeWidget->topLevelItem(i);
      item->setCheckState(0,state);
    }
}

void DialogRenameFiles::on_toolButtonDown_clicked()
{
    QTreeWidgetItem *curItem=ui->treeWidget->currentItem();
    if(!curItem)return;

    int index= ui->treeWidget->indexOfTopLevelItem(curItem);
    if(index== ui->treeWidget->topLevelItemCount()-1)return;

    QTreeWidgetItem *item=curItem->clone();
   // item=curItem->clone();

    ui->treeWidget->takeTopLevelItem(index);
    ui->treeWidget->insertTopLevelItem(index+1,item);
    ui->treeWidget->setCurrentItem(item);
}

void DialogRenameFiles::on_toolButtonUp_clicked()
{
    QTreeWidgetItem *curItem=ui->treeWidget->currentItem();
    if(!curItem)return;

    int index= ui->treeWidget->indexOfTopLevelItem(curItem);
    if(index==0)return;

    QTreeWidgetItem *item=curItem->clone();
   // item=curItem->clone();

    ui->treeWidget->takeTopLevelItem(index);
    ui->treeWidget->insertTopLevelItem(index-1,item);
    ui->treeWidget->setCurrentItem(item);
}

void DialogRenameFiles::on_treeWidget_itemSelectionChanged()
{
    QTreeWidgetItem *curItem=ui->treeWidget->currentItem();
    if(!curItem){
        ui->toolButtonUp->setEnabled(false);
        ui->toolButtonDown->setEnabled(false);
        return;
    }
    int index= ui->treeWidget->indexOfTopLevelItem(curItem);
    if(index== ui->treeWidget->topLevelItemCount()-1){
        ui->toolButtonUp->setEnabled(true);
        ui->toolButtonDown->setEnabled(false);
    }else
    if(index==0){
        ui->toolButtonUp->setEnabled(false);
        ui->toolButtonDown->setEnabled(true);
    }else{
        ui->toolButtonUp->setEnabled(true);
        ui->toolButtonDown->setEnabled(true);
    }

    qDebug()<<"on_treeWidget_itemSelectionChanged";
}
