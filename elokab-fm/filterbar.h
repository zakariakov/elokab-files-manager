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
          explicit FilterBar(QWidget* parent = nullptr);

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
