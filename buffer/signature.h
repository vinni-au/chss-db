#ifndef SIGNATURE_H
#define SIGNATURE_H
#include "global.h"
#include <vector>

struct Signature {
public:
    Signature(DBDataType* types, uint32 size);
    DBDataType get_field_type(uint32 pos);
    uint32 get_offset(uint32 pos);
private:
    DBDataType* m_fields;
    uint32* m_offsets;
};

#endif // SIGNATURE_H
