#include "changepassworddialog.h"
#include "ui_changepassworddialog.h"
#include "db.h"
#include <QSqlQuery>
#include <QMessageBox>

ChangePasswordDialog::ChangePasswordDialog(QWidget *parent) :
    QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint),
    ui(new Ui::ChangePasswordDialog)
{
    ui->setupUi(this);

    connect(ui->okButton, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), SLOT(reject()));
}

ChangePasswordDialog::~ChangePasswordDialog()
{
    delete ui;
}

void ChangePasswordDialog::accept()
{
    QString oldPass = ui->oldPass->text();
    QString newPass1 = ui->newPass1->text();
    QString newPass2 = ui->newPass2->text();

    if (oldPass.isEmpty()) {
        ui->oldPass->setFocus();
        QMessageBox::warning(0, "Peringatan", "Masukkan kata sandi lama!");
        return;
    }

    if (newPass1.isEmpty()) {
        ui->newPass1->setFocus();
        QMessageBox::warning(0, "Peringatan", "Masukkan kata sandi baru!");
        return;
    }

    if (newPass2.isEmpty()) {
        ui->newPass2->setFocus();
        QMessageBox::warning(0, "Peringatan", "Konfirmasi kata sandi baru!");
        return;
    }

    if (newPass1 != newPass2) {
        QMessageBox::warning(0, "Peringatan", "Konfirmasi kata sandi salah!");
        ui->newPass2->setFocus();
        return;
    }

    QSqlQuery q(Db::connection());
    q.prepare("select data from settings where id='app.editing_password'");
    q.exec();
    q.next();
    if (oldPass != q.value(0).toString()) {
        ui->oldPass->setFocus();
        QMessageBox::warning(0, "Peringatan", "Kata sandi lama salah!");
        return;
    }

    q.prepare("update settings set data=? where id='app.editing_password'");
    q.bindValue(0, newPass1);
    q.exec();

    QMessageBox::information(0, "Informasi", "Kata sandi pengeditan telah diperbarui!");

    QDialog::accept();
}
