#include "dialogrenamefiles.h"
#include "ui_dialogrenamefiles.h"

DialogRenameFiles::DialogRenameFiles(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRenameFiles)
{
    ui->setupUi(this);
}

DialogRenameFiles::~DialogRenameFiles()
{
    delete ui;
}
