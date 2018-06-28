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


#ifndef DIALOGXDESKTOP_H
#define DIALOGXDESKTOP_H

#include <QDialog>


namespace Ui {
class DialogXdesktop;
}

/**
 * @brief The DialogXdesktop class
 * @brief انشاء اختصار سطح المكنب
 * @author أبو زكريا
 * @date 19/10/2014
 */
class DialogXdesktop : public QDialog
{
        Q_OBJECT
        
public:
    /**
         * @brief DialogXdesktop
         * @param path مسار سطخ المكتب
         * @param parent
         */
        explicit DialogXdesktop(const QString &path, QWidget *parent = 0);
        ~DialogXdesktop();
        
private:
        Ui::DialogXdesktop *ui;
     QString m_path;
     QString m_iconName;
public slots:
//static  QString createDescktopFile(const QString &exec,bool disply);
private slots:
        void saveDesktopApp();
// static void removeUnicodeSpace(const QString &path);
        void on_toolButtonIcon_clicked();
        void on_pushButton_clicked();
        void on_lineEditIconName_textChanged(const QString &arg1);
        void on_buttonBox_accepted();
};

#endif // DIALOGXDESKTOP_H
