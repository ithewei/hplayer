#ifndef CUSTOM_EVENT_TYPE_H_
#define CUSTOM_EVENT_TYPE_H_

// QEvent::User = 1000
// QEvent::MaxUser = 65535

class QCustomEvent {
public:
enum Type{
    User = 10000,
    OpenMediaSucceed,
    OpenMediaFailed,
    PlayerEOF,
    PlayerError,
};
};

#endif  // CUSTOM_EVENT_TYPE_H_
