#include "logindialog.h"
#include "ui_logindialog.h"
#include "changepassworddialog.h"
#include "db.h"
#include <QSqlQuery>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    connect(ui->okButton, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), SLOT(reject()));
    connect(ui->changePassButton, SIGNAL(clicked(bool)), SLOT(changePassword()));
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::accept()
{
    QString pass = ui->passwordEdit->text();
    if (pass.size() > 0) {
        QSqlQuery q(Db::connection());
        q.prepare("select data from settings where id='app.editing_password'");
        q.exec();
        q.next();
        if (pass != q.value(0).toString()) {
            ui->passwordEdit->setFocus();
            QMessageBox::warning(0, "Peringatan", "Kata sandi salah!");
            return;
        }

        setProperty("ALLOW_EDITING", true);
    }

    QDialog::accept();
}

void LoginDialog::changePassword()
{
    ChangePasswordDialog dialog;
    dialog.exec();
}
