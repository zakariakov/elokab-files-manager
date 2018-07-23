/***************************************************************************
 *   elokab Copyright (C) 2014 AbouZakaria <yahiaui@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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

          //!
          void moveFilesToTrash(const QStringList &list);

     private slots:
void creatSymsLinks();
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

          //!
          bool trashExists();

          //!
          void moveFileToTrash(const QString &file);

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
QHash<QString,QString>m_hashSymLink;
          //!
          ProgressDlg *progress;

          //!
          qint64 mTotalSize;

          /*!< مجلد معلومات المهملات */
          QString TrashPathInfo;

          /*!< مجلد الملفات المحذوفة */
          QString TrashPathFiles;

};

#endif // FILESACTIONS_H
