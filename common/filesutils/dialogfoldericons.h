#ifndef DIALOGFOLDERICONS_H
#define DIALOGFOLDERICONS_H

#include <QDialog>
#include <QListWidgetItem>
namespace Ui {
class DialogFolderIcons;
}

class DialogFolderIcons : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFolderIcons(QWidget *parent = nullptr);
    ~DialogFolderIcons();

    QString iconName(){return mIconName;}
private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem */*previous*/);

    void on_listWidget_itemActivated(QListWidgetItem *item);

private:
    Ui::DialogFolderIcons *ui;
    QString mIconName;
};

#endif // DIALOGFOLDERICONS_H
