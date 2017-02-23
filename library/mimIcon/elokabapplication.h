#ifndef ELOKABAPPLICATION_H
#define ELOKABAPPLICATION_H

#include <QApplication>
#include "elokabsettings.h"
class ElokabApplication : public QApplication
{
    Q_OBJECT
public:

    ElokabApplication(int& argc, char** argv);
   const QString translationPath();
private slots:
   void setIconsThemeName();

private:
  ElokabSettings *msetting;
};

#endif // ELOKABAPPLICATION_H
