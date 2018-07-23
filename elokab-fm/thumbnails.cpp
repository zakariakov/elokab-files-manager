#include "thumbnails.h"
#include "defines.h"
#include <QFileInfo>
#include <QDebug>
#include <QtConcurrent>
#include <QImage>
#include <QMessageAuthenticationCode>
#include <EMimIcon>
#include <QPainter>
#include <QImageReader>

QString codeMd5(QString fileName)
{

    QMessageAuthenticationCode code(QCryptographicHash::Md5);
    code.addData(fileName.toLatin1());
    return code.result().toHex();
}

//***********************  Thumbnails ***********************

Thumbnails::Thumbnails(QObject *parent) : QObject(parent)
{
    canReadPdf=EMimIcon::findProgram("convert");
    //canReadPdf=EMimIcon::findProgram("pdfimages");
    canReadVideo=EMimIcon::findProgram("ffmpeg");

    mThread=new Thread;
    // connect(mThread,SIGNAL(canceled(QString)),this,SLOT(cancel(QString)));
    connect(mThread,SIGNAL(terminated(QString)),this,SIGNAL(updateThumbnail(QString)));
    connect(mThread,SIGNAL(finished()),this,SLOT(startNewThread()));

}

Thumbnails::~Thumbnails()
{

   delete mThread;

}
//_____________________________________________________________
void Thumbnails::directoryChanged(const QString &path)
{
    // qDebug()<<__FILE__<<__FUNCTION__<<mCurentPath<<path;
    if(mCurentPath!=path){
        mCurentPath=path;

        while (myMap.count()>10) {
            QString   filename = myMap.firstKey();
            myMap.remove(filename);
        }
          //  myMap.clear();

     }

}

//_____________________________________________________________
void Thumbnails::addFileName(const QFileInfo &info,const QString &type)
{

     qDebug()<<__FILE__<<__FUNCTION__<<info.fileName()<<type;

     while (myMap.count()>50) {
         QString   filename = myMap.firstKey();
         myMap.remove(filename);
     }

       myMap[info.filePath()]=type;
    //myMap.insert(myMap.constBegin() ,info.filePath(),type);
    if(!mThread->isRunning())
    { startRender(); }

}

//_____________________________________________________________
void Thumbnails::startNewThread()
{
    myMap.remove(mThread->curentPath());
   // qDebug()<<__FILE__<<"finiched>>>>>>>>>>"<<mThread->curentPath();
    startRender();
}

//_____________________________________________________________
void Thumbnails::startRender()
{
    if(myMap.count()<1) {return;}
    if(mThread->isRunning()){return;}
    // isRunning=true;
    QString   filename = myMap.firstKey();
    QString   type = myMap.first();
    QFileInfo info(filename);

    // qDebug()<<__FILE__<<__FUNCTION__<<filename<<type;

    if(type==_PDF_TYPE && !canReadPdf) {
        {myMap.remove(info.filePath()); return;} }

    if(type==_VIDEO_TYPE && !canReadVideo){
        {myMap.remove(info.filePath()); return;} }

    mThread->setFile(info,type);
    mThread->start();
}


//***********************  THREAD ******************************

void Thread::run()
{
    if(mType==_IMAGE_TYPE)  { createImageThumbnail(); }

    if(mType==_PDF_TYPE  )  { createPdfThumbnail();   }

    if(mType==_VIDEO_TYPE)  { createVideoThumbnail(); }

}

//***************************************************************
//*                  Generate Images thumbnails                 *
//***************************************************************
void Thread::createImageThumbnail()
{
    //qDebug()<<__FILE__<<__FUNCTION__<<mInfo.filePath();


    QImageReader reader(mInfo.filePath());
    if(!reader.canRead()){  return;}

    if(qMax(reader.size().width(),reader.size().height())<=128){ return;}

    QString codeName=codeMd5(mInfo.filePath());

    QString thumbnail=Edir::thumbnaileCachDir();
    QString fileThumbnail=thumbnail+"/"+codeName;

    //qDebug()<<"creatthumb"<<fi.filePath();
    if(QFile::exists(fileThumbnail)){

        reader.setFileName(fileThumbnail);
        if(reader.canRead()){
            QString  fModified=reader.text(_KEY_DATETIME);
            if(fModified== mInfo.lastModified().toString("dd MM yyyy hh:mm:ss"))
            { return;}
        }

    }

    QImage image;
    if( image.load(mInfo.filePath()))
    {
        // qDebug()<<"saveImageThumb"<<__LINinfo_<<fi.filePath();
        image= image.scaled(QSize(128,128),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        image.setText(_KEY_DATETIME,mInfo.lastModified().toString("dd MM yyyy hh:mm:ss"));

            QByteArray text=mInfo.filePath().toUtf8();
       //text.toHex()
           image.setText(_KEY_FILEPATH,text.toHex());

        QByteArray format="jpg";
        if(image.hasAlphaChannel())
            format="png";

        if(image.save(fileThumbnail,format,50))   {
            qDebug()<<__FILE__<<__FUNCTION__<<"image saved"<<mInfo.fileName();
            emit terminated(mInfo.filePath());

        }

    }

    //---------------------end

}

//***************************************************************
//*                  Generate PDF thumbnails                    *
//***************************************************************
void Thread::createPdfThumbnail()
{

    qDebug()<<__FILE__<<__FUNCTION__<<mInfo.filePath();
    // if(!canReadPdf) {cancel(mInfo.filePath()); return;}
    //-----------------------------------------------

    QString mthumbnail=Edir::thumbnaileCachDir();

    QString md5Name=codeMd5(mInfo.filePath());

    QString fileThumbnail=mthumbnail+"/"+md5Name;

    if(QFile::exists(fileThumbnail)){
        QImageReader reader(fileThumbnail);
        if(reader.canRead()){
            qDebug()<<__FILE__<<__FUNCTION__<<"exist"<<fileThumbnail;
            return ;
        }
    }

    // if(!QFile::exists(fileThumbnail+"pdf-000.png")){
    if(!QFile::exists(fileThumbnail+".jpg")){
       // qDebug()<<__FUNCTION__<<">> start prossec"<<mInfo.fileName();
        QProcess p;
        QStringList list;

        list<<"-thumbnail"<<"x128"
           <<"-quality"<<"50"
          <<mInfo.filePath()+"[0]"<<fileThumbnail+".jpg";
        p.start("convert",list);

        //          list<<"-l"<<"1"<<"-png"<<info.filePath()<<fileThumbnail+"pdf";
        //          p.start("pdfimages",list);
        if (!p.waitForStarted()) {  return ; }

        if (!p.waitForFinished()){  return ; }

        qDebug()<<"error:"<<p.readAllStandardOutput();
       // qDebug()<<__FUNCTION__<<">> finish prossec"<<mInfo.fileName();

    }

    //QString name=fileThumbnail+"pdf-000.png";
    QString name=fileThumbnail+".jpg";

    QImage imagePdf;
    if( imagePdf.load(name))
    {

        //qDebug()<<__FUNCTION__<<">> load image "<<mInfo.fileName();
        // imagePdf=imagePdf.scaled(QSize(128,128),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        QFile::remove(name);
        QImage imageIco;
        imageIco.load(":/icons/x-pdf.svg");
        QPainter p(&imagePdf);
        p.drawImage(0,0,imageIco);
        imagePdf.setText(_KEY_DATETIME,mInfo.lastModified().toString("dd MM yyyy hh:mm:ss"));
        imagePdf.setText(_KEY_FILEPATH,mInfo.filePath());

        if(imagePdf.save(fileThumbnail,"jpg",90))   {
            qDebug()<<__FILE__<<__FUNCTION__<<"pdf saved"<<mInfo.fileName();
            emit terminated(mInfo.filePath());

        }// image.save

    }

    //---------------------end

}

//***************************************************************
//*                  Generate VIDEO thumbnails                 *
//***************************************************************
void Thread::createVideoThumbnail()
{

    // qDebug()<<__FILE__<<__FUNCTION__<<mInfo.filePath();
    // if(!canReadVideo){cancel(mInfo.filePath()); return;}

    if(mListExclude.contains(mInfo.filePath())) { return;}


    QString thumbnail=Edir::thumbnaileCachDir();
    QString md5Name=codeMd5(mInfo.filePath());
    QString fileThumbnail=thumbnail+"/"+md5Name;

    //qDebug()<<"creatthumb"<<fi.filePath();
    if(QFile::exists(fileThumbnail)){
        // qDebug()<<__FUNCTION__<<"exist"<<fileThumbnail;
        return ;
    }

    if(!QFile::exists(fileThumbnail+".video")){

      QString pos=  videoInfo();
        QStringList list;
        list<<"-i"<<mInfo.filePath()<<"-n"<<"-t"<<"1"<<"-r"<<"1"
           <<"-ss"<<pos<<"-s"<<"128x128"<<"-f"<<"image2"<<fileThumbnail+".video";

        QStringList list2;
        list2<<"-i"<<mInfo.filePath();

        QProcess p;
         //-------------------------------------------

        //-------------------------------------------
        p.start("ffmpeg",list);

        if (!p.waitForStarted()) {   return ;  }

        if (!p.waitForFinished()){   return ;  }

        QString err=p.readAllStandardError();
        if(err.contains("not contain any stream"))
        {mListExclude.append(mInfo.filePath());}

//        if(!err.isEmpty())
//            qDebug()<<__FUNCTION__<<">> error: "<<err;

    }

    QImage imagevideo;
    if( imagevideo.load(fileThumbnail+".video"))
    {

        QImage imageIcon;
        imageIcon.load(":/icons/video.svg");
        QPainter p(&imagevideo);
        p.drawImage(32,32,imageIcon);
        imagevideo.setText(_KEY_DATETIME,mInfo.lastModified().toString("dd MM yyyy hh:mm:ss"));
        imagevideo.setText(_KEY_FILEPATH,mInfo.filePath());

        if(imagevideo.save(fileThumbnail,"jpg",50))   {
            qDebug()<<__FILE__<<__FUNCTION__<<"video saved"<<mInfo.fileName();
            emit terminated(mInfo.filePath());

        }

    }

    //---------------------end

}

QString Thread::videoInfo()
{
    QStringList list2;
    list2<<"-i"<<mInfo.filePath();
    QString ret="5.0";
    QProcess p;
     //-------------------------------------------
    p.start("ffmpeg",list2);
    if (!p.waitForStarted()) {   return ret ;  }

    if (!p.waitForFinished()){   return ret;   }

     QString error=p.readAllStandardError();

     if(error.isEmpty())return ret;

     QStringList list=error.split("\n");

     foreach (QString s, list) {
         if(s.trimmed().startsWith("Duration")){
             s=s.remove("Duration:");
             QString name=s.section(",",0,0);
             QStringList listtime=name.trimmed().split(":");
             if(listtime.count()==3){
                 QString h=listtime.at(0);
                 QString m=listtime.at(1);
                 QString s=listtime.at(2);
                 if     (h.toFloat()>0)    ret= "10.0";
                 else if(m.toFloat()>0)    ret=    ret;
                 else if(s.toFloat()<=0.9) ret=  "0.1";
                 else if(s.toFloat()<=5.0) ret=  "1.0";
                 else if(s.toFloat()<=10.0)ret=  "3.0";
                 else if(s.toFloat()>10.0) ret=    ret;
                 qDebug()<<ret;
                 return ret;
             }

         }

     }
 return ret;

}
