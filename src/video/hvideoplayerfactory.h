#ifndef HVIDEOPLAYERFACTORY_H
#define HVIDEOPLAYERFACTORY_H

#include "hmedia.h"
#include "hvideoplayer.h"
#include "hvideocapture.h"

class HVideoPlayerFactory
{
public:
    static HVideoPlayer* create(MEDIA_TYPE type){
        switch (type) {
        case MEDIA_TYPE_FILE:
        case MEDIA_TYPE_NETWORK:
        case MEDIA_TYPE_CAPTURE:
            return new HVideoCapture;
        default:
            return NULL;
        }
    }
};

#endif // HVIDEOPLAYERFACTORY_H
