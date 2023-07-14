#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("d:\\lianxi\\qt\\students.dat");
    if (!db.open())
        qDebug() << db.lastError().text();
    else
    {
        qDebug() << "open database ok";

        QSqlQuery query;
        query.exec("SELECT * FROM students WHERE score >= 60 AND score <= 80");
        while (query.next())
        {
            QString id = query.value(0).toString();
            QString name = query.value(1).toString();
            QString score = query.value(2).toString();
            QString Class = query.value(3).toString();

            qDebug() << id << name << score << Class;
        }

        db.close();
    }

    return a.exec();
}
