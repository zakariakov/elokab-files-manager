#ifndef OPENWITHDLG_H
#define OPENWITHDLG_H

#include <QDialog>

#include <QDomDocument>
#include <QSettings>
#include <QTreeWidgetItem>
namespace Ui {
class OpenWithDlg;
}

class OpenWithDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit OpenWithDlg(bool remember=true,QWidget *parent = 0);
    ~OpenWithDlg();
    QString execName(){return mExecName;}
    QString desktopFileName(){return mDesktopFileName;}
    QString desktopText(){return mDesktopText;}
    QIcon desktopIcon(){return mDesktopIcon;}
public slots:
    void   setmim(const QString mim){mMim=mim;}
    bool addAssociation();
private slots:
    void slotitemSelectionChanged();
    void chargeTreeWidgetFillBack();
    void chargeXmlDocument();
    void addMenuDirectory(QString name, QString directory,QString category);
    void chargeAppDesktop();
    void slotExecChanged(const QString &txt);
    //  void on_buttonBox_accepted();

    void getOpenFile();
  //  QString createDescktopFile(const QString &exec);
   // void on_buttonBox_accepted();

private:
    Ui::OpenWithDlg *ui;
    QDomDocument m_doc;
    QString  mExecName;
    QString mDesktopFileName;
    QIcon mDesktopIcon;
    QString mDesktopText;
    QStringList mListCompliter;
    QString mMim;
  //  bool mRemember;
};

#endif // OPENWITHDLG_H
