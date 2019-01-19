#include "hopenmediadlg.h"
#include "hdevice.h"

FileTab::FileTab(QWidget *parent) : QWidget(parent){
    QVBoxLayout* vbox = new QVBoxLayout;

    vbox->addStretch();
    vbox->addWidget(new QLabel(tr("File:")));

    QHBoxLayout* hbox = new QHBoxLayout;
    edit = new QLineEdit;
    hbox->addWidget(edit);
    btnBrowse = new QPushButton("...");
    connect(btnBrowse, &QPushButton::clicked, this, [=](){
        QString file = QFileDialog::getOpenFileName(this, tr("Open Meida File"), "",
            "Video Files(*.3gp *.amv *.asf *.avi *.flv *.m2v *.m4v *.mkv *.mp2 *.mp4 *.mpg *.swf *.ts *.rmvb *.wmv)\n"
            "All Files(*)");
        if (!file.isEmpty()){
            edit->setText(file);
        }
    });
    hbox->addWidget(btnBrowse);

    vbox->addLayout(hbox);
    vbox->addStretch();

    setLayout(vbox);
}

NetworkTab::NetworkTab(QWidget *parent) : QWidget(parent){
    QVBoxLayout* vbox = new QVBoxLayout;

    vbox->addStretch();
    vbox->addWidget(new QLabel(tr("URL:")));

    edit = new QLineEdit;

    vbox->addWidget(edit);
    vbox->addStretch();

    setLayout(vbox);
}

CaptureTab::CaptureTab(QWidget *parent) : QWidget(parent){
    QVBoxLayout* vbox = new QVBoxLayout;

    vbox->addStretch();
    vbox->addWidget(new QLabel(tr("Device:")));

    cmb = new QComboBox;
    vector<HDevice> devs = getVideoDevices();
    for (int i = 0; i < devs.size(); ++i){
        cmb->addItem(devs[i].name);
    }

    vbox->addWidget(cmb);
    vbox->addStretch();

    setLayout(vbox);
}

HOpenMediaDlg::HOpenMediaDlg(QWidget *parent) : QDialog(parent)
{
    initUI();
    initConnect();
}

void HOpenMediaDlg::initUI(){
    setWindowTitle(tr("Open media"));
    setFixedSize(600, 300);

    QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Open | QDialogButtonBox::Cancel);
    connect(btns, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);

    tab = new QTabWidget;
    tab->addTab(new FileTab, QIcon(":/image/file.png"), tr("File"));
    tab->addTab(new NetworkTab, QIcon(":/image/network.png"), tr("Network"));
    tab->addTab(new CaptureTab, QIcon(":/image/capture.png"), tr("Capture"));

    tab->setCurrentIndex(DEFAULT_MEDIA_TYPE);

    QVBoxLayout* vbox = genVBoxLayout();
    vbox->addWidget(tab);
    vbox->addWidget(btns);

    setLayout(vbox);
}

void HOpenMediaDlg::initConnect(){

}

void HOpenMediaDlg::accept(){
    switch (tab->currentIndex()) {
    case MEDIA_TYPE_FILE:{
        FileTab* filetab = qobject_cast<FileTab*>(tab->currentWidget());
        if (filetab){
            media.type = MEDIA_TYPE_FILE;
            media.src  = qPrintable(filetab->edit->text());
        }
    }
        break;
    case MEDIA_TYPE_NETWORK:{
        NetworkTab* nettab = qobject_cast<NetworkTab*>(tab->currentWidget());
        if (nettab){
            media.type = MEDIA_TYPE_NETWORK;
            media.src  = qPrintable(nettab->edit->text());
        }
    }
        break;
    case MEDIA_TYPE_CAPTURE:{
        CaptureTab* captab = qobject_cast<CaptureTab*>(tab->currentWidget());
        if (captab){
            media.type = MEDIA_TYPE_CAPTURE;
            media.src  = qPrintable(captab->cmb->currentText());
            media.index = captab->cmb->currentIndex();
        }
    }
        break;
    default:
        break;
    }

    if ( media.type == MEDIA_TYPE_NONE ||
        (media.src.empty() && media.index < 0)) {
        QMessageBox::information(this, tr("Info"), tr("Invalid media source!"));
        return;
    }

    QDialog::accept();
}
