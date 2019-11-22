#ifndef QT_RCLOADER_H
#define QT_RCLOADER_H

#define FOREACH_ICON(F) \
    F(RC_SUBMIT,    "submit.png")   \
    F(RC_CLOSE,     "close.png")    \
    \
    F(RC_START,     "start.png")    \
    F(RC_PAUSE,     "pause.png")    \
    F(RC_STOP,      "stop.png")     \
    \
    F(RC_ADD,       "add.png")      \
    F(RC_DEL,       "del.png")      \
    F(RC_TRASH,     "trash.png")    \

#define FOREACH_ICON_MV(F) \
    F(RC_STYLE1,    "style1.png")   \
    F(RC_STYLE2,    "style2.png")   \
    F(RC_STYLE4,    "style4.png")   \
    F(RC_STYLE9,    "style9.png")   \
    F(RC_STYLE16,   "style16.png")  \
    F(RC_STYLE25,   "style25.png")  \
    F(RC_STYLE36,   "style36.png")  \
    F(RC_STYLE49,   "style49.png")  \
    F(RC_STYLE64,   "style64.png")  \

#define FOREACH_RC(F)   \
    FOREACH_ICON(F)     \
    FOREACH_ICON_MV(F)  \

#define ENUM_RCID(rcid, _) rcid,

enum RCID {
    RC_FIRST = 1,
    FOREACH_RC(ENUM_RCID)
    RC_LAST
};

#include "singleton.h"
#include <QMap>
#include <QPixmap>
class HRcLoader
{
    SINGLETON_DECL(HRcLoader)
private:
    HRcLoader() {}

public:
    void loadIcon();

    QPixmap get(RCID rcid) {
        return m_mapIcon[rcid];
    }
private:
    QMap<RCID, QPixmap> m_mapIcon;
};

#define rcloader HRcLoader::instance()

#endif // QT_RCLOADER_H
