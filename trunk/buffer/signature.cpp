#include "signature.h"

Signature::Signature(std::vector<std::string> names, std::vector<DBDataType> types) : m_field_names(names),
        m_fields(types), m_bytes_size(0), m_offsets(new uint32[names.size()]) {
    for(uint32 i = 0; i != names.size(); ++i) {
        m_fields[i] = types[i];
        m_offsets[i] = m_bytes_size;
        m_bytes_size += types[i].get_size();
    }
}

DBDataType Signature::get_field_type(uint32 pos) const {
    return m_fields[pos];
}

uint32 Signature::get_offset(uint32 pos) const {
    return m_offsets[pos];
}

uint32 Signature::get_size_in_bytes() const {
    return m_bytes_size;
}
