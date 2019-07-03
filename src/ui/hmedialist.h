#ifndef HMEIDALIST_H
#define HMEIDALIST_H

#include "qtheaders.h"

class HMediaList : public QWidget
{
    Q_OBJECT
public:
    explicit HMediaList(QWidget *parent = nullptr);

signals:

public slots:

protected:
    void initUI();
    void initConnect();
};

#endif // HMEIDALIST_H
