#ifndef CONFIRMDLG_H
#define CONFIRMDLG_H

#include <QDialog>
#include <QPushButton>
namespace Ui {
class ConfirmDlg;
}

class ConfirmDlg : public QDialog
{
          Q_OBJECT
          
     public:

          //!
          explicit ConfirmDlg(const QString &source, const QString &dest, bool cut=false, QWidget *parent = 0);

          //!
          ~ConfirmDlg();

          //!
          enum Actions{
               ReName,
               OverWrite,
               Skyp,
               WriteIntoFolder,
               SkypFolder
          };

     public slots:

          //!
          QString getName();

          //!
          Actions getAction(){return mAction;}

          //!
          bool isApplyToAll();

     private slots:

          //!
          void nameChanged(const QString &txt);

          //!
          void renameAccepted();

          //!
          void overWriteAccepted();

          //!
          void skypAccepted();

          //!
          void writeIntoAccepted();

     private:

          Ui::ConfirmDlg *ui;

          //!
          QPushButton *skypButton;

          //!
          QPushButton *renameButton;

          //!
          QPushButton *overWriteButton;

          //!
          QPushButton *writeIntoButton;

          //!
          Actions mAction;

          //!
          QString mOldName;

          //!
          QString mPath;

          //!
          bool mIsdir;

};

#endif // CONFIRMDLG_H
