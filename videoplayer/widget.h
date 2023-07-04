#ifndef WIDGET_H
#define WIDGET_H

#include "videowidget.h"

#include <QWidget>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void onBtnOpenClick();

private:
    void initWindow();

    VideoWidget *videoWidget;
};
#endif // WIDGET_H
