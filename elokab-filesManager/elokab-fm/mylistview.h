#ifndef MYLISTVIEW_H
#define MYLISTVIEW_H
#include "actions.h"
#include <QListView>
#include<QFileSystemModel>
/**
 * @brief The MyListView class
 */
class MyListView : public QListView
{
    Q_OBJECT
public:
    /**
     * @brief MyListView
     * @param model
     * @param parent
     */
    explicit MyListView(QFileSystemModel *model, Actions *action, QWidget *parent = 0);

    /**
     * @brief selectedIndexes
     * @return
     */
    QModelIndexList selectedIndexes() const;

signals:
    /**
   * @brief selectedAvailabe
   */
    void  selectedAvailabe(bool);
   // void curDirSelected(const QString &);
   void setUrlDir(const QString &);
    void fileEntered(const QString &);
public slots:

    /**
     * @brief selectedFiles
     * @return
     */
    QStringList selectedFiles();

    /**
     * @brief setRootPath
     * @param url
     */
    void setRootPath(const QString &url);

    /**
     * @brief startEdit
     */
    void startEdit();

    void   setViewIconSize(int size) ;

     //!
     void setListViewMode(ViewMode mode);
private slots:

    /**
     * @brief onItemEntered
     * @param index
     */
    void onItemEntered(QModelIndex index);

     //!
    void viewEntered();

    /**
     * @brief setFileRenamed
     * @param path
     * @param old
     * @param newN
     */
    void setFileRenamed(QString path,QString old,QString newN);

private:

     //!
    QFileSystemModel *mModel;

     //!
     Actions *mActions;

    //QItemSelectionModel *listSelectionModel;
};

#endif // MYLISTVIEW_H
