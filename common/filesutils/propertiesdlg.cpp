#include "propertiesdlg.h"
#include "ui_propertiesdlg.h"
#include <EMimIcon>
#include <EIcon>
#include "openwithdlg.h"

#include <QFileInfo>
#include <QXmlStreamReader>
#include <QDebug>
#include <QProgressDialog>
#include <QDirIterator>
#include <QTextCodec>
#include <QTimer>
#include <QDateTime>
#include <QListWidgetItem>
#define NO_VALID "Invalidcolor"
PropertiesDlg::PropertiesDlg(const QStringList &urls, QWidget *parent) :
     QDialog(parent),
     ui(new Ui::PropertiesDlg),mOpenWithchanged(false),mPermsChanged(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
     ui->setupUi(this);
     mUrls=urls;
     ui->tabWidget->removeTab(2);
     ui->tabWidget->removeTab(1);
     ui->widgetFolderColor->setVisible(false);
     ui->labelFolder->setPixmap(EIcon::fromTheme("folder").pixmap(48));
     ui->labelInformations->setText(QString::number(mUrls.count()));
     mSize=0;
     mStop=false;
     setMultiFiles(mUrls.count()-1);
     QFileInfo fi=mUrls.at(0);
     if(mUrls.count()>1)
     {
          calculatSelectedFiles(mUrls);
     }
     else if(mUrls.count()==1)
     {
          ui->tabWidget->addTab(ui->tabPermissions,tr("Permissions"));
          QDateTime time=fi.lastModified();
          QDateTime lasttime=fi.lastRead();
          ui->labelModified->setText(time.toString("dd.MM.yyyy hh:mm"));
          ui->labellastRead->setText(lasttime.toString("dd.MM.yyyy hh:mm"));
           mMim=EMimIcon::mimeTyppe(fi);
          ui->labelType->setText(EMimIcon::mimLang(mMim,locale().name().section("_",0,0)));
          mIcon=EMimIcon::icon(fi);
           ui->toolButton->setIcon(mIcon);
          ui->lineEdit->setText(fi.fileName());
          ui->widgetDevice->setVisible(fi.isDir());
          ui->widgetFolderColor->setVisible(fi.isDir());
          ui->labelSymCaption->setVisible(fi.isSymLink());
          ui->labelSymlink->setVisible(fi.isSymLink());
          ui->checkBoxApplyAll->setVisible(fi.isDir());
          ui->checkBoxExexute->setHidden(fi.isDir());

          if(fi.isSymLink())
               ui->labelSymlink->setText(  fi.symLinkTarget());

          if(fi.isDir())
          {
               QHash<QString ,qint64>hash=(EMimIcon::getDriveInfo(fi.absoluteFilePath()));
               int Perc=(hash["Perc"]);
               QString Total=EMimIcon::formatSize(hash["Total"]);
               QString Free=EMimIcon::formatSize(hash["Total"]-hash["Used"]);

               ui->progressBarDevice->setFormat(QString(tr("%1 free Of %2 %p%")).arg(Free).arg(Total));
               ui->progressBarDevice->setValue(Perc);
               setupFolderColor();
          }
          if(fi.isFile()&&!fi.isDir())
          {
               if(mMim!="application/x-executable"){
               ui->tabWidget->addTab(ui->tabOpenWith,tr("Open With"));
               setupOpenWith(mMim);
               }

          }
          getPermission(fi);
     }

     ui->labelLocation->setText(fi.absolutePath());

     qApp->processEvents();

     if(!fi.isRoot()&&fi.absoluteDir().absolutePath()!="/media")
          QTimer::singleShot(100,this,SLOT(calculatMultiFilesSize()));

     this->adjustSize();

}

PropertiesDlg::~PropertiesDlg()
{
     mStop=true;
#ifdef DEBUG_APP
     qDebug()<<"PropertiesDlg::deleted";
#endif
     delete ui;
}

void PropertiesDlg::closeEvent(QCloseEvent *)
{
#ifdef DEBUG_APP
     qDebug()<<"PropertiesDlg::QCloseEvent";
#endif
     mStop=true;

}

void PropertiesDlg::setupOpenWith(const QString &mim)
{

     ui->lineEditMime->setText(mim);

     QStringList list=EMimIcon::associatedApplication(mim);
     QHash<QString ,QVariant> hash;

     foreach (QString f, list)
     {
          QString fp=   EMimIcon::desktopFilePath(f);
          if(fp.isEmpty())
               continue;
          hash=EMimIcon:: desktopFile(fp,this->locale().name().section("_",0,0));
          QListWidgetItem* item=new QListWidgetItem(ui->listWidgetOpenWith);
          item->setText(hash["Name"].toString());
          item->setData(Qt::UserRole,hash["Exec"]);
          item->setData(Qt::ToolTipRole,f);
          item->setIcon(EIcon::fromTheme(hash["Icon"].toString()));
     }

}

void PropertiesDlg::setMultiFiles(bool arg)
{

     ui->widgetInfoOne->setHidden(arg);
     ui->widgetInformations->setVisible(arg);
     ui->widgetOneFile->setHidden(arg);

}

void PropertiesDlg::calculatSelectedFiles(const QStringList &urls)
{
     int folders=0;
     int files=0;

     foreach (QString s, urls)
     {

          QFileInfo fi(s);
          if(fi.isDir())
          {
               folders++;

          }else
          {
               files++;

          }

     }

     ui->labelInformations->setText(QString::number(folders)+tr(" folders, ")+
                                    QString::number(files)+tr(" files, "));

}

void PropertiesDlg::calculatMultiFilesSize()
{

     mSize=0;
     foreach (QString s, mUrls)
     {
          if(mStop)
               return;

          QFileInfo fi(s);
          if(fi.isDir())
          {
               qApp->processEvents();
               getDirSize(s);
          }
          else
          {
               mSize+=fi.size();
          }

     }

     ui->labelSize->setText(EMimIcon::formatSize(mSize));
     mSize=0;
    // mUrls.clear();

}

void PropertiesDlg::getDirSize(QString path)
{

     QDirIterator it( path,QDir::AllEntries |
                     QDir::System |
                     QDir::NoDotAndDotDot |
                     QDir::NoSymLinks |
                     QDir::Hidden, QDirIterator::Subdirectories);

     while(it.hasNext())
     {
          if(mStop)  return;

          it.next();
          QString f=it.filePath();

          if(QFileInfo(f).isDir())
          {
               qApp->processEvents();
               if(mStop) return;

               ui->labelSize->setText(tr("Calculating...")+EMimIcon::formatSize(mSize));
          }
          qApp->processEvents();
          mSize += QFileInfo(f).size();

     }
     //return size;
}

void PropertiesDlg::on_toolButtonUp_clicked()
{
     QListWidgetItem *curItem=ui->listWidgetOpenWith->currentItem();
     if(!curItem)return;

     int index= ui->listWidgetOpenWith->row(curItem);

     if(index==0)return;

     QListWidgetItem *item=new QListWidgetItem;
     item=curItem->clone();

     ui->listWidgetOpenWith->takeItem(index);
     ui->listWidgetOpenWith->insertItem(index-1,item);
     ui->listWidgetOpenWith->setCurrentItem(item);
     mOpenWithchanged=true;
}

void PropertiesDlg::on_toolButtonDown_clicked()
{
     QListWidgetItem *curItem=ui->listWidgetOpenWith->currentItem();
     if(!curItem)return;

     int index= ui->listWidgetOpenWith->row(curItem);
     if(index== ui->listWidgetOpenWith->count()-1)return;
     QListWidgetItem *item=new QListWidgetItem;
     item=curItem->clone();

     ui->listWidgetOpenWith->takeItem(index);
     ui->listWidgetOpenWith->insertItem(index+1,item);
     ui->listWidgetOpenWith->setCurrentItem(item);
     mOpenWithchanged=true;
}

void PropertiesDlg::on_toolButtonAdd_clicked()
{
     OpenWithDlg *openWithDlg=new OpenWithDlg(false);
     openWithDlg->setmim(mMim);
     if(openWithDlg->exec()==QDialog::Accepted)
     {

          QString proc=openWithDlg->execName();

          if(!proc.isEmpty())
          {
               QListWidgetItem* item=new QListWidgetItem(ui->listWidgetOpenWith);
               QString txt=openWithDlg->desktopText();
               if(txt.isEmpty())
                    txt=proc;



               QString dFilename=openWithDlg->desktopFileName();

               item->setText(txt);
               item->setData(Qt::UserRole,proc);
               item->setData(Qt::ToolTipRole,dFilename);
               item->setIcon(openWithDlg->desktopIcon());

               qDebug()<<"PropertiesDlg::on_toolButtonAdd_clicked()"<<txt<<proc<<dFilename;
          }

     }
     delete openWithDlg;
     mOpenWithchanged=true;
}

void PropertiesDlg::on_toolButtonDelete_clicked()
{
     QListWidgetItem *curItem=ui->listWidgetOpenWith->currentItem();
     if(!curItem)return;

     int index= ui->listWidgetOpenWith->row(curItem);
     ui->listWidgetOpenWith->takeItem(index);
     mOpenWithchanged=true;

}

void PropertiesDlg::on_buttonBox_accepted()
{
     if(mOpenWithchanged){
          QStringList list;
          for (int i = 0; i < ui->listWidgetOpenWith->count(); ++i)
          {
              QListWidgetItem *item= ui->listWidgetOpenWith->item(i);
              QString desktopFile=item->data(Qt::ToolTipRole).toString();
              qDebug()<<"PropertiesDlg::on_toolButtonDelete_clicked"<<desktopFile;
              if(desktopFile.isEmpty()){
                  QString proc=item->data(Qt::UserRole).toString();
                  desktopFile=EMimIcon::createDescktopFile(proc);
              }


               list.append(desktopFile);

          }

          if(list.count()>0)
               EMimIcon::setMimeAssociatedApplication(ui->lineEditMime->text(),list);
          qDebug()<<"PropertiesDlg::on_toolButtonDelete_clicked"<<list;
     }

     if(mPermsChanged)
     {
          QString file=mUrls.at(0);
          setPermission(file,ui->checkBoxExexute->isChecked());
      //    chmod(file.toLocal8Bit(),ui->lineEditChmod->text().toInt(0,8));    //convert to octal

          if(ui->checkBoxApplyAll->isChecked())
          {
               QProgressDialog *progress=new QProgressDialog ("Files permissions...", "Abort...", 0, 0, this);
               progress->setWindowModality(Qt::WindowModal);
               connect(progress, SIGNAL(canceled()), this, SLOT(close()));
               progress->show();

               applyPermsToAll(file) ;
               progress->cancel();
          }
     }

     if(ui->groupBoxFolderColor->isChecked()){
         int h=ui->hsHue->value();
         int s=ui->hsSaturation->value();
         int l=ui->hsLightnes->value();

         QColor col=QColor::fromHsl(h,s,l);
         if(col.isValid())
             saveColorFolder(col.name());

     }else{
         if(mFolderColor)
             saveColorFolder(NO_VALID);
     }


}

void PropertiesDlg::applyPermsToAll(const QString &path)
{

     QDirIterator it(path,QDir::AllEntries |
                     QDir::System |
                     QDir::NoDotAndDotDot |
                     QDir::NoSymLinks |
                     QDir::Hidden, QDirIterator::Subdirectories);

     while(it.hasNext())
     {

          it.next();
          QString file=it.filePath();
          //no change exec permission
          setPermission(file,it.fileInfo().isExecutable());
          qApp->processEvents();


     }
     //return size;
}

void PropertiesDlg::getPermission(const QFileInfo &info)
{

     if(info.permission(QFile::WriteUser))
          ui->comboBoxOwner->setCurrentIndex(2);
     else if(info.permission(QFile::ReadUser))
          ui->comboBoxOwner->setCurrentIndex(1);


     if(info.permission(QFile::WriteGroup))
          ui->comboBoxGroup->setCurrentIndex(2);
     else  if(info.permission(QFile::ReadGroup))
          ui->comboBoxGroup->setCurrentIndex(1);


     if(info.permission(QFile::WriteOther))
          ui->comboBoxOther->setCurrentIndex(2);
     else if(info.permission(QFile::ReadOther))
          ui->comboBoxOther->setCurrentIndex(1);


     ui->labelUserName->setText(info.owner());
     ui->labelGroupName->setText(info.group());
     qDebug()<<"owner uid  :"<<info.ownerId();
     qDebug()<<"group uid  :"<<info.groupId();

     ui->checkBoxExexute->setChecked(info.isExecutable());

    ui->widgetPermission->setEnabled(setPermission(info.absoluteFilePath(),info.isExecutable()));
    ui->widgetFolderColor->setEnabled( ui->widgetPermission->isEnabled());

     connect(ui->comboBoxOwner,SIGNAL(currentIndexChanged(int)),this,SLOT(setChmod(int)));
     connect(ui->comboBoxGroup,SIGNAL(currentIndexChanged(int)),this,SLOT(setChmod(int)));
     connect(ui->comboBoxOther,SIGNAL(currentIndexChanged(int)),this,SLOT(setChmod(int)));
     connect(ui->checkBoxExexute,SIGNAL(stateChanged(int)),this,SLOT(setChmod(int)));
     connect(ui->checkBoxApplyAll,SIGNAL(stateChanged(int)),this,SLOT(setChmod(int)));

}

bool PropertiesDlg::setPermission(const QString &path, bool isExecutable)
{

     QFile file(path);
     QFile::Permissions perms;
     if(ui->comboBoxOwner->currentIndex()==1)
          perms=QFile::ReadUser;
     else if(ui->comboBoxOwner->currentIndex()==2)
          perms=QFile::ReadUser|QFile::WriteUser;

     if(ui->comboBoxGroup->currentIndex()==1)
          perms=perms|QFile::ReadGroup;
     else if(ui->comboBoxGroup->currentIndex()==2)
          perms=perms|QFile::ReadGroup|QFile::WriteGroup;

     if(ui->comboBoxOther->currentIndex()==1)
          perms=perms|QFile::ReadOther;
     else if(ui->comboBoxOther->currentIndex()==2)
          perms=perms|QFile::ReadOther|QFile::WriteOther;

     if(isExecutable){
          if(ui->comboBoxOwner->currentIndex()!=0)
               perms=perms|QFile::ExeUser;
          if(ui->comboBoxGroup->currentIndex()!=0)
               perms=perms|QFile::ExeGroup;
          if(ui->comboBoxOther->currentIndex()!=0)
               perms=perms|QFile::ExeOther;
     }
     qDebug()<<"permUser"<<perms;
     return file.setPermissions(perms);
}

void PropertiesDlg::setChmod(int)
{

mPermsChanged=true;
//setPermission(mUrls.at(0));
}

void PropertiesDlg::setupFolderColor()
{
    QString path=mUrls.at(0)+"/.directory";

     QSettings setting(path,QSettings::IniFormat);
     setting.beginGroup("Desktop Entry");
     QString color= setting.value("FolderColor",NO_VALID).toString();
     QColor col(color);
     if(col.isValid()){
         int h=col.hslHue();
         int s=col.saturation();
         int l=col.lightness();
          qDebug()<<"setupFolderColor()"<<h<<s<<l;
         mFolderColor=true;
         ui->groupBoxFolderColor->setChecked(true);
         ui->hsHue->setValue(col.hslHue());
         ui->hsSaturation->setValue(col.saturation());
         ui->hsLightnes->setValue(col.lightness());

     }else{
           mFolderColor=false;
     }
     connect(ui->hsHue,SIGNAL(valueChanged(int)),this,SLOT(setFolderColor()));
     connect(ui->hsSaturation,SIGNAL(valueChanged(int)),this,SLOT(setFolderColor()));
     connect(ui->hsLightnes,SIGNAL(valueChanged(int)),this,SLOT(setFolderColor()));
     connect(ui->groupBoxFolderColor,SIGNAL(toggled(bool)),this,SLOT(toggleIcon(bool)));


}
void PropertiesDlg::toggleIcon(bool checked)
{
    if(checked){
          setFolderColor();
    }else{
        ui->toolButton->setIcon(mIcon);
    }
}
void PropertiesDlg::setFolderColor()
{
   int h=ui->hsHue->value();
   int s=ui->hsSaturation->value();
   int l=ui->hsLightnes->value();

   QIcon icon=EMimIcon::iconColorized(QIcon::fromTheme("folder"),
                                        QColor::fromHsl(h,s,l) );

  ui->toolButton->setIcon(icon);
}

void PropertiesDlg::saveColorFolder(const QString &color)
{

    QString path=mUrls.at(0)+"/.directory";

    QSettings setting(path,QSettings::IniFormat);
    setting.sync();
    setting.beginGroup("Desktop Entry");
    if(color==NO_VALID)
        setting.remove("FolderColor");
    else
        setting.setValue("FolderColor",color);
    qDebug()<<"setting"<<color;
    setting.endGroup();
    qApp->processEvents();
    removePercentEncoding(path);
}

void PropertiesDlg::removePercentEncoding(const QString &path)
{
    qApp->processEvents();
    QFile file(path);
    QTextStream inOut(&file);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
     inOut.setCodec(codec);

    if (!file.open(QIODevice::ReadOnly )) return;
    QString txt = inOut.readAll();
    file.close();

    qApp->processEvents();
    QByteArray encodedString =QByteArray::fromPercentEncoding(txt.toUtf8());
    QString string = codec->toUnicode(encodedString);

    if(!file.open(QIODevice::WriteOnly ))return;
    inOut<<string;
    qDebug()<<"string"<<string;
    file.close();

}

