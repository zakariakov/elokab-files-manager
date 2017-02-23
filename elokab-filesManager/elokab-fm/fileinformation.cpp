#include "fileinformation.h"
#include "ui_fileinformation.h"
#include <EMimIcon>
#include "messages.h"
#include <QFileInfo>
#include <QDateTime>
//#ifdef DEBUG_APP
//#include <QDebug>
//#endif
/**************************************************************************************
 *                                  FILEINFORMATION
 **************************************************************************************/
FileInformation::FileInformation(QWidget *parent) :
     QWidget(parent),
     ui(new Ui::FileInformation)
{

#ifdef DEBUG_APP
     Messages::showMessage(Messages::TOP,"FileInformation::FileInformation()");
#endif


     ui->setupUi(this);
     setContextMenuPolicy(Qt::CustomContextMenu);

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"FileInformation::FileInformation()");
#endif
}

/**************************************************************************************
 *                                  FILEINFORMATION
 **************************************************************************************/
FileInformation::~FileInformation()
{
     delete ui;
}

/**************************************************************************************
 *                                  FILEINFORMATION
 **************************************************************************************/
void FileInformation:: showEvent ( QShowEvent * /*event*/ )
{
     setFileName(mFile);
}


/**************************************************************************************
 *                                  FILEINFORMATION
 **************************************************************************************/
void FileInformation::setFileName(const QString &file)
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"FileInformation::setFileName()");
#endif
     mFile=file;
     if(!this->isVisible())
          return;



#ifdef DEBUG_APP
     Messages::showMessage(Messages::NORMALE,"FileInformation::setFileName()","file:"+file);
#endif



     if(mFile.startsWith("file://"))
     {
         mFile.remove("file://");
         QFileInfo fi(mFile);

         QString txt=fi.fileName();
         int pointSize=this->font().pointSize();
         int lent=txt.length();

         int width=200/pointSize;
         if(lent>width){
             int pos=0;
             while (pos<lent) {
                 int index=txt.indexOf(" ",pos);
                 pos=width+pos;
                 if(index==-1||(index-pos)>width)
                     txt.insert(pos,"\n")  ;
             }
         }

         ui->labelTitle->setText(txt);
         if(fi.isDir())
             setDirInformation(fi);
         else
             setFileInformation(fi);



     }else{
          ui->labelTitle->setText("");
          ui->labelInfo->setText(file);
          ui->labelPixmap->setPixmap(EIcon::fromTheme("help-info").pixmap(128));
     }

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"FileInformation::setFileName()");
#endif
}

/**************************************************************************************
 *                                 DIRSINFORMATION
 **************************************************************************************/
void FileInformation::setDirInformation(const QFileInfo &fi)
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"FileInformation::setDirInformation()");
#endif

     QDateTime time=fi.lastModified();
 //    QIcon icon=MimeTypeXdg::iconFolder(mFile);
//     if(fi.isSymLink())
//     {
//          ui->labelPixmap->setPixmap(MimeTypeXdg::iconSymLink(icon).pixmap(128).scaled(128,128));
//     }else{
//          ui->labelPixmap->setPixmap(icon.pixmap(128).scaled(128,128));
//     }
     QIcon icon=EMimIcon::icon(fi,false);
     ui->labelPixmap->setPixmap(icon.pixmap(128).scaled(128,128));

     QString sym;
     if(fi.isSymLink())
              sym=tr("Point To: ")+fi.symLinkTarget()+"\n";
     QString info=QString(tr("\n %7"
                              "Type: %6 \n "
                             "Size: %1 \n"
                             "Modified: %2 \n"
                             "\n"
                             "User Permission: %3 %4 %5")).
               arg(getDirSize(mFile)).
               arg(time.toString("dd.MM.yyyy hh:mm")).
               arg(fi.permission(QFile::ReadUser) ? "r" : "-").
               arg(fi.permission(QFile::WriteUser) ? "w" : "-").
               arg(fi.permission(QFile::ExeUser) ? "x" : "-").
               arg(EMimIcon::mimLang("inode/directory",locale().name().section("_",0,0))).
               arg(sym)  ;

     ui->labelInfo->setText(info);

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"FileInformation::setDirInformation()");
#endif
}

/**************************************************************************************
 *                                  FILEINFORMATION
 **************************************************************************************/
void FileInformation::setFileInformation(const QFileInfo &fi)
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"FileInformation::setFileInformation()");
#endif

     QString mim=EMimIcon::mimeTyppe(fi);
     if(mim.startsWith("image"))
     {
          setImageInformation(fi,mim);
     }else{
  QDateTime time=fi.lastModified();
          QIcon icon=EMimIcon::icon(fi,false);
          ui->labelPixmap->setPixmap(icon.pixmap(128).scaled(128,128));
          QString sym;
          if(fi.isSymLink())
              sym=tr("Point To: ")+fi.symLinkTarget()+"\n";

          QString info=QString(tr("\n %7"
                                   "Type: %1 \n "
                                  "Size: %2 \n"
                                  "Modified: %3 \n"
                                  "\n"
                                  "User Permission: %4 %5 %6")).
                    arg(EMimIcon::mimLang(mim,locale().name().section("_",0,0))).
                    arg(EMimIcon::formatSize(fi.size())).
                    arg(time.toString("dd.MM.yyyy hh:mm")).
                    arg(fi.permission(QFile::ReadUser) ? "r" : "-").
                    arg(fi.permission(QFile::WriteUser) ? "w" : "-").
                    arg(fi.permission(QFile::ExeUser) ? "x" : "-").
                    arg(sym)  ;



          ui->labelInfo->setText(info);
     }

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"FileInformation::setFileInformation()");
#endif
}

/**************************************************************************************
 *                                 IMAGEINFORMATION
 **************************************************************************************/
void FileInformation::setImageInformation(const QFileInfo &fi,const QString &mim)
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"FileInformation::setImageInformation()");
#endif

     QDateTime time=fi.lastModified();
     QPixmap pix(mFile);
     int w=0;
     int h=0;

     if(pix.isNull()){

          QIcon icon=EMimIcon::icon(fi);
          ui->labelPixmap->setPixmap(icon.pixmap(128).scaled(128,128));

     }else{
          int scal=128;
          w=pix.width();
          h=pix.height();

          if(w>=200||h>=200)
               scal=200;
          else if(w<=128||h<=128)
               scal=128;
          else
               if(w>=h)
                    scal=w;
               else
                    scal=h;

          ui->labelPixmap->setPixmap(QPixmap(pix.scaled(QSize(scal,scal),Qt::KeepAspectRatio)));
     }
     QString sym;
     if(fi.isSymLink())
         sym=tr("Point To: ")+fi.symLinkTarget()+"\n";
     QString info=QString(tr("\n %6"
                              "Type: %1 \n "
                             "Size: %2 \n"
                             "Modified: %3 \n \n "
                             "width: %4  \n"
                             "height: %5")).
               arg(EMimIcon::mimLang(mim,locale().name().section("_",0,0))).
               arg(EMimIcon::formatSize(fi.size())).
               arg(time.toString("dd.MM.yyyy hh:mm")).
               arg(w).
               arg(h).
               arg(sym)  ;

     ui->labelInfo->setText(info);

#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"FileInformation::setImageInformation()");
#endif
}

/**************************************************************************************
 *                                 SIZEINFORMATION
 **************************************************************************************/
QString FileInformation::getDirSize(const QString &path)
{
#ifdef DEBUG_APP
     Messages::showMessage(Messages::BEGIN,"FileInformation::getDirSize()");
#endif

     QDir dir(path);
     int folder=0,file=0;

     foreach (QString subfile, dir.entryList(QDir::AllEntries|  QDir::NoDotAndDotDot|QDir::Hidden))
     {
          QFileInfo fi(path+"/"+subfile);

          if(fi.isDir())
               folder++;
          else
               file++;

     }
#ifdef DEBUG_APP
     Messages::showMessage(Messages::END,"FileInformation::getDirSize()");
#endif
     return (QString::number(folder)+tr(" sub-folders ")+QString::number(file)+ tr(" files"));

     //return size;
}
