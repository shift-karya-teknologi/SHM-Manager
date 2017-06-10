#include "db.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "db.config.h"

namespace {

QSqlDatabase _createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", QSqlDatabase::defaultConnection);
    db.setHostName(DB_HOST);
    db.setPort(DB_PORT);
    db.setUserName(DB_USERNAME);
    db.setDatabaseName(DB_SCHEMA);
    db.setPassword(DB_PASSWORD);
    return db;
}

bool _connect(QSqlDatabase& db)
{
    if (!db.isOpen() && !db.open()) {
        QMessageBox mb(QMessageBox::Critical, "Database Error", "Gagal terhubung ke server.",
                       QMessageBox::Ok);
        mb.setDetailedText(db.lastError().text());
        mb.exec();
        return false;
    }

    return true;
}

}

QSqlDatabase Db::connection()
{
    bool testSuccess = true;
    QSqlDatabase db;
    {
        if (QSqlDatabase::connectionNames().size() == 0)
            db = _createConnection();
        else
            db = QSqlDatabase::database(QSqlDatabase::defaultConnection);

        if (!_connect(db)) {
            return db;
        }

        QSqlQuery q = db.exec("select 1");
        if (q.lastError().isValid()) {
            qWarning() << "Query test failed";
            db.close();
            testSuccess = false;
        }
    }

    if (!testSuccess) {
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
        _connect(db = _createConnection());
    }

    return db;
}
