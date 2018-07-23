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

#ifndef CONFIRMDLG_H
#define CONFIRMDLG_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ConfirmDlg;
}

class ConfirmDlg : public QDialog
{
          Q_OBJECT
          
     public:

          //!
          explicit ConfirmDlg(const QString &source, const QString &dest, bool cut=false, QWidget *parent = 0);

          //!
          ~ConfirmDlg();

          //!
          enum Actions{
               ReName,
               OverWrite,
               Skyp,
               WriteIntoFolder,
               SkypFolder
          };

     public slots:

          //!
          QString getName();

          //!
          Actions getAction(){return mAction;}

          //!
          bool isApplyToAll();

     private slots:

          //!
          void nameChanged(const QString &txt);

          //!
          void renameAccepted();

          //!
          void overWriteAccepted();

          //!
          void skypAccepted();

          //!
          void writeIntoAccepted();

     private:

          Ui::ConfirmDlg *ui;

          //!
          QPushButton *skypButton;

          //!
          QPushButton *renameButton;

          //!
          QPushButton *overWriteButton;

          //!
          QPushButton *writeIntoButton;

          //!
          Actions mAction;

          //!
          QString mOldName;

          //!
          QString mPath;

          //!
          bool mIsdir;

};

#endif // CONFIRMDLG_H
