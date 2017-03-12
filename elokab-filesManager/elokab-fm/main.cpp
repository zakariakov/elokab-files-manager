
#include "mainwindow.h"
#include <ElokabApplication>
#include <QTranslator>
#include <QLibraryInfo>
/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */

int main(int argc, char *argv[])
{
    setenv("TERM", "xterm", 1); // TODO/FIXME: why?

/*
     a.setApplicationName("elokab-fm");
     a.setApplicationVersion("0.1");
     a.setOrganizationName("elokab");

     /// مسار مجلد البرنامج
      QString translatPath= Edir::dataDir()+"/translations";
     /// جلب اعدادات اللغة
     QSettings globalSetting("elokab","elokabsettings");
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
     /// جلب ترجمة البرنامج من مجلد البرنامج
     QTranslator translator;
     translator.load(QString(translatPath+"/"+locale+"/"+a.applicationName()));

     a.installTranslator(&translator);
*/
     ElokabApplication a(argc, argv);
     QStringList args = a.arguments();
     if(args.count()>1)
     {

         QUrl url(args.at(1));

         QString  pathUrl;

//         if(url.isLocalFile())
//             pathUrl=url.toLocalFile();
//         else
             pathUrl= url.toString();
             if(pathUrl.startsWith("file://"))
                 pathUrl.remove("file://");
         QFileInfo fi(pathUrl);
         if(!fi.isDir()){

             EMimIcon::launchApplication(pathUrl);
             return 0;
         }
     }

            /// جلب ترجمة البرنامج من مجلد البرنامج
           a.setApplicationName("elokab-fm");
     QTranslator translator;
            translator.load(a.translationPath());
            a.installTranslator(&translator);

     a.setWindowIcon(EIcon::fromTheme("system-file-manager","folder"));

  MainWindow w;
     w.show();

     return a.exec();
}
