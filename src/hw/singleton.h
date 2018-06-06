#ifndef SINGLETON_H
#define SINGLETON_H

#include "hdef.h"

#define DISABLE_COPY(Class) \
    Class(const Class &) = delete; \
    Class &operator=(const Class &) = delete;

#define DCLR_SINGLETON(Class) \
    public: \
        static Class* instance(); \
        static void exitInstance(); \
    private: \
        DISABLE_COPY(Class) \
        static Class* s_pInstance;

#define IMPL_SINGLETON(Class) \
    Class* Class::s_pInstance = NULL; \
    Class* Class::instance(){ \
        if (!s_pInstance){ \
            s_pInstance = new Class; \
        } \
        return s_pInstance; \
    } \
    void Class::exitInstance(){ \
        SAFE_DELETE(s_pInstance); \
    }

#endif // SINGLETON_H
