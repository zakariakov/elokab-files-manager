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

#ifndef OPENWITHDLG_H
#define OPENWITHDLG_H

#include <QDialog>

#include <QDomDocument>
#include <QSettings>
#include <QTreeWidgetItem>
namespace Ui {
class OpenWithDlg;
}

class OpenWithDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit OpenWithDlg(bool remember=true,QWidget *parent = nullptr);
    ~OpenWithDlg();
    QString execName(){return mExecName;}
    QString desktopFileName(){return mDesktopFileName;}
    QString desktopText(){return mDesktopText;}
    QIcon desktopIcon(){return mDesktopIcon;}
public slots:
    void   setmim(const QString mim){mMim=mim;}
    bool addAssociation();
private slots:
    void slotitemSelectionChanged();
    void chargeTreeWidgetFillBack();
    void chargeXmlDocument();
    void addMenuDirectory(QString name, QString directory,QString category);
    void chargeAppDesktop();
    void slotExecChanged(const QString &txt);
    //  void on_buttonBox_accepted();

    void getOpenFile();
  //  QString createDescktopFile(const QString &exec);
   // void on_buttonBox_accepted();

private:
    Ui::OpenWithDlg *ui;
    QDomDocument m_doc;
    QString  mExecName;
    QString mDesktopFileName;
    QIcon mDesktopIcon;
    QString mDesktopText;
    QStringList mListCompliter;
    QString mMim;
  //  bool mRemember;
};

#endif // OPENWITHDLG_H
