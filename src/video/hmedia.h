#ifndef HMEDIA_H
#define HMEDIA_H

#include <string>

enum MEDIA_TYPE{
    MEDIA_TYPE_FILE = 0,
    MEDIA_TYPE_NETWORK,
    MEDIA_TYPE_CAPTURE,

    MEDIA_TYPE_NONE
};

#define DEFAULT_MEDIA_TYPE  MEDIA_TYPE_CAPTURE

typedef struct media_s{
    MEDIA_TYPE  type;
    int         index; // for camera index
    std::string src;
    std::string descr;

    media_s(){
        type = MEDIA_TYPE_NONE;
        index = -1;
    }
}HMedia;

#endif // HMEDIA_H
