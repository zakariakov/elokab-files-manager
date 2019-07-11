#include "dialogaddaction.h"
#include "ui_dialogaddaction.h"

DialogAddAction::DialogAddAction(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddAction)
{
    ui->setupUi(this);
}

DialogAddAction::~DialogAddAction()
{
    delete ui;
}
