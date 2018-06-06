#include "hmultiview.h"
#include "qtstyles.h"
#include "mainwindow.h"

HMultiView::HMultiView(QWidget *parent) : QWidget(parent)
{
    initUI();
    initConnect();
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
    layout.init(row,col);
    relayout();
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

    layout.merge(lt,rb);
    relayout();
}

void HMultiView::exchangeCells(HVideoWidget* player1, HVideoWidget* player2){
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
void HMultiView::relayout(){
    int row = layout.row;
    int col = layout.col;
    int cell_w = width()/col;
    int cell_h = height()/row;

    int margin_x = (width()-cell_w*col)/2;
    int margin_y = (height()-cell_h*row)/2;
    int x = margin_x;
    int y = margin_y;
    for (int i = 0; i < views.size(); ++i){
        views[i]->hide();
    }
    for (int r = 0; r < row; ++r){
        for (int c = 0; c < col; ++c){
            int id = r*col + c + 1;
            HLayoutCell cell;
            QWidget *wdg = getPlayerByID(id);
            if (layout.getLayoutCell(id, cell)){
                wdg->setGeometry(x, y, cell_w*cell.getColspan() - CELL_BORDER, cell_h*cell.getRowspan()- CELL_BORDER);
                wdg->show();
            }
            x += cell_w;
        }
        x = margin_x;
        y += cell_h;
    }
}

void HMultiView::resizeEvent(QResizeEvent* e){
    relayout();
}

void HMultiView::mousePressEvent(QMouseEvent *e){
    ptMousePress = e->pos();
}

void HMultiView::mouseReleaseEvent(QMouseEvent *e){
    QRect rc = adjustRect(ptMousePress, e->pos());
    HVideoWidget* player1 = getPlayerByPos(rc.topLeft());
    HVideoWidget* player2 = getPlayerByPos(rc.bottomRight());
    if (player1 && player2 && player1 != player2){
        if (action == EXCHANGE){
            exchangeCells(player1, player2);
        }else if (action == MERGE){
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
    if (wdg->geometry() == rect()){
        restoreLayout();
    }else{
        saveLayout();
        for (int i = 0; i < views.size(); ++i){
            views[i]->hide();
        }
        wdg->setGeometry(rect());
        wdg->show();
    }
}

void HMultiView::saveLayout(){
    save_layout.layout = layout;
    for (int i = 0; i < views.size(); ++i){
        HVideoWidget* player = (HVideoWidget*)views[i];
        HWndInfo wnd;
        wnd.id = player->playerid;
        wnd.rc = player->geometry();
        wnd.visible = player->isVisible();
        save_layout.views.push_back(wnd);
    }
}

void HMultiView::restoreLayout(){
    layout = save_layout.layout;
    for (int i = 0; i < save_layout.views.size(); ++i){
        HWndInfo& wnd = save_layout.views[i];
        HVideoWidget* player = NULL;
        if ((player = getPlayerByID(wnd.id))){
            player->setGeometry(wnd.rc);
            player->setVisible(wnd.visible);
        }
    }
}

void HMultiView::play(HMedia& media){
    HVideoWidget* player = getIdlePlayer();
    if (player == NULL){
        QMessageBox::information(this, tr("Info"), tr("No spare player, please stop one and try agian!"));
    }else{
        player->open(media);
    }
}
