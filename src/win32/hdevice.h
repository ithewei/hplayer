#ifndef H_DEVICE_H
#define H_DEVICE_H

#include <vector>

struct HDevice {
    char name[256];
    //...
};

std::vector<HDevice> getVideoDevices();
std::vector<HDevice> getAudioDevices();

#endif // H_DEVICE_H
