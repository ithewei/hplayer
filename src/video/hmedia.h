#ifndef H_MEDIA_H
#define H_MEDIA_H

#include <string>
#include "avdef.h"

typedef struct media_s {
    media_type_e    type;
    std::string     src;
    std::string     descr;
    int             index; // for camera index

    media_s() {
        type = MEDIA_TYPE_NONE;
        index = -1;
    }
} HMedia;

#endif // H_MEDIA_H
