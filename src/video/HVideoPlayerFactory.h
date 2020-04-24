#ifndef H_VIDEO_PLAYER_FACTORY_H
#define H_VIDEO_PLAYER_FACTORY_H

#include "hmedia.h"
#include "HVideoPlayer.h"
// #include "HVideoCapture.h"
#include "hffplayer.h"

class HVideoPlayerFactory
{
public:
    static HVideoPlayer* create(media_type_e type) {
        switch (type) {
        case MEDIA_TYPE_FILE:
        case MEDIA_TYPE_NETWORK:
            return new HFFPlayer;
        case MEDIA_TYPE_CAPTURE:
            // return new HVideoCapture;
            return new HFFPlayer;
        default:
            return NULL;
        }
    }
};

#endif // H_VIDEO_PLAYER_FACTORY_H
