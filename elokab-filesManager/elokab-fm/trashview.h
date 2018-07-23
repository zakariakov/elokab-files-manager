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

#ifndef TRASHVIEW_H
#define TRASHVIEW_H

//#include "actions.h"
#include <QTreeWidget>
#include <QFileSystemWatcher>
/**
 * @brief The TrashView class
 */
class TrashView : public QTreeWidget
{
          Q_OBJECT

     public:

          /**
     * @brief TrashView
     * @param action
     * @param parent
     */
          explicit TrashView(/*Actions *action,*/
                             QWidget *parent = 0);

     signals:
          void fileSelected(const QString &txt);

     public slots:

          //!
          void updateTrashFiles();

          //!
          void moveFilesToTrash(const QStringList &list);

          //!
          void deleteFiles(const QStringList &list);
          //!
          void restorFiles(const QStringList &list);

          //!
          void cleanTrash();

     private slots:

          //!
          QStringList readDesktopFile(const QString &fileName);

          //!
          void directoryChanged(QString);

          //!
          void clearTrash();

          //!
          void deleteFile();

          //!
          void restoreFiles();

          //!
          void costumMenu(QPoint);

          //!
          void moveFileToTrash(const QString &file);

          //!
          void itemSelectionChanged();

     private:

          /*!< تغير ملفات المهملات */
          bool TrashFilesChanged ;

          /*!< مجلد المهملات */
          QString TrashPath;

          /*!< مجلد معلومات المهملات */
          QString TrashPathInfo;

          /*!< مجلد الملفات المحذوفة */
          QString TrashPathFiles;


          /*!< مراقب تغير الملفات */
          QFileSystemWatcher *fsWatcher;

          /*!< الاوامر */
       //   Actions *mActions;
};

#endif // TRASHVIEW_H
