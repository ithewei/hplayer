#ifndef H_MULTI_VIEW_H
#define H_MULTI_VIEW_H

#include "qtheaders.h"
#include "htable.h"
#include "HVideoWidget.h"

#define MV_STYLE_MAXNUM     64

// F(id, row, col, label, image)
#define FOREACH_MV_STYLE(F) \
    F(MV_STYLE_1,  1, 1, " MV1",  ":/image/style1.png")     \
    F(MV_STYLE_2,  1, 2, " MV2",  ":/image/style2.png")     \
    F(MV_STYLE_4,  2, 2, " MV4",  ":/image/style4.png")     \
    F(MV_STYLE_9,  3, 3, " MV9",  ":/image/style9.png")     \
    F(MV_STYLE_16, 4, 4, " MV16", ":/image/style16.png")    \
    F(MV_STYLE_25, 5, 5, " MV25", ":/image/style25.png")    \
    F(MV_STYLE_36, 6, 6, " MV36", ":/image/style36.png")    \
    F(MV_STYLE_49, 7, 7, " MV49", ":/image/style49.png")    \
    F(MV_STYLE_64, 8, 8, " MV64",  ":/image/style64.png")   \

enum MV_STYLE {
#define ENUM_MV_STYLE(id, row, col, label, image) id,
    FOREACH_MV_STYLE(ENUM_MV_STYLE)
};

class HMultiView : public QWidget
{
    Q_OBJECT
public:
    enum Action {
        STRETCH,
        EXCHANGE,
        MERGE,
    };
    explicit HMultiView(QWidget *parent = nullptr);
    ~HMultiView();

    HVideoWidget* getPlayerByID(int playerid);
    HVideoWidget* getPlayerByPos(QPoint pt);
    HVideoWidget* getIdlePlayer();

signals:

public slots:
    void setLayout(int row, int col);
    void saveLayout();
    void restoreLayout();

    void mergeCells(int lt, int rb);
    void exchangeCells(HVideoWidget* player1, HVideoWidget* player2);
    void stretch(QWidget* wdg);

    void play(HMedia& media);

protected:
    void initUI();
    void initConnect();
    void updateUI();

    virtual void resizeEvent(QResizeEvent* e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);

public:
    HTable table;
    HTable prev_table;
    QVector<QWidget*> views;
    QLabel *labRect;
    QLabel *labDrag;

    QPoint ptMousePress;
    uint64_t tsMousePress;
    Action action;
    bool bStretch;
};

#endif // H_MULTI_VIEW_H
