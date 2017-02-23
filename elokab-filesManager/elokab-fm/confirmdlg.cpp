#include "confirmdlg.h"
#include "mimetypexdg.h"
#include "ui_confirmdlg.h"
#include  <QFileInfo>
#include  <QDateTime>
#ifdef DEBUG_APP
#include  <QDebug>
#endif
/****************************************************************************
 *
 ****************************************************************************/
ConfirmDlg::ConfirmDlg(const QString &source, const QString &dest,bool cut, QWidget *parent) :
     QDialog(parent),
     ui(new Ui::ConfirmDlg)
{
#ifdef DEBUG_APP
     qDebug()<<"=============== ConfirmDlg::ConfirmDlg()==================\n"
            <<"source :"<<source
            <<"dest :"<<dest
            <<"cut :"<<cut
            <<"\n==========================================================\n";
#endif
     ui->setupUi(this);
     QFileInfo fiSource(source);
     QFileInfo fiDest(dest);

     skypButton=new QPushButton(tr("Skyp"));
     renameButton=new QPushButton(tr("Rename"));
     overWriteButton=new QPushButton(tr("OverWrite"));
     writeIntoButton=new QPushButton(tr("Write Into"));

     renameButton->setEnabled(false);

     ui->buttonBox->addButton(skypButton,QDialogButtonBox::NoRole);
     ui->buttonBox->addButton(renameButton,QDialogButtonBox::NoRole);

     mPath=fiDest.path();
     mOldName=fiDest.fileName();


     mIsdir=fiSource.isDir();
     if(mIsdir)
          setWindowTitle(tr("Folder Already Exists"));
     else
          setWindowTitle(tr("File Already Exists"));

     if(fiSource.path()!=fiDest.path()&&cut==false)
     {



          if(mIsdir)
               ui->buttonBox->addButton(writeIntoButton,QDialogButtonBox::NoRole);
          else
               ui->buttonBox->addButton(overWriteButton,QDialogButtonBox::NoRole);


          QDateTime timeS=fiSource.lastModified();
          QDateTime timeD=fiDest.lastModified();

          QString infoS=QString(tr("Source:\n %1 \n "
                                   "Size: %2 \n"
                                   "Modified: %3 \n"
                                   ).
                                arg(source).
                                arg(MimeTypeXdg::formatSize(fiSource.size())).
                                arg(timeS.toString("dd.MM.yyyy hh:mm")));
          QString infoD=QString(tr("Destination:\n %1 \n "
                                   "Size: %2 \n"
                                   "Modified: %3 \n"
                                   ).
                                arg(dest).
                                arg(MimeTypeXdg::formatSize(fiDest.size())).
                                arg(timeD.toString("dd.MM.yyyy hh:mm")));

          ui->labelSource->setText(infoS);
          ui->labelDest->setText(infoD);
          ui->labelInfo->setText(tr("This action will overwrite the destination:"));

     }else{

          ui->labelInfo->setText(QString(tr("This action would overwrite %1:")).arg(dest));
     }

     ui->lineEditRename->setText(fiDest.fileName());
     connect(ui->lineEditRename,SIGNAL(textChanged(QString)),this,SLOT(nameChanged(QString)));
     connect(renameButton,SIGNAL(clicked()),this,SLOT(renameAccepted()));
     connect(overWriteButton,SIGNAL(clicked()),this,SLOT(overWriteAccepted()));
     connect(skypButton,SIGNAL(clicked()),this,SLOT(skypAccepted()));
     connect(writeIntoButton,SIGNAL(clicked()),this,SLOT(writeIntoAccepted()));

     int idx= ui->lineEditRename->text().indexOf(".");
     if(idx==-1)idx=ui->lineEditRename->text().length();
     ui->lineEditRename->setSelection(0,idx);

#ifdef DEBUG_APP
     qDebug()<<"END============ ConfirmDlg::ConfirmDlg()===============END\n";

#endif
}

/****************************************************************************
 *
 ****************************************************************************/
ConfirmDlg::~ConfirmDlg()
{

#ifdef DEBUG_APP

     qDebug()<<"ConfirmDlg deleted========================================\n";
#endif
     this->blockSignals(true);
     ui->lineEditRename->blockSignals(true);
     delete ui;
}

/****************************************************************************
 *
 ****************************************************************************/
void ConfirmDlg::nameChanged(const QString &txt)
{
     renameButton->setEnabled(txt!=mOldName);
     renameButton->setFocus();
}

/****************************************************************************
 *
 ****************************************************************************/
void ConfirmDlg::renameAccepted()
{
     mAction=ReName;
     this->accept();
}

/****************************************************************************
 *
 ****************************************************************************/
void ConfirmDlg::overWriteAccepted()
{
     mAction=OverWrite;
     this->accept();
}

/****************************************************************************
 *
 ****************************************************************************/
void ConfirmDlg::writeIntoAccepted()
{
     mAction=WriteIntoFolder;
     this->accept();
}

/****************************************************************************
 *
 ****************************************************************************/
void ConfirmDlg::skypAccepted()
{
     if(mIsdir)
          mAction=SkypFolder;
     else
          mAction=Skyp;

     this->accept();
}

/****************************************************************************
 *
 ****************************************************************************/
bool ConfirmDlg::isApplyToAll()
{
     return ui->checkBoxApplyAll->isChecked();
}

/****************************************************************************
 *
 ****************************************************************************/
QString ConfirmDlg::getName()
{
     return mPath+"/"+ui->lineEditRename->text().trimmed();
}
