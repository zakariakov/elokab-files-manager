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

#ifndef PATHWIDGET_H
#define PATHWIDGET_H

#include <QWidget>
#include <QToolButton>
#include <QHBoxLayout>
#include <QMenu>
#include<QFileSystemModel>
#include<QCompleter>
#include<QToolBar>

/****************************************************
 *                     PathWidget
 ***************************************************/
/**
 *
 */
namespace Ui {
class PathWidget;
}
/**
 * @brief The PathWidget class ادات تحرير المسار
 */
class PathWidget : public QWidget
{
          Q_OBJECT

     public:
          /**
     * @brief PathWidget
     * @param url
     * @param parent
     */
          explicit PathWidget(QWidget *parent = nullptr);

          ~PathWidget();
void changeEvent(QEvent *event);
     protected:


          //!
          void  resizeEvent(QResizeEvent *);

     signals:


          //! عند تغير المسار
          void  urlChanged(const QString &url);

     public slots:


          //! setUrl تلقي المسار
          void setUrl(const QString &url);

     private slots:

void changeStyleSheet();
          //! عند الضغط على ادات النص
          void on_lineEdit_returnPressed();


          //! انشاء روابط للمجلدات بقوائم للمجلدات الفرعية
          void setupToolUrl(const QString &url);

          //!
      //    QMenu * menuDirs(QDir dir);


          //!  تنفيذ الامر
          void goActionPath();

          /*!< اذا كانت اللوحة ظاهرة سيتم لنشاء الاوامر */
          void toolbarVisible(bool checked);

          void adjustActions();
     private:

          Ui::PathWidget *ui;

          /*!< قائمة بالاوامر */
        //  QList<QAction *>listActions;
          QActionGroup   *actionsGroup ;

          /*!< قائمة بالقوائم */
        //  QList<QMenu *>listmenus;

          /*!< لوحة الادوات للاوامر */
          QToolBar *mToolBar;

          /*!< قائمة الجذر */
          QMenu *mMenu;

          /*!< امر الجذر */
        //  QAction *mactRoot;

          /*!< موديول التكميل التلقائي */
          QFileSystemModel *fsModel ;

          /*!< التكميل التلقائي */
          QCompleter  *completer ;
  QString mColor;
};

#endif // PATHWIDGET_H
