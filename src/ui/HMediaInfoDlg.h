#ifndef H_MEIDA_INFO_DLG_H
#define H_MEIDA_INFO_DLG_H

#include "qtheaders.h"

class HMediaInfoDlg : public QDialog
{
    Q_OBJECT
public:
    explicit HMediaInfoDlg(QWidget *parent = nullptr);

signals:

public slots:

protected:
    void initUI();
    void initConnect();
};

#endif // H_MEIDA_INFO_DLG_H
