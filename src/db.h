#ifndef DB_H
#define DB_H

#include <QSqlDatabase>

class Db
{
public:
    static QSqlDatabase connection();
};

#endif // DB_H
