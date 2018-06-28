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

#include "widgetabout.h"
#include "ui_widgetabout.h"

WidgetAbout::WidgetAbout(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetAbout)
{
    ui->setupUi(this);
    ui->labelAuthor->setText(trUtf8("Abou zakaria"));
    ui->labelLisence->setText(trUtf8("Gpl3"));
    ui->labelVersion->setText(QApplication::applicationVersion());
    ui->labelAppName->setText(QApplication::applicationName());
    ui->textEditThinks->setText( tr(
                                     "thanks to:"
                                     "<ul>"
                                     "<li>Linux arab community (http://www.linuxac.org)</li>"
                                     "<li>Muhammad Shaban (Mr.Muhammad@outlook.com)</li>"
                                     "<li>Kovax (kovax3@gmail.com)</li>"
                                     "<li>Ahmed soudani (http://www.qt-ar.org)</li>"
                                     "<li>Amine Roukh  (amineroukh@gmail.com)</li>"
                                     "<li>RazorQt (http://www.razor-qt.org)</li>"
                                     "<li>KDE (http://www.kde.org)</li>"
                                     ));
    connect(ui->pushButtonAbotQt,SIGNAL(clicked()),qApp,SLOT(aboutQt()));
}

WidgetAbout::~WidgetAbout()
{
    delete ui;
}

void  WidgetAbout::setPixmap(const QPixmap &pix)
{
    ui->labelPixmap->setPixmap(pix.scaled(64,64));
}

void  WidgetAbout::setDescription(const QString &txt)
{
    ui->labelDescription->setText(txt);
}

void  WidgetAbout::setApplicationText(const QString &txt)
{
    if(txt.isEmpty()){
        ui->labelAppName->setText(QApplication::applicationName());
    }else{
        ui->labelAppName->setText(txt);
    }
}
