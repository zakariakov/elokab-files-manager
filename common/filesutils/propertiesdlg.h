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

#ifndef PROPERTIESDLG_H
#define PROPERTIESDLG_H

#include <QDialog>
#include <QHash>
#include <QFileInfo>
#include <QToolButton>
namespace Ui {
class PropertiesDlg;
}

class PropertiesDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit PropertiesDlg(const QStringList &urls,QWidget *parent = nullptr);
    ~PropertiesDlg();
signals:
    void finiched();
private:
    Ui::PropertiesDlg *ui;
qint64 mSize;
QStringList mUrls;
bool mStop;
void closeEvent(QCloseEvent *);
bool mOpenWithchanged;
bool mPermsChanged;
QString mMim;
bool mFolderColor;
QIcon mIcon;
QString mCustomIcon;
QHash<QString,QToolButton*>mHashColors;
private slots:
//QString mimLang(const QString &mim);
   void setMultiFiles(bool arg);
   void calculatSelectedFiles(const QStringList &urls);
  void calculatMultiFilesSize();
  void getDirSize(QString path);
  void setupOpenWith(const QString &mim);

  void on_toolButtonUp_clicked();
  void on_toolButtonDown_clicked();
  void on_toolButtonAdd_clicked();
  void on_toolButtonDelete_clicked();
  void on_buttonBox_accepted();
void applyPermsToAll(const QString &path);
  void getPermission(const QFileInfo &info);
void setChmod(int);
void setupFolderColor();
void toggleIcon(bool checked);
void setFolderColor();
void saveColorFolder(const QString &color);
void saveCustomFolder();
void removePercentEncoding(const QString &path);
//QString getChmod(bool isExec);
bool setPermission(const QString &path, bool isExecutable);
void on_toolButton_clicked();
};

#endif // PROPERTIESDLG_H
