#include "HVideoTitlebar.h"
#include "qtstyles.h"

HVideoTitlebar::HVideoTitlebar(QWidget *parent) : QFrame(parent)
{
    initUI();
    initConnect();
}

void HVideoTitlebar::initUI() {
    setFixedHeight(VIDEO_TITLEBAR_HEIGHT);

    labTitle = new QLabel;
    btnClose = genPushButton(QPixmap(":/image/close.png"), tr("close"));

    QHBoxLayout *hbox = genHBoxLayout();
    hbox->addWidget(labTitle);
    hbox->addStretch();
    hbox->addWidget(btnClose);

    setLayout(hbox);
}

void HVideoTitlebar::initConnect() {

}
