#include "elokabapplication.h"

#include <QSettings>
#include <QLocale>
#include "eicon.h"
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>
#include <QDir>
#include <QIcon>
ElokabApplication::ElokabApplication(int &argc, char **argv):
    QApplication(argc, argv)
{
    setApplicationVersion("0.3");
    setOrganizationName("elokab");
    msetting=new ElokabSettings;
    connect(msetting,SIGNAL(iconThemeChanged()),this,SLOT(setIconsThemeName()));
}

 void  ElokabApplication::setIconsThemeName()
 {

     EIcon::instance()->seTemeName(msetting->loadIconThems());
 }

 const QString ElokabApplication::translationPath()
{
     QDir appDir(applicationDirPath());
     appDir.cdUp();
    QString dirPath=  appDir.absolutePath()+"/share/"+organizationName();

    /// جلب اعدادات اللغة
    QSettings globalSetting(organizationName(),"elokabsettings");
    globalSetting.beginGroup("Language");
    QString locale=globalSetting.value("Name","Default").toString();
    globalSetting.endGroup();

    /// اذاكانت اللغة الافتراضية
    if(locale=="Default")
            locale = QLocale::system().name().section("_",0,0);
    /// اللغة الحالية لجميع البرنامج
    QLocale::setDefault(QLocale(locale));
    /// جلب ترجمات كيوتي
    QString translatorFileName = QLatin1String("qt_");
    translatorFileName += locale;
    QTranslator *translatorsys = new QTranslator;
    if (translatorsys->load(translatorFileName, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
            QApplication::installTranslator(translatorsys);
     qDebug()<<dirPath+"/translations/"+locale+"/"+applicationName();

         qDebug()<<locale<<layoutDirection();
    return dirPath+"/translations/"+locale+"/"+applicationName();

}
