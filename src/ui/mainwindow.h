#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qtheaders.h"
#include "qtstyles.h"
#include "centralwidget.h"
#include "singleton.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    DCLR_SINGLETON(MainWindow)

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum STATUS{
        NORMAL,
        FULLSCREEN,
        MV_FULLSCREEN,
    }status;

protected:
    void initUI();
    void initConnect();

    void initMenu();

    virtual void keyPressEvent(QKeyEvent* e);

signals:
    void reqPlay(HMedia& media);

public slots:
    void about();
    void fullscreen();

    void onMVStyleSelected(int id);
    void mv_fullscreen();

    void onOpenMedia(int index);

public:
    CentralWidget* center;
    QAction *actMenubar;
    QAction *actFullscreen;
    QAction *actMvFullscreen;

    QVector<QToolBar*> toolbars;
};

#define g_mainwnd MainWindow::instance()

#endif // MAINWINDOW_H