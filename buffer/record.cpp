#include "record.h"
#include <cstring>

Record::Record(Signature* signature) : m_signature(signature), m_data(new char[signature->get_size_in_bytes()]) {
    memset(m_data, 0, signature->get_size_in_bytes());}

char* Record::get_data_pointer() const {
    return m_data;
}

int Record::getInt(uint32 pos) const {
    uint32 offset = m_signature->get_offset(pos);
    return *(int*)(m_data + offset);
}

double Record::getDouble(uint32 pos) const {
    uint32 offset = m_signature->get_offset(pos);
    return *(int*)(m_data + offset);
}

std::string Record::getVarchar(uint32 pos) const {
    uint32 offset = m_signature->get_offset(pos);
    uint32 size = m_signature->get_field_type(pos).get_size();
    return std::string(m_data + offset, m_data + offset + size);
}
