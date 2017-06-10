#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class ChangePasswordDialog;
}

class ChangePasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePasswordDialog(QWidget *parent = 0);
    ~ChangePasswordDialog();

public slots:
    void accept();

private:
    Ui::ChangePasswordDialog *ui;
};

#endif // CHANGEPASSWORDDIALOG_H
