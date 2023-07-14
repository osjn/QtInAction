#ifndef BOOKDELEGATE_H
#define BOOKDELEGATE_H

#include <QSqlRelationalDelegate>
#include <QModelIndex>
#include <QPixmap>
#include <QSize>

QT_FORWARD_DECLARE_CLASS(QPainter)

class BookDelegate : public QSqlRelationalDelegate
{
public:
    BookDelegate(QObject *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

private:
    QPixmap star;
};

#endif // BOOKDELEGATE_H
