#include "record.h"
#include <cstring>

Record::Record(Signature* signature) : m_signature(signature), m_data(new char[signature->get_size_in_bytes()]) {
    memset(m_data, 0, signature->get_size_in_bytes());}

char* Record::get_data_pointer() const {
    return m_data;
}

void Record::setInt(uint32 pos, int value) {
    uint32 offset = m_signature->get_offset(pos);
    *(int*)(m_data + offset) = value;
}

void Record::setDouble(uint32 pos, double value) {
    uint32 offset = m_signature->get_offset(pos);
    *(double*)(m_data + offset) = value;
}

void Record::setVarchar(uint32 pos, std::string value) {
    uint32 offset = m_signature->get_offset(pos);
    memcpy(m_data + offset, value.c_str(), value.size());
}

int Record::getInt(uint32 pos) const {
    uint32 offset = m_signature->get_offset(pos);
    return *(int*)(m_data + offset);
}

double Record::getDouble(uint32 pos) const {
    uint32 offset = m_signature->get_offset(pos);
    return *(double*)(m_data + offset);
}

DBDataValue Record::get(uint32 pos) const {
    if(m_signature->get_field_type(pos).get_type() == DBDataType::INT)
        return DBDataValue(getInt(pos));
    if(m_signature->get_field_type(pos).get_type() == DBDataType::DOUBLE)
        return DBDataValue(getDouble(pos));
    if(m_signature->get_field_type(pos).get_type() == DBDataType::VARCHAR)
        return DBDataValue(getVarchar(pos));
}

std::string Record::getVarchar(uint32 pos) const {
    uint32 offset = m_signature->get_offset(pos);
    uint32 size = m_signature->get_field_type(pos).get_size();
    return std::string(m_data + offset, m_data + offset + size);
}
