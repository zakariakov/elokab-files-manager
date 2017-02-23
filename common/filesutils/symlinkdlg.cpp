#include "symlinkdlg.h"
#include "ui_symlinkdlg.h"
#include <EIcon>
#include <QFile>
#include <QFileDialog>
#include <QDebug>
SymLinkDlg::SymLinkDlg(const QString &path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SymLinkDlg),mPath(path)
{
    qDebug()<<"SymLinkDlg========================"<<mPath;
     setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    ui->toolButtonDir->setIcon(EIcon::fromTheme("folder-open","folder"));
    ui->toolButtonFile->setIcon(EIcon::fromTheme("file",("unknown")));
    connect(ui->toolButtonDir,SIGNAL(clicked()),this,SLOT(creatLinkDir()));
    connect(ui->toolButtonFile,SIGNAL(clicked()),this,SLOT(creatLinkFile()));
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(buttonBoxAccepted()));
}

SymLinkDlg::~SymLinkDlg()
{
     qDebug()<<"SymLinkDlg========================deleted"<<mPath;
    delete ui;

}

/*****************************************************************************************************
 *                          *انشاء اختصار الى ملف
 *****************************************************************************************************/
void SymLinkDlg::creatLinkFile()
{


     QString fileName = QFileDialog::getOpenFileName(0, tr("Open File"),
                                                     mPath,
                                                     tr("All file (*)"));
 if(fileName.isEmpty())return;


 ui->lineEditFilPath->setText(fileName);

}

/*****************************************************************************************************
 *                        *انشاء اختصار الى مجلد
 *****************************************************************************************************/
void SymLinkDlg::creatLinkDir()
{


     QString fileName = QFileDialog::getExistingDirectory(0, tr("Open Directory "),
                                                          mPath);

     if(fileName.isEmpty())return;

     ui->lineEditFilPath->setText(fileName);


}
/*****************************************************************************************************
 *                        *انشاء اختصار
 *****************************************************************************************************/

bool SymLinkDlg::creatLink()
{


    QString fileName=ui->lineEditFilPath->text();
    QString newName=ui->lineEditFileName->text();
    if ( newName.isEmpty())
        return false;
    if ( fileName.isEmpty())
        return false;


         QFile file(mPath+"/"+newName);
         if(file.exists())
             return false;

         if(!QFile::link(fileName,mPath+"/"+newName))
             return false;


  return true;

}
void SymLinkDlg::buttonBoxAccepted()
{
    if(creatLink())
        accept();
}
