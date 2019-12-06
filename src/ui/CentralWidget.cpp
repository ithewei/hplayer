#include "CentralWidget.h"
#include "qtstyles.h"

CentralWidget::CentralWidget(QWidget *parent) : QWidget(parent)
{
    initUI();
    initConnect();
}

CentralWidget::~CentralWidget() {
    hlogd("~CentralWidget");
}

void CentralWidget::initUI() {
    lside = new LsideWidget;
    mv = new HMultiView;
    rside = new RsideWidget;

    QSplitter *split = new QSplitter(Qt::Horizontal);
    split->addWidget(lside);
    split->addWidget(mv);
    split->addWidget(rside);

    lside->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    lside->setMinimumWidth(LSIDE_MIN_WIDTH);
    lside->setMaximumWidth(LSIDE_MAX_WIDTH);

    mv->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mv->setMinimumWidth(MV_MIN_WIDTH);
    mv->setMinimumHeight(MV_MIN_HEIGHT);

    rside->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    rside->setMinimumWidth(RSIDE_MIN_WIDTH);
    rside->setMaximumWidth(RSIDE_MAX_WIDTH);

    split->setStretchFactor(0, 1);
    split->setStretchFactor(1, 1);
    split->setStretchFactor(2, 1);

    QHBoxLayout *hbox = genHBoxLayout();
    hbox->addWidget(split);
    setLayout(hbox);
}

void CentralWidget::initConnect() {

}
