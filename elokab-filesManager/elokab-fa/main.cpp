
#include "filesactions.h"

#include <ElokabApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QIcon>
#include <QDir>
#include <QSettings>

#include <QLocale>

#include <QDebug>
/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */

int main(int argc, char *argv[])
{

      ElokabApplication a(argc, argv);
           /// جلب ترجمة البرنامج من مجلد البرنامج
          a.setApplicationName("elokab-fa");
    QTranslator translator;
           translator.load(a.translationPath());
           a.installTranslator(&translator);
    QStringList args = a.arguments();

    //------------------------------------argument-------------------------------------------------

    qDebug()<<"FilesActions"<<args.count();
    if(args.count()==3)
    {
        FilesActions w;

        QString action = args.at(1);
        QString str = args.at(2);
        QStringList list=str.split(",");
        if (action == "-rm" )
        {

            qDebug()<<"remove"<<str;

            w.removeFiles(list);

        }
        else if (action == "-mt" )
        {

            qDebug()<<"move to trash"<<str;

            w.moveFilesToTrash(list);

        }
        return 0;
    }

    if(args.count()==4)
    {
        FilesActions w;

        QString action = args.at(1);
        QString str = args.at(2);
         QString dist = args.at(3);
        QStringList list=str.split(",");

        if (action == "-m")
        {
            qDebug()<<"move"<<str;
            w.moveFiles(list,dist);

        }
        else if (action == "-p"  )
        {
            qDebug()<<"past"<<str;
             w.pastFiles(list,dist);
        }



        return 0;
    }
    QString help=QObject::tr
            ("Elokab Files Actions Gui Version 0.1 .\n"
             " \n"
             "Usage: elokab-fa [OPTION]..File(s)..\n"
             "Usage: elokab-fa [OPTION]..SOURCE... DIRECTORY..\n"
             "OPTION:\n"
             " -rm          Remove File(s).\n"
             " -mt          Move File(s) to trash.\n"
             " -m           Rename SOURCE to DEST, or move SOURCE(s) to DIRECTORY.\n"
             " -p           Copy SOURCE to DEST, or multiple SOURCE(s) to DIRECTORY.\n"

           );
     qWarning() <<help; return 0;



   //  FilesActions w;
    // w.show();

     //return a.exec();
}
