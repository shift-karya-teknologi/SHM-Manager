#include "shmmanager.h"
#include "logindialog.h"
#include "db.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>

#define APP_VERSION 2

int main(int argc, char *argv[])
{
    QLocale::setDefault(QLocale(QLocale::Indonesian, QLocale::Indonesia));
    QApplication app(argc, argv);

    {
        QSqlDatabase db = Db::connection();
        if (!db.isOpen()) {
            return -1;
        }

        QSqlQuery q = db.exec("select * from settings where id='app.minimum_version'");
        q.next();
        int minimumAppVersion = q.value("data").toInt();
        if (APP_VERSION < minimumAppVersion) {
            QMessageBox::warning(0, "Kesalahan Aplikasi", "Aplikasi yang digunakan sudah usang, silahkan perbarui!");
            return 0;
        }
    }

    bool allowEditing = false;

    {
        LoginDialog dialog;
        if (!dialog.exec()) {
            return 0;
        }
        allowEditing = dialog.property("ALLOW_EDITING").toBool();
    }

    ShmManager manager;
    manager.setProperty("ALLOW_EDITING", allowEditing);
    manager.applAcl();
    manager.showMaximized();

    int exitCode = app.exec();

    {
        QSqlDatabase::database().close();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }

    return exitCode;
}
