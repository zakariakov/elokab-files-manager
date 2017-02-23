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


#ifndef MENUGEN_H
#define MENUGEN_H

#include <QtGui>
#include <QtCore>
#include <QObject>

/*!
 * \brief The MenuGen class
 *خاص بانشاء ملف بالتطبيقات والتطبيقات الخاثة بكل ملف
 *application & mimetype
 */
class MenuGen : public QObject
{
    Q_OBJECT
public:
    explicit MenuGen(const QString &locale,QObject *parent = 0);

signals:
    /*!
         * \brief appDesktopChanged عند تغير احد ملفات ديسكتوب
         */
    void appDesktopChanged();
private:
    QTimer *mtimer;
    QString m_locale;//اللغة الحالية
    QStringList wordList;//قائمة بجميع الملفات من نوع ديسكتوب
    QStringList m_desktopList;
 //   QStringList mimList;//قائمة بكل انواع الملفات والبرامج المرتبطة بها
    int m_appCount;//عدد التطبيقات
    QFileSystemWatcher *m_sWatcher; //مراقب تغير الملفات والمجلدات

public slots:
    /*!
     * \brief blockSignals توقيف مراقبة الملفات عندما يكون في حالة تكوين الملفات
     * \param b اشارة بنعم او لا
     */
    void blockSignals(bool b){m_sWatcher->blockSignals(b);}
    /*!
     * \brief checkAppsDesktopFile التأكد من ان البرامج لم تتغير وبان ملف البرامج موجود
     * \return ايجابي او سلبي
     */
    bool checkAppsDesktopFile();
    /*!
     * \brief genirateAppFile انشاء ملف البرامج والتطبيقات المظمنة مع الملفات
     */
    void genirateAppFile();

private slots:
    /**
     * @brief updatApp بعث اشارة لمراقب الملفات بعد مدة
     *لتجنب التحميل المتكرر عند الاشارات المتقاربة
     */
    void updatApp();

    void loadSettings();
    void saveSettings();
    /*!
     * \brief countDesktopDir عدد مجلدات التي تحوي البرامج
     * \param dir المجلد
     * \return  العدد
     */
    int countDesktopDir(QString dir);
//    /*!
//     * \brief countDesktopFile  عدد الملفات التي تحوي البرامج
//     * \param dir المجلد
//     * \return  العدد
//     */
//    int countDesktopFile(QString   dir);
    /**
     * @brief chargeDesktopDir تجميل مجلد معين
     * @param dir المجلد
     */
    void chargeDesktopDir(QString dir);
    /**
     * @brief chargeDesktopFile تحميل الملفات من المجلد
     * @param dir المجلد
     */
    void chargeDesktopFile(QString   dir);
    /**
     * @brief openDesktopEntry فتح ملف من نوع ديسكتوب
     * @param file اسم الملف
     * @return   name+"|"+exec+"|"+category+"|"+icon+"|"+file+"|"+GenericName;
     */
    QString openDesktopEntry(QString file);
};

#endif // MENUGEN_H

