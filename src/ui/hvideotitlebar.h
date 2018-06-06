#ifndef HVIDEOTITLEBAR_H
#define HVIDEOTITLEBAR_H

#include "qtheaders.h"

class HVideoTitlebar : public QFrame
{
    Q_OBJECT
public:
    explicit HVideoTitlebar(QWidget *parent = nullptr);

protected:
    void initUI();
    void initConnect();

public:
    QLabel *labTitle;
    QPushButton *btnClose;
};

#endif // HVIDEOTITLEBAR_H
