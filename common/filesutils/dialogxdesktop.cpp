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

#include "dialogxdesktop.h"
#include "ui_dialogxdesktop.h"
#include <EIcon>

#include <QFileDialog>
#include <QSettings>
#include <QInputDialog>
#include <QTextCodec>
#include <QTextStream>
DialogXdesktop::DialogXdesktop(const QString &path,QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogXdesktop), m_path(path)
{
        ui->setupUi(this);
        ui->lineEditFileName->setText(tr("Link to Application"));
        ui->toolButtonIcon->setIcon(EIcon::fromTheme("application-x-desktop"));
        ui->lineEditIconName->setText("application-x-desktop");
        m_iconName="application-x-desktop";
}

DialogXdesktop::~DialogXdesktop()
{
        delete ui;
}
void DialogXdesktop::saveDesktopApp()
{
        if(ui->lineEditFileName->text().isEmpty())
        return;

        QString fileName=m_path+"/"+ui->lineEditFileName->text()+".desktop";
        if(QFile::exists(fileName)){
                bool ok;
                QString newName = QInputDialog::getText(0, tr("New Desktop File"),
                                                        tr("Enter name:"), QLineEdit::Normal,
                                                        ui->lineEditFileName->text(), &ok);

                if (ok && !newName.isEmpty()){
                        QFile file(m_path+"/"+newName+".desktop");
                        if(file.exists()){
                              saveDesktopApp();
                        }else{
                              fileName=m_path+"/"+newName+".desktop";

                        }

                }

        }



/*
  QString lC=this->locale().name().section("_",0,0);
        QSettings settings(fileName,QSettings::IniFormat);

       settings.setIniCodec(QTextCodec::codecForLocale());

        settings.beginGroup("Desktop Entry");
        settings.setValue("Type","Application");
        settings.setValue("Name",ui->lineEditName->text());
        settings.setValue("Name["+lC+"]",ui->lineEditName->text());
        settings.setValue("Comment",ui->lineEditComment->text());
        settings.setValue("Description",ui->lineEditDescription->text());
        settings.setValue("Icon",m_iconName);
        settings.setValue("Exec",ui->lineEditCommand->text());
        settings.endGroup();
        */
//----------------------------------------------------------------
        QFile fileS(fileName);

        if (! fileS.open(QIODevice::WriteOnly ))
            return ;

        QTextStream out(&fileS);
        out.setCodec(QTextCodec::codecForName("UTF-8"));
        out<<"[Desktop Entry]"<< endl;;
        out<<"Type=Application"<< endl;
        out<<"Name="<<ui->lineEditName->text()<< endl;
        out<<"Comment="<<ui->lineEditComment->text()<< endl;
        out<<"Description="<<ui->lineEditDescription->text()<< endl;
        out<<"Icon="<<m_iconName<< endl;
        out<<"Exec="<<ui->lineEditCommand->text()<< endl;

        fileS.close();
//---------------------------------------------------------------------
m_path=fileName;
//removeUnicodeSpace(fileName);
}

void DialogXdesktop::on_toolButtonIcon_clicked()
{
        QFileDialog dlg;
//".png", ".svg", ".svgz", ".svg.gz", ".xpm"
        QString fn = dlg.getOpenFileName(this, tr("Open Images Files..."),
                                         QDir::homePath(), trUtf8("Image File (*.png *.svg *.svgz *.svg.gz *.xpm);;All Files (*)"));
        if (!fn.isEmpty())
        {
                  ui->toolButtonIcon->setIcon(QIcon(fn));
                  ui->lineEditIconName->setText(fn);
                  m_iconName=fn;

        }
}

void DialogXdesktop::on_pushButton_clicked()
{
        QFileDialog dlg;

        QString fn = dlg.getOpenFileName(this, tr("Open Files..."),
                                         QDir::homePath(), trUtf8("All Files (*)"));
        if (!fn.isEmpty())
        {
                  ui->lineEditCommand->setText(fn);

        }
}

void DialogXdesktop::on_lineEditIconName_textChanged(const QString &arg1)
{
        QIcon icon;
        icon=EIcon::fromTheme(arg1);
        if(QFile::exists(arg1)){
            icon=QIcon(arg1);
             ui->toolButtonIcon->setIcon(icon);
               m_iconName=arg1;
        }else if(!icon.isNull()){

             ui->toolButtonIcon->setIcon(icon);
               m_iconName=arg1;
        }
}

void DialogXdesktop::on_buttonBox_accepted()
{
    saveDesktopApp();
}
//_____________________________________________________________________
