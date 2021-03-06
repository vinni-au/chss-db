#include "signature.h"

Signature::Signature(std::vector<std::string> names, std::vector<DBDataType> types, std::vector<std::pair<IndexType, bool> > indextypes) :
    m_field_names(names), m_fields(types), m_indextypes(indextypes), m_bytes_size(0), m_offsets(new uint32[names.size()]) {
    for(uint32 i = 0; i != names.size(); ++i) {
        m_index[names[i]] = i;
        m_fields[i] = types[i];
        m_offsets[i] = m_bytes_size;
        m_bytes_size += types[i].get_size();
    }
}

Signature::~Signature() {
    delete[] m_offsets;
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

uint32 Signature::get_size() const {
    return m_fields.size();
}

uint32 Signature::get_index(std::string const& name) {
    if(m_index.count(name))
        return m_index[name];
    return -1;
}

std::string const& Signature::get_name(uint32 pos) const {
    return m_field_names[pos];
}
