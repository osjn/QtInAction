#include "bookwindow.h"
//#include "bookdelegate.h"
#include "initdb.h"

BookWindow::BookWindow(QWidget *parent)
    : QMainWindow(parent)
{
    if (!QSqlDatabase::drivers().contains("QSQLITE"))
        QMessageBox::critical(
                    this,
                    "无法加载数据库",
                    "本程序需要SQLITE驱动");

    QSqlError err = initDb();
    if (err.type() != QSqlError::NoError)
    {
        showError(err);
        return;
    }

    model = new QSqlRelationalTableModel(this);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("books");

    authorIdx = model->fieldIndex("author");
    genreIdx = model->fieldIndex("genre");

    model->setRelation(authorIdx, QSqlRelation("author", "id", "name"));
    model->setRelation(genreIdx, QSqlRelation("genres", "id", "name"));

    model->setHeaderData(authorIdx, Qt::Horizontal, tr("Author Name"));
    model->setHeaderData(genreIdx, Qt::Horizontal, tr("Genre"));
    model->setHeaderData(model->fieldIndex("title"), Qt::Horizontal, tr("Title"));
    model->setHeaderData(model->fieldIndex("year"), Qt::Horizontal, tr("Year"));
    model->setHeaderData(model->fieldIndex("rating"), Qt::Horizontal, tr("Rating"));

    if (!model->select())
    {
        showError(model->lastError());
        return;
    }

    QTableView *bookTable = new QTableView(this);
    bookTable->setModel(model);
    bookTable->setItemDelegate(new BookDelegate(bookTable));
}

void BookWindow::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "无法初始化数据库", "初始化数据时发生错误：" + err.text());
}

BookWindow::~BookWindow()
{
}

