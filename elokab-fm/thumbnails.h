#ifndef THUMBNAILS_H
#define THUMBNAILS_H

#include <QObject>
#include <QMap>
#include <QFileInfo>
#include <QThread>
//*********************THREAD**************************
class Thread : public QThread
{
    Q_OBJECT

public:
    Thread(){}

    void setFile(const QFileInfo &info,const QString &type){mInfo=info;mType=type;}
    QString curentPath(){return mInfo.filePath();}

signals:
    void terminated(const QString &path);
    void excluded(const QString &path);


protected:
    void run();

private:
    QFileInfo   mInfo;
    QString     mType;


    void createImageThumbnail();
    void createPdfThumbnail();
    void createVideoThumbnail();
    QMap<QString,QString> videoInfo();
};

//*********************THUMBNAILS**************************
class Thumbnails : public QObject
{
    Q_OBJECT
public:
    explicit Thumbnails(QObject *parent = nullptr);
    //!
   ~Thumbnails();
signals:
    void updateThumbnail(const QString &path);

public slots:
    void addFileName(const QFileInfo &info, const QString &type);
    void directoryChanged(const QString &path);

private slots:
    void startRender();
    void startNewThread();
    void setLisExclude(const QString &path){mListExclude.append(path);}

private:
    Thread   *mThread;

     QStringList mListExclude;

    QString   mCurentPath;
    QString   mCurType;

    bool      canReadPdf;
    bool      canReadVideo;

    QMap<QString,QString>myMap;


};

#endif // THUMBNAILS_H
