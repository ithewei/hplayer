#ifndef HCOS_H
#define HCOS_H

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <windows.h>
    #undef  WIN32_LEAN_AND_MEAN
#else
    #include <strings.h>
    #define stricmp     strcasecmp
    #define strnicmp    strncasecmp
    
    #include <unistd.h>
    #include <pthread.h>
    #include <sys/time.h>
#endif

#include <time.h>

#include "hdef.h"

#ifdef _WIN32
inline uint32 getpid(){
    return GetCurrentProcessId();
}
#endif

inline uint32 gettid(){
#ifdef _WIN32
    return GetCurrentThreadId();
#else
    return pthread_self();
#endif
}

inline void msleep(unsigned long ms){
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms*1000);
#endif
}

inline uint64 gettick(){
#ifdef _WIN32
    return GetTickCount();
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec/1000;
#endif
}

typedef struct datetime_s{
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
    int ms;
}datetime_t;

inline datetime_t get_datetime(){
    datetime_t  dt;
#ifdef _WIN32
    SYSTEMTIME tm;
	GetSystemTime(&tm);
	dt.year     = tm.wYear;
	dt.month    = tm.wMonth;
	dt.day      = tm.wDay;
	dt.hour     = tm.wHour;
	dt.min      = tm.wMinute;
	dt.sec      = tm.wSecond;
	dt.ms       = tm.wMilliseconds;
#else
    struct timeval tv;
    struct tm* tm = NULL;
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    dt.year     = tm->tm_year + 1900;
    dt.month    = tm->tm_mon  + 1;
    dt.day      = tm->tm_mday;
    dt.hour     = tm->tm_hour;
    dt.min      = tm->tm_min;
    dt.sec      = tm->tm_sec;
    dt.ms       = tv.tv_usec/1000;
#endif    
    return dt;
}

#endif // HCOS_H
