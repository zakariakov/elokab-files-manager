#ifndef PROGRESSDLG_H
#define PROGRESSDLG_H

#include <QDialog>

namespace Ui {
class ProgressDlg;
}
/**
 * @brief The ProgressDlg class مؤشر تقدم النسخ واللصق
 */
class ProgressDlg : public QDialog
{
          Q_OBJECT

     public:

          explicit ProgressDlg(QWidget *parent = 0);
          ~ProgressDlg();

     signals:

          /*!< مؤشر الانهاء من قبل المستخدم */
          void canceled();

     public slots:

          /*!< لعرض معلومات على النافذة */
          void setInfoText(const QString &txt);

          /*!< لعرض مؤشر نسخ الملفات */
          void setCopyVisible(bool arg);

          /*!< مؤشر عدد الملفات حتى الان */
          void  setValue( int value);

          /*!< مؤشر حجم النسخ  */
          void setCopyValue( int value);

          /*!< عدد الا جمالي للملفات */
          void setMaximum( int max);

     private slots:

           //!
          void on_buttonBox_rejected();

     private:

          Ui::ProgressDlg *ui;
};

#endif // PROGRESSDLG_H
