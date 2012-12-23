#ifndef SIGNATURE_H
#define SIGNATURE_H
#include "global.h"
#include <vector>

struct Signature {
public:
    Signature(char* str, uint32 size);
    char getField(uint32 pos);
    bool isInt(uint32 pos);
    bool isDouble(uint32 pos);
    bool isVarchar(uint32 pos);
private:
    std::vector<char> m_fields;
};

#endif // SIGNATURE_H
