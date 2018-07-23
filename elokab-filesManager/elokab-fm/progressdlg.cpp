#include "progressdlg.h"
#include "ui_progressdlg.h"

//___________________________________________
ProgressDlg::ProgressDlg(QWidget *parent) :
     QDialog(parent),
     ui(new Ui::ProgressDlg)
{
     ui->setupUi(this);
     ui->progressBarCopy->setVisible(false);
}

//___________________________________________
ProgressDlg::~ProgressDlg()
{
     delete ui;
}

//___________________________________________
void ProgressDlg::setCopyVisible(bool arg)
{
     ui->progressBarCopy->setVisible(arg);
}

//___________________________________________
void ProgressDlg::setInfoText(const QString &txt)
{
     ui->labelInfo->setText(txt);
}

//___________________________________________
void ProgressDlg::setValue( int value)
{
     ui->progressBar->setValue(value);
}

//___________________________________________
void ProgressDlg::setCopyValue( int value)
{
     ui->progressBarCopy->setValue(value);
}

//___________________________________________
void ProgressDlg::setMaximum( int max)
{
     if(max==1)
          ui->progressBar->setVisible(false);
     ui->progressBar->setMaximum(max);
}

//___________________________________________
void ProgressDlg::on_buttonBox_rejected()
{
     emit canceled();
}
