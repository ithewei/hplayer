#include "hutil.h"
#include "hdef.h"
#include <stdio.h>
#include <string.h>

static const char* s_month[] = {"January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"};

int month_atoi(const char* month){
    for (int i = 0; i < ARRAY_SIZE(s_month); ++i){
        if (strnicmp(month, s_month[i], strlen(month)) == 0)
            return i+1;
    }
    return 0;
}

const char* month_itoa(int month){
    if (month < 1 || month > 12){
        return NULL;
    }
    return s_month[month-1];
}

datetime_t get_compile_datetime(){
    static datetime_t dt;
    char month[32];
    sscanf(__DATE__, "%s %d %d", month, &dt.day, &dt.year);
    sscanf(__TIME__, "%d %d %d", &dt.hour, &dt.min, &dt.sec);
    dt.month = month_atoi(month);
    return dt;
}