#include "qtrcloader.h"

#define IMAGE_PATH  "./images/"

SINGLETON_IMPL(HRcLoader)

void HRcLoader::loadIcon() {
#define LOAD_ICON(rcid, filename) \
    m_mapIcon[rcid] = QPixmap(QString(IMAGE_PATH) + filename); \

    FOREACH_ICON(LOAD_ICON)
    FOREACH_ICON_MV(LOAD_ICON)

#undef LOAD_ICON
}
