#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "qtheaders.h"
#include "qtstyles.h"
#include "singleton.h"

#include "CentralWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    SINGLETON_DECL(MainWindow)

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum STATUS {
        NORMAL,
        FULLSCREEN,
        MV_FULLSCREEN,
    } status;

protected:
    void initUI();
    void initConnect();

    void initMenu();

    virtual void keyPressEvent(QKeyEvent* e);

signals:

public slots:
    void about();
    void fullscreen();

    void onMVStyleSelected(int id);
    void mv_fullscreen();

    void OpenMediaDlg(int index);

public:
    QAction *actMenubar;
    QAction *actFullscreen;
    QAction *actMvFullscreen;

    QVector<QToolBar*> toolbars;

    CentralWidget* center;
};

#define g_mainwnd MainWindow::instance()

#endif // MAIN_WINDOW_H
