#ifndef DIALOGACTIONS_H
#define DIALOGACTIONS_H

#include <QDialog>
#include <QListWidgetItem>
namespace Ui {
class DialogActions;
}

/**
 * @brief The DialogActions class فئة خاصة بتخصيص ازرار الاوامر على شريط الادواة
 *  \author    أبو زكريا
 *  \version   3.6.8
 *  \date      2010-2014
 *  \copyright GNU Public License.
 */
class DialogActions : public QDialog
{
          Q_OBJECT

     public:
          explicit DialogActions(QWidget *parent = 0);
          ~DialogActions();


     private:
          Ui::DialogActions *ui;
          /*!< قائمة الاوامر التي ستضاف للوحة*/
          QStringList mListAvailableActions;
     public slots:

          /**
          * @brief setListActions  قوائم الاوامر
          * @param listActions الاوامر المتاحة في البرنامج
          * @param listToAdd القوائم المستخدمة في اللوحة حاليا
          */
          void setListActions( QList<QAction *>listActions,QStringList listToAdd);

          /**
          * @brief getListActToAdd جلب قائمة الاوامر
          * @return قائمة بالاوامر المخصصة
          */
          QStringList listAvailableActions(){return mListAvailableActions;}
     private slots:
          void slotItemPressed(QListWidgetItem* item);
          /*!< النقر على زر موافق */
          void on_buttonBox_accepted();
          /*!< عند تغير التحديد في شجرة الاوامر التي ستضاف للوحة */
          void destItemSelectionChanged();
          /*!< عند تغير التحديد في شجرة الاوامرالمتاحة */
          void sourceItemSelectionChanged();
          /*!< النقر على زر نقل للاعلى */
          void slotActionUp();
          /*!< النقر على زر نقل للاعلى */
          void slotActionDown();
          /*!< النقر على زر اضافى امر */
          void slotAddAction();
          /*!< النقر على زر حذف الامر */
          void slotRemoveAction();

};

#endif // DIALOGACTIONS_H
