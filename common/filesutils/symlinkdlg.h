#ifndef SYMLINKDLG_H
#define SYMLINKDLG_H

#include <QDialog>

namespace Ui {
class SymLinkDlg;
}

class SymLinkDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit SymLinkDlg(const QString &path,QWidget *parent = 0);
    ~SymLinkDlg();
    
private:
    Ui::SymLinkDlg *ui;
    QString mPath;

private slots:
   void creatLinkFile();
   void creatLinkDir();
      bool creatLink();
   void buttonBoxAccepted();
};

#endif // SYMLINKDLG_H
