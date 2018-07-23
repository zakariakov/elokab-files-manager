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
