#ifndef HVIDEOPLAYERFACTORY_H
#define HVIDEOPLAYERFACTORY_H

#include "hmedia.h"
#include "HVideoPlayer.h"
#include "HVideoCapture.h"
#include "hffplayer.h"

class HVideoPlayerFactory
{
public:
    static HVideoPlayer* create(MEDIA_TYPE type) {
        switch (type) {
        case MEDIA_TYPE_FILE:
        case MEDIA_TYPE_NETWORK:
            return new HFFPlayer;
        case MEDIA_TYPE_CAPTURE:
            //return new HVideoCapture;
            return new HFFPlayer;
        default:
            return NULL;
        }
    }
};

#endif // HVIDEOPLAYERFACTORY_H
