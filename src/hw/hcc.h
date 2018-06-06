#ifndef H_CC_H
#define H_CC_H

#ifdef __GNUC__
    #define GNUC_ALIGN(n)   __attribute__((aligned(n))) 
#else
    #define GNUC_ALIGN(n)
#endif

#endif // H_CC_H