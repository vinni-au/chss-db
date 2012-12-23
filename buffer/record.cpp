#include "record.h"
#include <cstring>

Record::Record(Signature* signature) : m_signature(signature) {
}

void Record::set_data(char* data, uint32 size) {
    if(m_data)
        delete[] m_data;
    m_data = new char[size];
    memcpy(m_data, data, size);
}

int Record::getInt(uint32 pos) {
    uint32 offset = m_signature->get_offset(pos);
    return *(int*)(m_data + offset);
}

double Record::getDouble(uint32 pos) {
    uint32 offset = m_signature->get_offset(pos);
    return *(int*)(m_data + offset);
}

std::string Record::getVarchar(uint32 pos) {
    uint32 offset = m_signature->get_offset(pos);
    uint32 size = m_signature->get_field_type(pos).get_size();
    return std::string(m_data + offset, m_data + offset + size);
}
