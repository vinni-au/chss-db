/*
 * Global declarations for chss-db
 */
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>

int const PAGESIZE = 4096;
int const MIN_PAGES_IN_MEMORY = 32;

typedef signed char     int8;       /* 8 bit signed */
typedef unsigned char   uint8;      /* 8 bit unsigned */
typedef short           int16;      /* 16 bit signed */
typedef unsigned short  uint16;     /* 16 bit unsigned */
typedef int             int32;      /* 32 bit signed */
typedef unsigned int    uint32;     /* 32 bit unsigned */

typedef unsigned long long       pageid_t;


struct DBDataType {
private:
    int m_type;
    int m_len;
public:
    static int const VARCHAR = 0;
    static int const INT = 1;
    static int const DOUBLE = 2;
    DBDataType(int type, int len = 0): m_type(type), m_len(len) {
        assert(type==VARCHAR || type==INT || type==DOUBLE);
        if (type==INT || type==DOUBLE) {
            assert(len==0);
        }
        assert(len>0);
    }

    int get_type() const {
        return m_type;
    }

    int get_size() const {
        if (get_type()==INT) {
            return sizeof(int32);
        }
        if (get_type()==DOUBLE) {
            return sizeof(double);
        }
        return m_len;
    }

    void print(std::ofstream const &out) {
        out.write((char*)(&m_type), sizeof(int));
        out.write((char*)(&m_len), sizeof(int));
    }

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
