#ifndef FILEINFORMATION_H
#define FILEINFORMATION_H

#include <QWidget>
#include <QFileInfo>

namespace Ui {
class FileInformation;
}

/**
 * @brief The FileInformation class
 */
class FileInformation : public QWidget
{
          Q_OBJECT

     public:

          //!
          explicit FileInformation(QWidget *parent = 0);

          //!
          ~FileInformation();

          //!
          void showEvent ( QShowEvent * /*event*/ );

     private:

          Ui::FileInformation *ui;

          //!
          QString mFile;

     public slots:

          //!
          void setFileName(const QString &file);

     private slots:

          //!
          QString getDirSize(const QString &path);

          //!
          void setDirInformation(const QFileInfo &fi);

          //!
          void setFileInformation(const QFileInfo &fi);

          //!
          void setImageInformation(const QFileInfo &fi,const QString &mim);

};

#endif // FILEINFORMATION_H
