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

#include "filterbar.h"

#include <QBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QToolButton>

#include <EIcon>
#include <QLineEdit>

/*****************************************************************************************************
 *
 *****************************************************************************************************/
FilterBar::FilterBar(QWidget* parent) :
     QWidget(parent)
{
     // Create close button
     QToolButton *closeButton = new QToolButton(this);
     closeButton->setAutoRaise(true);
     closeButton->setIcon(EIcon::fromTheme("window-close","dialog-close"));
     closeButton->setToolTip(tr("Hide Filter Bar"));
     connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

     // Create button to lock text when changing folders
     m_clearButton = new QToolButton(this);
     m_clearButton->setAutoRaise(true);
     //  m_lockButton->setCheckable(true);
     m_clearButton->setIcon(EIcon::fromTheme("edit-clear-list","edit-clear"));
     m_clearButton->setToolTip(tr( "clear filter"));
     connect(m_clearButton, SIGNAL(clicked()), this, SLOT( clear()));

     // Create label
     QLabel* filterLabel = new QLabel(tr( "Filter:"), this);

     // Create filter editor
     m_filterInput = new QLineEdit(this);
     m_filterInput->setLayoutDirection(Qt::LeftToRight);
     //m_filterInput->setClearButtonShown(true);
     connect(m_filterInput, SIGNAL(textChanged(QString)),
             this, SLOT(setTextFilter(QString)));
     setFocusProxy(m_filterInput);
     rdbAny=new QRadioButton(tr("Contains"),this);
     rdbStart=new QRadioButton(tr("Start With"),this);
     rdbEnd=new QRadioButton(tr("End With"),this);
     rdbAny->setChecked(true);
     // Apply layout
     QHBoxLayout* hLayout = new QHBoxLayout(this);
     hLayout->setMargin(0);
     hLayout->addWidget(closeButton);

     hLayout->addWidget(filterLabel);
     hLayout->addWidget(m_filterInput);

     hLayout->addWidget(m_clearButton);
     hLayout->addWidget(rdbAny);
     hLayout->addWidget(rdbStart);
     hLayout->addWidget(rdbEnd);
     filterLabel->setBuddy(m_filterInput);
}

/**************************************************************************************
 *
 **************************************************************************************/
FilterBar::~FilterBar()
{
}
void FilterBar::setTextFilter(const QString &txt)
{
     if(rdbAny->isChecked())
          emit filterChanged("*"+txt+"*");
     // emit filterChanged(txt);
     else if(rdbStart->isChecked())
          emit filterChanged(txt+"*");
     else if(rdbEnd->isChecked())
          emit filterChanged("*"+txt);
}

/**************************************************************************************
 *
 **************************************************************************************/
void FilterBar::closeEvent(QCloseEvent *)
{
     closeFilterBar();
}

/**************************************************************************************
 *
 **************************************************************************************/
void FilterBar::closeFilterBar()
{

     clear();
     hide();

}

/**************************************************************************************
 *
 **************************************************************************************/
void FilterBar::selectAll()
{
     m_filterInput->selectAll();
}

/**************************************************************************************
 *
 **************************************************************************************/
void FilterBar::clear()
{
     m_filterInput->clear();
     emit filterChanged("*");
}

/**************************************************************************************
 *
 **************************************************************************************/
void FilterBar::showEvent(QShowEvent* event)
{
     if (!event->spontaneous()) {
          m_filterInput->setFocus();
     }
}

/**************************************************************************************
 *
 **************************************************************************************/
void FilterBar::keyReleaseEvent(QKeyEvent* event)
{
     QWidget::keyReleaseEvent(event);

     switch (event->key()) {
          case Qt::Key_Escape:
               m_filterInput->clear();
               emit filterChanged("*");
               break;

          case Qt::Key_Enter:
          case Qt::Key_Return:
               emit focusViewRequest();
               break;

          default:
               break;
     }
}

//#include "filterbar.moc"
