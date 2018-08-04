#ifndef DIALOGRENAMEFILES_H
#define DIALOGRENAMEFILES_H

#include <QDialog>
#include <QAbstractButton>
namespace Ui {
class DialogRenameFiles;
}

class DialogRenameFiles : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRenameFiles(QStringList list,QWidget *parent = nullptr);
    ~DialogRenameFiles();

private:
    Ui::DialogRenameFiles *ui;
    QPushButton *btnPreview;
    QPushButton *btnUndo;

   // QStringList mList;
private slots:
    void preview();
    void replaceString();
    void insertString();
    void insertNumiric();
    void on_buttonBox_clicked(QAbstractButton *button);
    void applyChange();
    void undoChange();
    void on_checkBox_toggled(bool checked);
    void on_toolButtonDown_clicked();
    void on_toolButtonUp_clicked();
    void on_treeWidget_itemSelectionChanged();
};

#endif // DIALOGRENAMEFILES_H
