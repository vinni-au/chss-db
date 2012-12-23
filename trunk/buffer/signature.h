#ifndef SIGNATURE_H
#define SIGNATURE_H
#include "global.h"
#include <vector>
#include <string>

struct Signature {
public:
    Signature(std::vector<std::string> names, std::vector<DBDataType> types);
    DBDataType get_field_type(uint32 pos);
    uint32 get_offset(uint32 pos);
private:
    uint32 m_bytes_size;
    std::vector<std::string> m_field_names;
    std::vector<DBDataType> m_fields;
    uint32* m_offsets;
};

#endif // SIGNATURE_H
