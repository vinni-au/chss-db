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
#include <cstdlib>

struct Record;
struct Signature;

int const PAGESIZE = 4096;
int const MIN_PAGES_IN_MEMORY = 32;

typedef signed char         int8;       /* 8 bit signed */
typedef unsigned char       uint8;      /* 8 bit unsigned */
typedef short               int16;      /* 16 bit signed */
typedef unsigned short      uint16;     /* 16 bit unsigned */
typedef int                 int32;      /* 32 bit signed */
typedef unsigned int        uint32;     /* 32 bit unsigned */
typedef long long           int64;      /* 64 bit signed */
typedef unsigned long long  uint64;      /* 64 bit signed */

typedef unsigned long long       pageid_t;


struct DBDataType {
    int m_type;
    int m_len;
    static int const VARCHAR = 0;
    static int const INT = 1;
    static int const DOUBLE = 2;
    DBDataType(int type, int len = 0): m_type(type), m_len(len) {
        assert(type==VARCHAR || type==INT || type==DOUBLE);
        if (type==INT || type==DOUBLE) {
            assert(len==0);
        } else
        assert(len>0 && len<=255);
    }

    DBDataType() {
        m_type = -1;
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

    void print(std::ofstream &out) const {
        out.write((char*)(&m_type), sizeof(int));
        out.write((char*)(&m_len), sizeof(int));
    }

    void read(std::ifstream &in) {
        in.read((char*)(&m_type), sizeof(int));
        in.read((char*)(&m_len), sizeof(int));
    }

    std::string name() {
        if (get_type() == INT)
            return "int";

        if (get_type() == DOUBLE)
            return "double";

        if (get_type() == VARCHAR)
            return "string";
    }

    static DBDataType construct(std::string const &type) {
        int _type;
        int _len = 0;
        if (type.substr(0, 3)=="int") {
            _type = INT;
        } else if (type.substr(0, 6)=="double") {
            _type = DOUBLE;
        } else {
            _type = VARCHAR;
            std::string t = type;
            t = t.substr(7);
            t = t.substr(1);
            t = t.substr(0, t.length()-1);
            _len = atoi(t.c_str());
        }
        return DBDataType(_type, _len);
    }

};

struct DBDataValue {
    explicit DBDataValue() {
    }

    explicit DBDataValue(int value) :
        m_int(value),
        m_type(DBDataType(DBDataType::INT))
    {   }

    explicit DBDataValue(double value) :
        m_double(value),
        m_type(DBDataType(DBDataType::DOUBLE))
    {   }

    explicit DBDataValue(std::string value) :
        m_string(value),
        m_type(DBDataType(DBDataType::VARCHAR, value.length()))
    {   }

    int intValue() const
    {   return m_int;   }

    double doubleValue() const
    {   return m_double;    }

    std::string stringValue() const
    {   return m_string;    }

    DBDataType const &type() const
    {   return m_type;  }

    bool operator < (const DBDataValue& value) const {
        if(m_type.get_type() != value.m_type.get_type())
            return false;
        if(m_type.get_type() == DBDataType::INT) {
            return m_int < value.m_int;
        }
        if(m_type.get_type() == DBDataType::DOUBLE) {
            return m_double < value.m_double;
        }
        if(m_type.get_type() == DBDataType::VARCHAR) {
            return m_string < value.m_string;
        }
    }

    bool operator > (const DBDataValue& value) const {
        return (value < *this);
    }

    bool operator == (const DBDataValue& value) const {
        if(m_type.get_type() != value.m_type.get_type())
            return false;
        if(m_type.get_type() == DBDataType::INT) {
            return m_int == value.m_int;
        }
        if(m_type.get_type() == DBDataType::DOUBLE) {
            return m_double == value.m_double;
        }
        if(m_type.get_type() == DBDataType::VARCHAR) {
            return m_string == value.m_string;
        }
    }

    bool operator != (const DBDataValue& value) const {
        return !(*this == value);
    }

    bool operator <= (const DBDataValue& value) const {
        return (*this < value) || (*this == value);
    }

    bool operator >= (const DBDataValue& value) const {
        return (*this > value) || (*this == value);
    }

protected:
    int m_int;
    double m_double;
    DBDataType m_type;
    std::string m_string;
};

struct RID {
    pageid_t pid;
    int slotNo;
    DBDataType type;
};

struct IDataReader {
    virtual bool hasNextRecord() = 0;
    virtual Signature* getSignature() = 0;
    virtual Record* getNextRecord() = 0;
    virtual ~IDataReader() {}
};

extern inline void postError(const char* who, const char* message);
std::string int_to_string(int value);
std::string get_dbfilename(int table_id);


#endif //_GLOBAL_H_
