#include "dialogfoldericons.h"
#include "ui_dialogfoldericons.h"

DialogFolderIcons::DialogFolderIcons(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFolderIcons)
{
    ui->setupUi(this);
}

DialogFolderIcons::~DialogFolderIcons()
{
    delete ui;
}
