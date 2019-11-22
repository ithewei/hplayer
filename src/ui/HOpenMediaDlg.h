#ifndef H_OPEN_MEDIA_DLG_H
#define H_OPEN_MEDIA_DLG_H

#include "qtheaders.h"

class FileTab : public QWidget
{
    Q_OBJECT
public:
    explicit FileTab(QWidget *parent = nullptr);

public:
    QLineEdit* edit;
    QPushButton* btnBrowse;
};

class NetworkTab : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkTab(QWidget *parent = nullptr);

public:
    QLineEdit* edit;
};

class CaptureTab : public QWidget
{
    Q_OBJECT
public:
    explicit CaptureTab(QWidget *parent = nullptr);

public:
    QComboBox* cmb;
};

#include "hmedia.h"
class HOpenMediaDlg : public QDialog
{
    Q_OBJECT
public:
    explicit HOpenMediaDlg(QWidget *parent = nullptr);

signals:

public slots:
    virtual void accept();

protected:
    void initUI();
    void initConnect();

public:
    QTabWidget* tab;
    HMedia media;
};

#endif // H_OPEN_MEDIA_DLG_H
