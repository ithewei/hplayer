#ifndef QT_FUNCTIONS_H
#define QT_FUNCTIONS_H

#include "qtheaders.h"

typedef QWidget HWidget;

//---define some widgets for unified style---------------------------------------------
inline void setFont(int size) {
    QFont font = qApp->font();
    font.setPointSize(size);
    qApp->setFont(font);
}

inline void setPalette(QColor clr) {
    qApp->setPalette(QPalette(clr));
}

inline void loadSkin(const char *skin) {
    // :/skin/dark.qss
    QFile file(QString(":/skin/") + QString(skin) + QString(".qss"));
    if (file.open(QFile::ReadOnly)) {
        qApp->setStyleSheet(file.readAll());
        file.close();
    }
}

inline void loadLang(const char* lang) {
    // :/lang/app_zh_CN.qm
    QTranslator *app_translator = new QTranslator(qApp);
    if (lang && *lang) {
        app_translator->load(QString(":/lang/app_") + QString(lang) + QString(".qm"));
    }
    else {
        app_translator->load(QLocale(), "app", "_", ":/lang");
    }
    qApp->installTranslator(app_translator);

    QTranslator *qt_translator = new QTranslator(qApp);
    if (lang && *lang) {
        qt_translator->load(QString(":/lang/qt_") + QString(lang) + QString(".qm"));
    }
    else {
        qt_translator->load(QLocale(), "qt", "_", ":/lang");
    }
    qApp->installTranslator(qt_translator);
}

inline QPushButton* genPushButton(QPixmap pixmap, QString tooltip = QString(), QSize sz = QSize(0,0), QWidget* parent = NULL) {
    QPushButton* btn = new QPushButton(parent);
    btn->setFlat(true);
    if (sz.isEmpty()) {
        sz = pixmap.size();
    }
    btn->setFixedSize(sz);
    btn->setIconSize(sz);
    btn->setIcon(pixmap.scaled(sz, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    btn->setToolTip(tooltip);
    return btn;
}

inline QHBoxLayout* genHBoxLayout() {
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setContentsMargins(10,1,10,1);
    hbox->setSpacing(1);
    return hbox;
}

inline QVBoxLayout* genVBoxLayout() {
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setContentsMargins(1,1,1,1);
    vbox->setSpacing(1);
    return vbox;
}

inline void setBgFg(QWidget* wdg, QColor bg, QColor fg = Qt::white) {
    wdg->setAutoFillBackground(true);
    QPalette pal = wdg->palette();
    pal.setColor(QPalette::Background, bg);
    pal.setColor(QPalette::Foreground, fg);
    wdg->setPalette(pal);
}

inline void connectButtons(QPushButton* btn1, QPushButton* btn2) {
    QObject::connect(btn1, SIGNAL(clicked(bool)), btn1, SLOT(hide()) );
    QObject::connect(btn1, SIGNAL(clicked(bool)), btn2, SLOT(show()) );

    QObject::connect(btn2, SIGNAL(clicked(bool)), btn2, SLOT(hide()) );
    QObject::connect(btn2, SIGNAL(clicked(bool)), btn1, SLOT(show()) );
}

inline void centerWidget(QWidget* wdg) {
    int w = wdg->width();
    int h = wdg->height();
    if (w < DESKTOP_WIDTH && h < DESKTOP_HEIGHT) {
        wdg->setGeometry((DESKTOP_WIDTH-w)/2, (DESKTOP_HEIGHT-h)/2, w, h);
    }
}

inline QRect adjustRect(QPoint pt1, QPoint pt2) {
    int x1 = qMin(pt1.x(), pt2.x());
    int x2 = qMax(pt1.x(), pt2.x());
    int y1 = qMin(pt1.y(), pt2.y());
    int y2 = qMax(pt1.y(), pt2.y());
    return QRect(QPoint(x1,y1), QPoint(x2,y2));
}

inline QRect adjustPos(QRect rc, QRect rcParent) {
    int x = qMax(rc.left(), rcParent.left());
    int y = qMax(rc.top(), rcParent.top());
    int w = qMin(rc.width(), rcParent.width());
    int h = qMin(rc.height(), rcParent.height());

    if (rc.right() >= rcParent.right()) {
        x = rcParent.right() - w;
    }
    if (rc.bottom() >= rcParent.bottom()) {
        y = rcParent.bottom() - h;
    }

    return QRect(x,y,w,h);
}

inline void toggle(QWidget* wdg) {
    wdg->setVisible(!wdg->isVisible());
}

#endif // QT_FUNCTIONS_H
