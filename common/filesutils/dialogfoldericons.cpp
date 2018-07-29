#include "dialogfoldericons.h"
#include "ui_dialogfoldericons.h"
#include "QDebug"

DialogFolderIcons::DialogFolderIcons(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFolderIcons)
{
    ui->setupUi(this);

    QStringList list;
     list<<"folder"
    <<"folder-black"      <<"folder-red"      <<"folders-publicshare"
    <<"folder-blue"       <<"folder-brown"    <<"folder-cyan"
    <<"folder-green"      <<"folder-grey"     <<"folder-magenta"
    <<"folder-orange"     <<"folder-yellow"   <<"folder-home"
    <<"folder-images"     <<"folder-music"    <<"folder-downloads"
    <<"folder-dropbox"    <<"folder-hdd"      <<"folder-pictures"
    <<"folder-art"        <<"folder-cloud"    <<"folder-document"
    <<"folder-documents"  <<"folder-recent"   <<"folder-remote"
    <<"folder-videos"     <<"folder-projects" <<"folder-network"
    <<"folder-system"     <<"folder-templates"<<"folder-publicshare";


    list.sort();
    foreach (QString name, list) {
        if(!QIcon::hasThemeIcon(name))
            continue;
        qDebug()<<name;
       QListWidgetItem *item=new QListWidgetItem(ui->listWidget);
        item->setIcon(QIcon::fromTheme(name).pixmap(64,64));
        item->setData(Qt::UserRole,name);
       item->setText(name);
    }
}

DialogFolderIcons::~DialogFolderIcons()
{
    delete ui;
}

void DialogFolderIcons::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem */**previous*/)
{
    mIconName=current->text();
}

void DialogFolderIcons::on_listWidget_itemActivated(QListWidgetItem *item)
{
    mIconName=item->text();
    accept();
}
