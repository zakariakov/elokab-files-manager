#ifndef DIALOGRENAMEFILES_H
#define DIALOGRENAMEFILES_H

#include <QDialog>

namespace Ui {
class DialogRenameFiles;
}

class DialogRenameFiles : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRenameFiles(QWidget *parent = nullptr);
    ~DialogRenameFiles();

private:
    Ui::DialogRenameFiles *ui;
};

#endif // DIALOGRENAMEFILES_H
