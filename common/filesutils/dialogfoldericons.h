#ifndef DIALOGFOLDERICONS_H
#define DIALOGFOLDERICONS_H

#include <QDialog>

namespace Ui {
class DialogFolderIcons;
}

class DialogFolderIcons : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFolderIcons(QWidget *parent = 0);
    ~DialogFolderIcons();

private:
    Ui::DialogFolderIcons *ui;
};

#endif // DIALOGFOLDERICONS_H
