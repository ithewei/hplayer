#include "HVideoWnd.h"
#include "confile.h"

HVideoWnd::HVideoWnd(QWidget *parent)
{
    fps = 0;
    framecnt = 0;
    tick = 0;
    draw_fps = g_confile->Get<bool>("draw_fps", "ui", false);
}

void HVideoWnd::calcFPS() {
    if (gettick() - tick > 1000) {
        fps = framecnt;
        framecnt = 0;
        tick = gettick();
    }
    else {
        ++framecnt;
    }
}
