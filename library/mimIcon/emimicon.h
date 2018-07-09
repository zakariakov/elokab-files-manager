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


#ifndef EMIMICON_H
#define EMIMICON_H
#include "eicon.h"
#include "edir.h"
#include <QObject>
#include <QString>
#include <QIcon>
#include <QDir>

#define APPX_EXE "application-x-executable"
/**
 * @brief The MimeTypeXdg class
 *خاص بجلب انواع الايقونات
 */
class EMimIcon
{

public:
    enum MESSAGES{
        BEGIN,    /// الخروج من الجلسة
        END, /// اسبات الجهاز
        NORMALE


    };

    //!
    explicit  EMimIcon(){}

    static  QIcon icon(const QFileInfo &info,bool previw=false);

    //!
    QHash<QString,QIcon> iconhash(const QFileInfo &info, bool previw);

    //!
    static QString mimeTyppe(const QFileInfo &info);

    //!
    static QIcon  iconFolder(const QString &f);

    //!
    static QIcon iconSymLink(QIcon icon);

    //!
    static  QByteArray iconThumbnails(const QString &file);

    //!
    static QIcon iconColorized(QIcon icon, QColor color);

    //!
    //    static QIcon iconWithoutSuffix(const QString &f);

    //!
    static QIcon iconDesktopFile(const QString &f);

    //!
    static QIcon iconBySuffix(const QString &suf, const QString &f);

    //!
    static QIcon iconByMimType(const QString &mim, const QString &f);

    /**
          * @brief getMimeTypeByFile جلب نوع الملف بواسطة مسار الملف
           * @param fileName مسار الملف
          * @return اسم الايقونة
          */
    static QString getMimeTypeByFile(QString fileName);

    /**
          * @brief getMimeTypeBySufix جلب نوع الملف بواسطة امتداد
          * @param sufix الامتداد
          * @return نوع الملف
          */

    static QString getMimeTypeBySufix(QString sufix);
    /**
          * @brief getIconFillBack جلب الايقونة الاحتياطية من نوع الملف
          * @param mimeType نوع الملف
          * @return "image" "video" "unknow"اسم الايقونة مثلا
          */
    static QString iconFillBack(QString mimeType);

    /**
          * @brief getIconFromCharPixmap جلب  ايقونة من "/usr/share/pixmaps/"
          * @param dir المجلد
          * @param iconName اسم الايقونة
          * @return مسار الايقونة
          */

    // static QIcon getIconFromCharPixmap(QDir dir, QString iconName );
    /**
          * @brief getIconFromeTheme جلب الايقونة من السمة او الاحتياطية من عدة اوجه
          * @param iconName
          * @return
          */

    // static  QIcon getIconFromeTheme(QString  iconName);

    /**
          * @brief getIconsFromeTheme
          * @param iconName اسم الايقونة
          * @param fillback الاحتياطية
          * @return الايقونة
          */


    //   static  QIcon getIconsFromeTheme(const QString  &iconName, QString fillback=APPX_EXE);
    //!
    //  static  QIcon iconsDefaultThemeApp(QString  iconName);


    //!
    static QHash<QString, QVariant> desktopFile(const QString &filePath, const QString &lc=QString());

    //!
    static QString desktopFilePath(const QString &file);

    //!
    static void launchApplication(const QString & fileName);

    //!
    static bool launchApp(const QString & fileName,const QString & mimetype="text/plain");

    //!
    static void launchAppFile(const QString & fileName,const QString & programe);

    //!
    static QString replaceArgument(const QString & proc);

    //!
    static QString formatSize(qint64 num);

    //!
    static QHash<QString ,qint64> getDriveInfo(QString path);

    //!
    static void updateMimeAssociatedApplication();

    //!
    static QStringList associatedApplication(const QString &mim);

    //!
    static QString getAssosiatedIcons(const QString &mim);
    //!
    static void setMimeAssociatedApplication(const QString &mimType, const QStringList &listAppDesktop );

    //!
    static void AddMimeAssociatedApplication(const QString &mimType,const QString &appDesktop  );

    //!
    static QString iconName(QString result) ;

    //!
    static QString mimLang(const QString &mim,const QString &local);
    static QString createDescktopFile(const QString &exec);
    static QString defaultTerminal();
signals:



protected:


private:


private slots:

    //!
    static QStringList getMimeDesktop(const QString &fileName );

    //!
    //  static QIcon chargeIconFile(QString dir,QString iconName);
};

#endif // EMIMICON_H
