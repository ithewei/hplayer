#include "hmultiview.h"
#include "qtstyles.h"
#include "mainwindow.h"

HMultiView::HMultiView(QWidget *parent) : QWidget(parent)
{
    initUI();
    initConnect();
    bStretch = false;
}

HMultiView::~HMultiView(){
    hlogd("~HMultiView");
}

void HMultiView::initUI(){
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    for (int i = 0; i < MV_STYLE_MAXNUM; ++i){
        HVideoWidget* player = new HVideoWidget(this);
        player->playerid = i+1;
        views.push_back(player);
    }

    setLayout(MV_STYLE_ROW, MV_STYLE_COL);

    labDrag = new QLabel(this);
    labDrag->setFixedSize(DRAG_WIDTH, DRAG_HEIGHT);
    labDrag->hide();
    labDrag->setStyleSheet(DRAG_QSS);

    labRect = new QLabel(this);
    labRect->hide();
    labRect->setStyleSheet(RECT_QSS);
}

void HMultiView::initConnect(){

}

void HMultiView::setLayout(int row, int col){
    saveLayout();
    table.init(row,col);
    updateUI();
}

void HMultiView::mergeCells(int lt, int rb){
#if 1
    // find first non-stop player as lt
    for (int i = lt; i <= rb; ++i){
        HVideoWidget* player = getPlayerByID(i);
        if (player && player->status != HVideoWidget::STOP){
            exchangeCells(player, getPlayerByID(lt));
            break;
        }
    }
#endif

    saveLayout();
    table.merge(lt,rb);
    updateUI();
}

void HMultiView::exchangeCells(HVideoWidget* player1, HVideoWidget* player2){
    qDebug("exchange %d<=>%d", player1->playerid, player2->playerid);

    QRect rcTmp = player1->geometry();
    int idTmp = player1->playerid;

    player1->setGeometry(player2->geometry());
    player1->playerid = player2->playerid;

    player2->setGeometry(rcTmp);
    player2->playerid = idTmp;
}

HVideoWidget* HMultiView::getPlayerByID(int playerid){
    for (int i = 0; i < views.size(); ++i){
        HVideoWidget *palyer = (HVideoWidget*)views[i];
        if (palyer->playerid == playerid)
            return palyer;
    }
    return NULL;
}

HVideoWidget* HMultiView::getPlayerByPos(QPoint pt){
    for (int i = 0; i < views.size(); ++i){
        QWidget* wdg = views[i];
        if (wdg->isVisible() && wdg->geometry().contains(pt))
            return (HVideoWidget*)wdg;
    }
    return NULL;
}

HVideoWidget* HMultiView::getIdlePlayer(){
    for (int i = 0; i < views.size(); ++i){
        HVideoWidget *player = (HVideoWidget*)views[i];
        if (player->isVisible() && player->status == HVideoWidget::STOP)
            return player;
    }
    return NULL;
}

#define CELL_BORDER     1
void HMultiView::updateUI(){
    int row = table.row;
    int col = table.col;
    int cell_w = width()/col;
    int cell_h = height()/row;

    int margin_x = (width()-cell_w*col)/2;
    int margin_y = (height()-cell_h*row)/2;
    int x = margin_x;
    int y = margin_y;
    for (int i = 0; i < views.size(); ++i){
        views[i]->hide();
    }

    int cnt = 0;
    for (int r = 0; r < row; ++r){
        for (int c = 0; c < col; ++c){
            int id = r*col + c + 1;
            HTableCell cell;
            QWidget *wdg = getPlayerByID(id);
            if (table.getTableCell(id, cell)){
                wdg->setGeometry(x, y, cell_w*cell.colspan() - CELL_BORDER, cell_h*cell.rowspan()- CELL_BORDER);
                wdg->show();
                ++cnt;
            }
            x += cell_w;
        }
        x = margin_x;
        y += cell_h;
    }

    bStretch = (cnt == 1);
}

void HMultiView::resizeEvent(QResizeEvent* e){
    updateUI();
}

void HMultiView::mousePressEvent(QMouseEvent *e){
    ptMousePress = e->pos();
}

void HMultiView::mouseReleaseEvent(QMouseEvent *e){
    if (action == EXCHANGE) {
        HVideoWidget* player1 = getPlayerByPos(ptMousePress);
        HVideoWidget* player2 = getPlayerByPos(e->pos());
        if (player1 && player2 && player1 != player2) {
            exchangeCells(player1, player2);
        }
    } else if (action == MERGE) {
        QRect rc = adjustRect(ptMousePress, e->pos());
        HVideoWidget* player1 = getPlayerByPos(rc.topLeft());
        HVideoWidget* player2 = getPlayerByPos(rc.bottomRight());
        if (player1 && player2 && player1 != player2){
            mergeCells(player1->playerid, player2->playerid);
        }
    }

    labRect->setVisible(false);
    labDrag->setVisible(false);
    setCursor(Qt::ArrowCursor);
}

void HMultiView::mouseMoveEvent(QMouseEvent *e){
    HVideoWidget *player = getPlayerByPos(ptMousePress);
    if (!player)
        return;

    if (e->buttons() == Qt::LeftButton){
        if (!labDrag->isVisible()){
            action = EXCHANGE;
            setCursor(Qt::OpenHandCursor);
            labDrag->setPixmap( QPixmap::fromImage(player->snap().scaled(labDrag->size())) );
            labDrag->setVisible(true);
        }

        labDrag->move(e->pos()-QPoint(labDrag->width()/2, labDrag->height()));
    }else if (e->buttons() == Qt::RightButton){
        if (!labRect->isVisible()){
            action = MERGE;
            setCursor(Qt::CrossCursor);
            labRect->setVisible(true);
        }

        labRect->setGeometry(adjustRect(ptMousePress, e->pos()));
    }
}

void HMultiView::mouseDoubleClickEvent(QMouseEvent *e){
    HVideoWidget* player = getPlayerByPos(e->pos());
    if (player){
        stretch(player);
    }
}

void HMultiView::stretch(QWidget* wdg){
    if (bStretch) {
        restoreLayout();
        bStretch = false;
    } else {
        saveLayout();
        for (int i = 0; i < views.size(); ++i) {
            views[i]->hide();
        }
        wdg->setGeometry(rect());
        wdg->show();
        bStretch = true;
    }
}

void HMultiView::saveLayout(){
    prev_table = table;
}

void HMultiView::restoreLayout(){
    HTable tmp = table;
    table = prev_table;
    prev_table = tmp;
    updateUI();
}

void HMultiView::play(HMedia& media){
    HVideoWidget* player = getIdlePlayer();
    if (player == NULL){
        QMessageBox::information(this, tr("Info"), tr("No spare player, please stop one and try agian!"));
    }else{
        player->open(media);
    }
}
