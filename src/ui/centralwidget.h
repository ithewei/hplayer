#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include "qtheaders.h"
#include "hmedialist.h"
#include "hmultiview.h"

class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CentralWidget(QWidget *parent = nullptr);
    ~CentralWidget();

signals:

public slots:

protected:
    void initUI();
    void initConnect();

public:
    HMediaList* ml;
    HMultiView* mv;
};

#endif // CENTRALWIDGET_H
