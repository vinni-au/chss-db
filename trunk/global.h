/*
 * Global declarations for chss-db
 */
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <iostream>
#include <vector>
#include <string>

int const PAGESIZE = 4096;
int const MIN_PAGES_IN_MEMORY = 32;

typedef signed char     int8;       /* 8 bit signed */
typedef unsigned char   ut8;      /* 8 bit unsigned */
typedef short           int16;      /* 16 bit signed */
typedef unsigned short  uint16;     /* 16 bit unsigned */
typedef int             int32;      /* 32 bit signed */
typedef unsigned int    uint32;     /* 32 bit unsigned */

typedef unsigned long long       pageid_t;

enum DBDataType {
    VARCHAR = 0x80,
    INT = 0x100,
    DOUBLE = 0x200
};

struct RID {
    pageid_t pid;
    int slotNo;
    DBDataType type;
};

struct IDataReader {
    virtual bool hasNext() = 0;
    virtual std::vector<std::pair<std::string, DBDataType> > getSchema() = 0;
    virtual std::vector<std::pair<DBDataType, void*> > getTuple() = 0;
};

extern inline void postError(const char* who, const char* message);

#endif //_GLOBAL_H_
