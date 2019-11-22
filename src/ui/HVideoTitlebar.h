#ifndef H_VIDEO_TITLEBAR_H
#define H_VIDEO_TITLEBAR_H

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

#endif // H_VIDEO_TITLEBAR_H
