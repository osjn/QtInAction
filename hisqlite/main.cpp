#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QRandomGenerator>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QStringList names;
    names << "小A" << "小B" << "小C" << "小D" << "小E" << "小F" << "小G"
          << "小Ｈ" << "小I" << "小J" << "小K" << "小L" << "小M" << "小N";

    QStringList classes;
    classes << "初1-1班" << "初1-2班" << "初1-3班" << "初2-1班" << "初2-2班"
            << "初2-3班" << "初3-1班" << "初3-2班" << "初3-3班";

    QSqlDatabase db;
    if (QSqlDatabase::contains("qt_sql_default_connection"))
        db = QSqlDatabase::database("qt_sql_default_connection");
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("students.dat");
        if (!db.open())
            qDebug() << db.lastError().text();
        else
        {
            qDebug() << "create ok";

            QSqlQuery query;
            query.exec("DROP TABLE students");
            query.exec("CREATE TABLE students ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "name VARCHAR(40) NOT NULL, "
                       "score INTEGER NOT NULL, "
                       "class VARCHAR(40) NOT NULL)");
            qDebug() << "create TABLE ok";

            query.prepare("INSERT INTO students (name, score, class) "
                          "VALUES (:name, :score, :class)");
            foreach (QString name, names)
            {
                query.bindValue(":name", name);
                query.bindValue(":score", (QRandomGenerator::global()->bounded(101) % 101));
                query.bindValue(":class", classes[QRandomGenerator::global()->bounded(1, 4) % classes.length()]);
                query.exec();
            }

            qDebug() << "insert record ok";

            db.close();
        }
    }


    return a.exec();
}
