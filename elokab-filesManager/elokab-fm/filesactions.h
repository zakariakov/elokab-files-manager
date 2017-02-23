#ifndef FILESACTIONS_H
#define FILESACTIONS_H
#include "progressdlg.h"
#include <QObject>
#include <QStringList>
#include <QHash>
class FilesActions : public QObject
{
          Q_OBJECT
     public:
          explicit FilesActions(QObject *parent = 0);

     signals:

     public slots:

          //!
          void removeFiles(const QStringList &files);

          //!
          void pastFiles(const QStringList &files, const QString &distDir);

          //!
          void moveFiles(const QStringList &files, const QString &distDir);

     private slots:

          //!
          void createListForRemove(const QString &path);

          //!
          bool createList(const QStringList &files, const QString &dist);

          //!
          void createLisdOfDirs(const QString &path, const QString &dist);

          //!
          void slotCancel();

          //!
          bool copyFile(const QString &source,const QString &dist);

          //!
          bool cutFile(const QString &source,const QString &dist);

          //!
          QString confirmName(const QString &source, const QString &dist, bool cut=false);

     private:

          //!
          bool mStop;

          //!
          bool mOverWrite;

          //!
          bool mSkyp;

          //!
          bool mApplyAll;

          //!
          bool mApplyAllFolder;

          //!
          bool mSkypFolder;

          //!
          bool mWriteIntoFolder;

          //!
          QStringList mListFiles;

          //!
          QStringList mListDirs;

          //!
          QHash<QString,QString>m_hash;

          //!
          ProgressDlg *progress;

          //!
          qint64 mTotalSize;

};

#endif // FILESACTIONS_H
