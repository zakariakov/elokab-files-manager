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
    explicit PropertiesDlg(const QStringList &urls,QWidget *parent = 0);
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
void removePercentEncoding(const QString &path);
//QString getChmod(bool isExec);
bool setPermission(const QString &path, bool isExecutable);
};

#endif // PROPERTIESDLG_H
