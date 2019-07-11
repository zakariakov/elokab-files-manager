#ifndef DIALOGADDACTION_H
#define DIALOGADDACTION_H

#include <QDialog>

namespace Ui {
class DialogAddAction;
}

class DialogAddAction : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddAction(QWidget *parent = nullptr);
    ~DialogAddAction();

private:
    Ui::DialogAddAction *ui;
};

#endif // DIALOGADDACTION_H
