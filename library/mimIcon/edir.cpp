#include "edir.h"
#include<QApplication>

#include<QDesktopServices>

//_________________________________________________________________
QString Edir::progPath(const QString &prog)
{
    // "/usr/bin/prog"
    QDir dir(QApplication::applicationDirPath());

    if(QFile::exists(dir.absoluteFilePath(prog))){
        return dir.absoluteFilePath(prog);
    } else{
        QString path = qgetenv("PATH");
        foreach(QString dir, path.split(":"))
        {

            if (QFile::exists(dir + QDir::separator() + prog)){
                return dir + QDir::separator() + prog;
            }
        }
    }
    return prog;
}

//_________________________________________________________________
QString Edir::dataDir()
{
    //  "/usr/share/elokab
    QDir appDir(QApplication::applicationDirPath());
    QString dirPath;
    appDir.cdUp();
    dirPath=  appDir.absolutePath()+"/share/elokab";
    return dirPath;
}

//_________________________________________________________________
QString Edir::dataAppDir()
{
  //  "/usr/share/elokab/elokab-appname"
    QString appName=QApplication::applicationName();
    return dataDir()+QDir::separator()+appName;
}


//_________________________________________________________________
QString Edir::dataHomeDir()
{
   //"$Home/.local/share
    QDir dir(QDir::homePath()+"/.local/share");
    if(!dir.exists())
        dir.mkpath(".");

    return dir.absolutePath();
}

//_________________________________________________________________
QString Edir::dataHomeAppDir()
{
    //"$Home/.local/share/elokab/elokab-appname
    QString appName=QApplication::applicationName();
    QDir dir(dataHomeDir()+"/elokab/"+appName);
    if(!dir.exists())
        dir.mkpath(".");

    return dir.absolutePath();
}

//_________________________________________________________________
QString Edir::libDir()
{
   // "/usr/lib/elokab/elokabe-appname"
   QString appName=QApplication::applicationName();
   QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cdUp();
    if(QFile::exists(pluginsDir.path()+"/lib/elokab/"+appName)){
        pluginsDir.cd(pluginsDir.path()+"/lib/elokab/"+appName);
    }else if(QFile::exists(pluginsDir.path()+"/lib64/elokab/"+appName)){
        pluginsDir.cd(pluginsDir.path()+"/lib64/elokab/"+appName);
    }
    return pluginsDir.absolutePath();
}

//_________________________________________________________________
QString Edir::configDir()
{
    //"$Home/.config/elokab"
    QDir dir(configHomeDir()+"/elokab");
    if(!dir.exists())
        dir.mkpath(".");
    return dir.absolutePath();
}

//_________________________________________________________________
QString Edir::configHomeDir()
{
    //"$Home/.config"
    QDir dir(QDir::homePath()+"/.config");
    if(!dir.exists())
        dir.mkpath(".");
    return dir.absolutePath();
}

//_________________________________________________________________
QStringList Edir::applicationsDirs()
{
    return QStringList()<<applicationsHomeDir()
                       <<"/usr/share/applications";
}

//_________________________________________________________________
QString Edir::applicationsHomeDir()
{
    QDir dir(QDir::homePath()+"/.local/share/applications");
    if(!dir.exists())
        dir.mkpath(".");
    return dir.absolutePath();
}

//_________________________________________________________________
QString Edir::cachDir()
{
    QString location;
#if QT_VERSION >= 0x050000
    location=QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
#else
    location=QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
#endif

    QDir dir(location);
    if(!dir.exists())
        dir.mkpath(".");
    return location;
}

//_________________________________________________________________
QString Edir::desktopDir()
{
    QString location;
#if QT_VERSION >= 0x050000
    location=QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#else
    location=QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif

    return location;
}

//_________________________________________________________________
QString Edir::documentsDir()
{
    QString location;
#if QT_VERSION >= 0x050000
    location=QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    location=QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#endif

    return location;
}

//_________________________________________________________________
QString Edir::musicDir()
{
    QString location;
#if QT_VERSION >= 0x050000
    location=QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
#else
    location=QDesktopServices::storageLocation(QDesktopServices::MusicLocation);
#endif

    return location;
}

//_________________________________________________________________
QString Edir::moviesDir()
{
    QString location;
#if QT_VERSION >= 0x050000
    location=QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
#else
    location=QDesktopServices::storageLocation(QDesktopServices::MoviesLocation);
#endif

    return location;
}

//_________________________________________________________________
QString Edir::picturesDir()
{
    QString location;
#if QT_VERSION >= 0x050000
    location=QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
#else
    location=QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
#endif

    return location;
}
