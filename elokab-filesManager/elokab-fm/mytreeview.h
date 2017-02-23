#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include <QTreeView>

#include <QFileSystemModel>
/**
 * @brief The MyTreeView class
 *
 */
class MyTreeView : public QTreeView
{
          Q_OBJECT
     public:
          /**
     * @brief MyTreeView
     * @param model
     * @param parent
     */
          explicit MyTreeView(QFileSystemModel *model,QWidget *parent = 0);

          //!
          QModelIndexList selectedIndexes() const;
     signals:

     public slots:

          //!
          QStringList selectedFiles();

          //!
          void setRootPath(const QString &url);

          //!
          void   setTreeIconSize(int size) ;

          //!
          void setExpandable(bool show){setRootIsDecorated(show);}

     private:

          /*!< cmment */
          QFileSystemModel *mModel;
};

#endif // MYTREEVIEW_H
