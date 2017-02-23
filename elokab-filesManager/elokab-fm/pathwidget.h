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
          explicit PathWidget(QWidget *parent = 0);

          ~PathWidget();

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


          //! عند الضغط على ادات النص
          void on_lineEdit_returnPressed();


          //! انشاء روابط للمجلدات بقوائم للمجلدات الفرعية
          void setupToolUrl(const QString &url);

          //!
          QMenu * menuDirs(QDir dir);


          //!  تنفيذ الامر
          void goActionPath();

          /*!< اذا كانت اللوحة ظاهرة سيتم لنشاء الاوامر */
          void toolbarVisible(bool checked);

     private:

          Ui::PathWidget *ui;

          /*!< قائمة بالاوامر */
          QList<QAction *>listActions;

          /*!< قائمة بالقوائم */
          QList<QMenu *>listmenus;

          /*!< لوحة الادوات للاوامر */
          QToolBar *mToolBar;

          /*!< قائمة الجذر */
          QMenu *mMenu;

          /*!< امر الجذر */
          QAction *mactRoot;

          /*!< موديول التكميل التلقائي */
          QFileSystemModel *fsModel ;

          /*!< التكميل التلقائي */
          QCompleter  *completer ;

};

#endif // PATHWIDGET_H
