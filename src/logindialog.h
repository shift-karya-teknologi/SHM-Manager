#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

public slots:
    void accept();
    void changePassword();

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
