#include "signature.h"

Signature::Signature(DBDataType* types, uint32 size) {
    uint32 current_offset = 0;
    for(uint32 i = 0; i != size; ++i) {
        m_fields[i] = types[i];
        m_offsets[i] = current_offset;
        current_offset += types[i].get_size();
    }
}

DBDataType Signature::get_field_type(uint32 pos) {
    return m_fields[pos];
}

uint32 Signature::get_offset(uint32 pos) {
    return m_offsets[pos];
}
