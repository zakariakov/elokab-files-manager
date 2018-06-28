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

#include "openwithdlg.h"
#include "ui_openwithdlg.h"
#include <EIcon>
#include <EDir>
#include <DesktopFile>
#include <MenuGen>

#ifdef DEBUG_APP
#include <QDebug>
#endif
#include <QCompleter>
#include <QDomDocument>
#include <QToolButton>
#include <QFileDialog>
OpenWithDlg::OpenWithDlg(bool remember, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenWithDlg)
{
    ui->setupUi(this);
    ui->checkBoxConfirm->setVisible(remember);
    //  mRemember=remember;
    //    connect(ui->treeWidget,SIGNAL(itemPressed(QTreeWidgetItem*,int)),this,SLOT(slotItemPressed(QTreeWidgetItem*,int)));
    connect(ui->treeWidget,SIGNAL(itemSelectionChanged()),this,SLOT(slotitemSelectionChanged()));

    connect(ui->lineEdit,SIGNAL(textEdited(QString)),this,SLOT(slotExecChanged(QString)));
    connect(ui->toolButton,SIGNAL(clicked()),this,SLOT(getOpenFile()));

    chargeXmlDocument();
    QCompleter *completer = new QCompleter(mListCompliter, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit->setCompleter(completer);
}

OpenWithDlg::~OpenWithDlg()
{
    delete ui;
}
void OpenWithDlg::slotitemSelectionChanged()
{
    QTreeWidgetItem *item=ui->treeWidget->currentItem();
    if(item){
        ui->lineEdit->setText(item->data(1,1).toString());
        mDesktopFileName=item->data(2,2).toString();
        mDesktopText=item->text(0);
        mDesktopIcon=item->icon(0);
        mExecName=item->data(1,1).toString();
    }
    //   qDebug()<<"slotitemSelectionChanged()";
}
void OpenWithDlg::slotExecChanged(const QString &txt)
{
    mExecName=txt;
    mDesktopFileName="";
    mDesktopText="";
    mDesktopIcon=EIcon::fromTheme("application-x-executable");
    //  qDebug()<<"textChanged()"<<txt;
}


void OpenWithDlg::chargeTreeWidgetFillBack()
{
    QStringList list;
    list<<trUtf8("Islamic")<<trUtf8("Office")<<trUtf8("AudioVideo")<<trUtf8("Graphics")
       <<trUtf8("Game")<<trUtf8("Development")<<trUtf8("Utility")
      <<trUtf8("Network")<<trUtf8("Settings")<<trUtf8("System")
     <<trUtf8("Other");
    enum  { NumIndex = 11};
    QTreeWidgetItem *item[NumIndex];
    for(int i=0 ;i<list.count();i++){
#ifdef DEBUG_APP
        qDebug()<<"treewidjet :"<<i;
#endif

        item[i]=new  QTreeWidgetItem(ui->treeWidget);
        item[i]->setText(0,list.at(i));
    }
    ui->treeWidget->topLevelItem(0)->setIcon(0,EIcon::fromTheme("applications-islamic"));
    ui->treeWidget->topLevelItem(1)->setIcon(0,EIcon::fromTheme("applications-office"));
    ui->treeWidget->topLevelItem(2)->setIcon(0,EIcon::fromTheme("applications-multimedia"));
    ui->treeWidget->topLevelItem(3)->setIcon(0,EIcon::fromTheme("applications-graphics"));
    ui->treeWidget->topLevelItem(4)->setIcon(0,EIcon::fromTheme("applications-games"));
    ui->treeWidget->topLevelItem(5)->setIcon(0,EIcon::fromTheme("applications-development"));
    ui->treeWidget->topLevelItem(6)->setIcon(0,EIcon::fromTheme("applications-utilities"));
    ui->treeWidget->topLevelItem(7)->setIcon(0,EIcon::fromTheme("applications-internet"));
    ui->treeWidget->topLevelItem(8)->setIcon(0,EIcon::fromTheme("applications-accessories"));
    ui->treeWidget->topLevelItem(9)->setIcon(0,EIcon::fromTheme("applications-system"));
    ui->treeWidget->topLevelItem(10)->setIcon(0,EIcon::fromTheme("applications-other"));

    QFile file(Edir::configDir()+"/applictions");
    // if(!file.exists()){
    MenuGen *gen=new MenuGen(this->locale().name().section("_",0,0));
    delete gen;
    //  }
    if (!file.open(QFile::ReadOnly)){
#ifdef DEBUG_APP
        qDebug()<<file.fileName()+" \n  "+file.errorString();
#endif
        return ;
    }
    QTextStream textStream(&file);
    QString line;

    line= textStream.readLine();//premier line
    QString lc=this->locale().name().section("_",0,0);
    while (!line.isNull()) {
        DesktopFile dsg(line.section("|",0,0),"Desktop Entry",lc);

        QString oldname=dsg.value("Name").toString();
        QString name=dsg.value("Name["+lc+"]",oldname).toString();
        QString exec=dsg.value("Exec").toString();
        QString category=dsg.value("Categories","Other").toString();
        QString icon=dsg.value("Icon").toString();




        QTreeWidgetItem *item=new QTreeWidgetItem;
        item->setData(1,1,exec);
        mListCompliter.append(exec);
        item->setData(2,2,QFileInfo(line.section("|",0,0)).fileName());
        item->setText(0,name);
        item->setIcon(0,EIcon::fromTheme(icon));

        if(category.contains("Islamic")){ ui->treeWidget->topLevelItem(0)->addChild(item); }

        else  if(category.contains("Office")) {ui->treeWidget->topLevelItem(1)->addChild(item);}

        else  if(category.contains("AudioVideo")) {ui->treeWidget->topLevelItem(2)->addChild(item);}

        else  if(category.contains("Graphics"))   {ui->treeWidget->topLevelItem(3)->addChild(item);}

        else if(category.contains("Game"))        {ui->treeWidget->topLevelItem(4)->addChild(item);}

        else if(category.contains("Development")) {ui->treeWidget->topLevelItem(5)->addChild(item);}

        else if(category.contains("Utility"))     {ui->treeWidget->topLevelItem(6)->addChild(item);}

        else  if(category.contains("Network"))    {ui->treeWidget->topLevelItem(7)->addChild(item);}

        else if(category.contains("Settings"))    {ui->treeWidget->topLevelItem(8)->addChild(item);}

        else if(category.contains("System"))      {ui->treeWidget->topLevelItem(9)->addChild(item);}

        else {   ui->treeWidget->topLevelItem(10)->addChild(item);}


        line= textStream.readLine();
    }
    file.close();
    for(int i=0;i<ui->treeWidget->topLevelItemCount();i++){
        if(ui->treeWidget->topLevelItem(i)->childCount()<1)
            ui->treeWidget->topLevelItem(i)->setHidden(true);
    }

}

//________________________________________________________________________________________
void OpenWithDlg::chargeXmlDocument()
{

    QString xmlPath=Edir::configHomeDir()+"/menus/elokab-applications.menu";
    if(!QFile::exists(xmlPath)){
        QDir appDir(qApp->applicationDirPath());
        appDir.cdUp();
        appDir.cdUp();
        xmlPath=  appDir.absolutePath()+"/etc/xdg/menus/elokab-applications.menu";

    }

    QFile file(xmlPath);



    if (!file.open(QIODevice::ReadOnly)){
#ifdef DEBUG_APP
        qDebug()<<file.errorString()<<xmlPath;
#endif
        chargeTreeWidgetFillBack();
        return;
    }
    if (!m_doc.setContent(&file)) {
#ifdef DEBUG_APP
        qDebug()<<"error xmlContent";
#endif
        file.close();
        chargeTreeWidgetFillBack();
        return;
    }
    file.close();

    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomElement docElem = m_doc.documentElement();

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement menu = n.toElement(); // try to convert the node to an element.
        if(!menu.isNull()) {
            //   qDebug()<<menu.tagName(); // the node really is an element.
            if(menu.tagName()=="Menu"){
                QString name=   menu.firstChildElement("Name").text();
                QString directory=  menu.firstChildElement("Directory").text();
                QString category;

                QDomNode Include = menu.firstChildElement("Include");
                QDomNode And = Include.firstChildElement("And");
                QDomElement Cat = And.toElement();
                if(!Cat.isNull()) {
                    category=   Cat.firstChildElement("Category").text();

                }


                addMenuDirectory(name,directory,category);
            }
        }
        n = n.nextSibling();
    }
    chargeAppDesktop();
}

//___________________________________________________________________________________
void OpenWithDlg::addMenuDirectory(QString name, QString directory,QString category)
{

    QString directoryPath=Edir::dataHomeDir()+"/desktop-directories/"+directory;
    if(!QFile::exists(directoryPath)){
        QDir appDir(qApp->applicationDirPath());
        appDir.cdUp();

        directoryPath=  appDir.absolutePath()+"/share/desktop-directories/"+directory;

    }
    if(!QFile::exists(directoryPath)){
        directoryPath= "/usr/share/desktop-directories/"+directory;

    }
    QString lc=this->locale().name().section("_",0,0);
    // QSettings settings(directoryPath,QSettings::IniFormat);
    DesktopFile settings(directoryPath,"Desktop Entry",lc);
    // settings.setIniCodec("UTF-8");
    // settings.beginGroup("Desktop Entry");
    QString textOld=settings.value("Name",name).toString();
    QString text=settings.value("Name["+lc+"]",textOld).toString();
    QString commentOld=settings.value("Comment").toString();
    QString comment=settings.value("Comment["+lc+"]",commentOld).toString();
    QString icon=settings.value("Icon").toString();

    // settings.endGroup();

    QTreeWidgetItem *item=new QTreeWidgetItem(ui->treeWidget);
    item->setText(0,text);
    item->setToolTip(0,comment);
    item->setData(1,Qt::UserRole,category);
    item->setData(0,Qt::UserRole,name);
    // item->setText(1,category);
    if(QFile::exists(icon)){
        item->setIcon(0,QIcon(icon));
    }else{
        QIcon icnF=EIcon::fromTheme(icon);
        item->setIcon(0,icnF);
    }
}

//________________________________________________________________________________
void OpenWithDlg::chargeAppDesktop()
{

    QFile file(Edir::configDir()+"/applictions");

    if(!file.exists()){
        MenuGen *gen=new MenuGen(this->locale().name().section("_",0,0));
        delete gen;
    }
    if (!file.open(QFile::ReadOnly)){
#ifdef DEBUG_APP
        qDebug()<<file.fileName()+" \n  "+file.errorString();
#endif
        return ;
    }
    QTextStream textStream(&file);
    QString line;
    QString lc=this->locale().name().section("_",0,0);
    line= textStream.readLine();//premier line

    while (!line.isNull()) {
        DesktopFile dsg(line.section("|",0,0),"Desktop Entry",lc);

        QString oldname=dsg.value("Name").toString();
        QString name=dsg.value("Name["+lc+"]",oldname).toString();
        QString exec=dsg.value("Exec").toString();
        QString category=dsg.value("Categories","Other").toString();
        QString icon=dsg.value("Icon").toString();


        QString file=line.section("|",0,0);

        QTreeWidgetItem *item=new QTreeWidgetItem;
        item->setData(1,1,exec);
        mListCompliter.append(exec);
        item->setData(2,2,QFileInfo(file).fileName());

        item->setText(0,name);

        if(QFile::exists(icon)){
            item->setIcon(0,QIcon(icon));
        }else{
            QIcon icnF=EIcon::fromTheme(icon);

            item->setIcon(0,icnF);
        }



        if(QFile::exists(icon)){
            item->setIcon(0,QIcon(icon));
        }else{
            QIcon icnF=EIcon::fromTheme(icon);

            item->setIcon(0,icnF);
        }



        for(int i=0;i<ui->treeWidget->topLevelItemCount();i++){
            QString cat=ui->treeWidget->topLevelItem(i)->data(1,Qt::UserRole).toString();
            if(category.contains(cat))
                ui->treeWidget->topLevelItem(i)->addChild(item);
        }


        line= textStream.readLine();
    }
    file.close();

    for(int i=0;i<ui->treeWidget->topLevelItemCount();i++){

        if(ui->treeWidget->topLevelItem(i)->childCount()==0)
            ui->treeWidget->takeTopLevelItem(i);
    }

}

//___________________________________________________________________________________
bool OpenWithDlg::addAssociation()
{
    return ui->checkBoxConfirm->isChecked();
}

//_____________________________________________________________________________________
void OpenWithDlg::getOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    QDir::homePath(),
                                                    tr("All files (*)"));
    if(!fileName.isEmpty()){
        ui->lineEdit->setText(fileName);
        slotExecChanged(fileName)   ;
    }
}

