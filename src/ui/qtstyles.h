#ifndef QT_STYLES_H
#define QT_STYLES_H

//----------------mutable-----------------------
#define LSIDE_VISIBLE       false
#define RSIDE_VISIBLE       false

#define LSIDE_MIN_WIDTH     200
#define LSIDE_MAX_WIDTH     400

#define MV_MIN_WIDTH        640
#define MV_MIN_HEIGHT       480

#define RSIDE_MIN_WIDTH     200
#define RSIDE_MAX_WIDTH     400

#define WITH_MV_STYLE       1
#define MV_STYLE_ROW        3
#define MV_STYLE_COL        3
//==============================================

#define DEFAULT_SKIN            ":/skin/dark.qss"
#define DEFAULT_PALETTE_COLOR   0xFF132D48
#define DEFAULT_LANGUAGE        ":/lang/app_zh_CN.qm"
#define DEFAULT_FONT_SIZE       12

#define MAIN_WINDOW_WIDTH   800
#define MAIN_WINDOW_HEIGHT  600

#define MASK_BG QColor(0x40, 0x40, 0x40, 0xC0)

#define VIDEO_TITLEBAR_HEIGHT           50
#define VIDEO_TITLEBAR_ICON_WIDTH       48
#define VIDEO_TITLEBAR_ICON_HEIGHT      48

#define VIDEO_TOOLBAR_HEIGHT            50
#define VIDEO_TOOLBAR_ICON_WIDTH        48
#define VIDEO_TOOLBAR_ICON_HEIGHT       48

#define DRAG_WIDTH      160
#define DRAG_HEIGHT     120

#define DRAG_QSS    "border:3px groove #FF8C00"
#define RECT_QSS    "border:2px solid red; background:transparent;"

#endif // QT_STYLES_H
