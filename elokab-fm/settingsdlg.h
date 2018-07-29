#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QDialog>
#include "settings.h"

namespace Ui {
class SettingsDlg;
}

class SettingsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDlg(Settings *settings,QWidget *parent = nullptr);
    ~SettingsDlg();

    bool   isSingleclick();
    bool   isConfirmDragDrop();
    bool   isRootDecorated();
    bool   isClassicIcons();
    bool   videoThumbNails();
    bool   pdfThumbNails();
    bool   doubleClickRemame();

    QString terminal();

private slots:

    void on_buttonBox_accepted();

    void on_pushButtonClean_clicked();

    void on_pushButtonRmove_clicked();

private:
     Settings mSettings;
    Ui::SettingsDlg *ui;

};

#endif // SETTINGSDLG_H
