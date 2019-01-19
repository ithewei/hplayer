#ifndef HDEVICE_H
#define HDEVICE_H

#include <string>
#include <vector>
using namespace std;

struct HDevice{
    char name[256];
    //...
};

vector<HDevice> getVideoDevices();
vector<HDevice> getAudioDevices();

#endif // HDEVICE_H
