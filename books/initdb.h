#ifndef INITDB_H
#define INITDB_H

#include <QtSql>

const auto BOOKS_SQL = QLatin1String(R"(
    create table books(id integer primary key, title varchar, author integer,
                        genre integer, year integer, rating integer)
    )");

const auto AUTHORS_SQL = QLatin1String(R"(
    create table books(id integer primary key, name varchar, birthday date)
    )");

QSqlError initDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();
    if (tables.contains("books", Qt::CaseInsensitive)
            && tables.contains("authors", Qt::CaseInsensitive))
        return  QSqlError();

    QSqlQuery q;
    if (!q.exec(BOOKS_SQL))
        return q.lastError();

    return QSqlError();
}

#endif // INITDB_H
