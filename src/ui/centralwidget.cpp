#include "centralwidget.h"
#include "qtstyles.h"

CentralWidget::CentralWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
    initConnect();
}

void CentralWidget::initUI(){
    ml = new HMediaList;
    mv = new HMultiView;

    QSplitter *split = new QSplitter(Qt::Horizontal);
    split->addWidget(ml);
    split->addWidget(mv);

    ml->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ml->setMinimumWidth(150);
    mv->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mv->setMinimumWidth(700);
    split->setStretchFactor(0, MEDIA_LIST_FACTOR);
    split->setStretchFactor(1, MULTI_VIEW_FACTOR);

    QHBoxLayout *hbox = genHBoxLayout();
    hbox->addWidget(split);
    setLayout(hbox);

    ml->setVisible(MEDIA_LIST_VISIBLE);
}

void CentralWidget::initConnect(){

}
