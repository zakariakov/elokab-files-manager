#ifndef EICON_H
#define EICON_H
#include <QIcon>
#include <QHash>
//#include "eiconengin.h"
#define THEM_BACK   "Elokab"
#define APP_EXE "application-x-executable"
#define THEM_PIXMAP   "/usr/share/pixmaps"
class EIcon
{
public:
    //!
    explicit  EIcon(){}
     static EIcon *instance();
    static  void seTemeName(const QString  &name);
    //!
    static  QIcon fromTheme(const QString  &iconName,const QString &fallback=QString());


private:
    static  bool hasPathIcon(const QString &name,const QString &path);
    static  bool searchIcon(const QString &name,const QString &path);

 static   QIcon iconFromThemePath(const QString &m_iconName, const QString &path);
  static  void addIconFile(QIcon m_icon, const QString m_iconName, const QString &curPath);
    //!

    QHash<QString,QIcon>cachIcon;

};

#endif // EICON_H
