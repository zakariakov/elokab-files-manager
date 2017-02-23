#ifndef TRASHVIEW_H
#define TRASHVIEW_H
#include "iconprovider.h"
#include "actions.h"
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
          explicit TrashView(IconProvider *iconProvider,
                             Actions *action,
                             QWidget *parent = 0);

     signals:
          void fileSelected(const QString &txt);

     public slots:

          //!
          void updateTrashFiles();

          //!
          void moveFilesToTrash(const QStringList &list);

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

          /*!< مدير الايقونات */
          IconProvider *iconP;

          /*!< مراقب تغير الملفات */
          QFileSystemWatcher *fsWatcher;

          /*!< الاوامر */
          Actions *mActions;
};

#endif // TRASHVIEW_H
