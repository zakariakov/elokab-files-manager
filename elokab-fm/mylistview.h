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
    explicit MyListView(QFileSystemModel *model, Actions *action, QWidget *parent = nullptr);

    /**
     * @brief selectedIndexes
     * @return
     */
    QModelIndexList selectedIndexes() const;
    virtual void startDrag(Qt::DropActions supportedActions);
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dragMoveEvent(QDragMoveEvent* e);
    virtual void dragLeaveEvent(QDragLeaveEvent* e);
    virtual void dropEvent(QDropEvent* e);

    void  setdDoubleClickTriggers(bool arg);

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
