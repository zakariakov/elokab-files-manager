/***************************************************************************
 *   Copyright (C) 2006-2010 by Peter Penz <peter.penz19@gmail.com>        *
 *   Copyright (C) 2006 by Gregor Kališnik <gregor@podnapisi.net>          *
 *   Copyright (C) 2012 by Stuart Citrin <ctrn3e8@gmail.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#ifndef FILTERBAR_H
#define FILTERBAR_H
#include <QWidget>
#include <QRadioButton>

class QLineEdit;
class QToolButton;

class FilterBar : public QWidget
{
          Q_OBJECT

     public:

          //!
          explicit FilterBar(QWidget* parent = 0);

          //!
          virtual ~FilterBar();

          //!
          void closeFilterBar();

          //!
          void closeEvent(QCloseEvent *);

          //!
          void selectAll();

     public slots:

          //!
          void clear();


     signals:

          //!
          void filterChanged(const QString& nameFilter);

          //!
          void closeRequest();

          //!
          void focusViewRequest();

     protected:

          //!
          virtual void showEvent(QShowEvent* event);

          //!
          virtual void keyReleaseEvent(QKeyEvent* event);

     private slots:

          //!
          void setTextFilter(const QString &txt);

     private:

          //!
          QLineEdit* m_filterInput;

          //!
          QToolButton* m_clearButton;

          //!
          QRadioButton *rdbAny;

          //!
          QRadioButton *rdbStart;

          //!
          QRadioButton *rdbEnd;
};

#endif
