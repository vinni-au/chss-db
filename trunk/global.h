/*
 * Global declarations for chss-db
 */
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <iostream>

#define PAGESIZE            4096
#define MIN_PAGES_IN_MEMORY 32

typedef signed char     int8;       /* 8 bit signed */
typedef unsigned char   uint8;      /* 8 bit unsigned */
typedef short           int16;      /* 16 bit signed */
typedef unsigned short  uint16;     /* 16 bit unsigned */
typedef int             int32;      /* 32 bit signed */
typedef unsigned int    uint32;     /* 32 bit unsigned */

inline void postError(const char* who, const char* message)
{
    std::cout << "ERROR in " << who << ": " << message << std::endl;
}

#endif //_GLOBAL_H_
