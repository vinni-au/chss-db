#include "record.h"
#include <cstring>
#include <algorithm>

Record::Record(uint32 position, Signature* signature) : m_position(position), m_signature(signature), m_data(new char[signature->get_size_in_bytes()]) {
    memset(m_data, 0, signature->get_size_in_bytes());}

Record::~Record() {
    delete[] m_data;
}

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
    uint32 size = std::min((int)value.size(), m_signature->get_field_type(pos).get_size());
    memcpy(m_data + offset, value.c_str(), size);
    for(uint32 i = size; i < m_signature->get_field_type(pos).get_size(); ++i) {
        m_data[i] = 0;
    }
}

void Record::set(uint32 pos, DBDataValue value) {
    if(value.type().get_type() == DBDataType::INT) {
        setInt(pos, value.intValue());
    } else if(value.type().get_type() == DBDataType::DOUBLE) {
        setDouble(pos, value.doubleValue());
    } else if(value.type().get_type() == DBDataType::VARCHAR) {
        setVarchar(pos, value.stringValue());
    }
}

int Record::getInt(uint32 pos) const {
    uint32 offset = m_signature->get_offset(pos);
    return *(int*)(m_data + offset);
}

double Record::getDouble(uint32 pos) const {
    uint32 offset = m_signature->get_offset(pos);
    return *(double*)(m_data + offset);
}

uint32 Record::getPosition() const {
    return m_position;
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
    std::string res = std::string(m_data + offset, m_data + offset + size);
    while(res.size() > 0 && res[res.size() - 1] == 0)
        res = res.substr(0, res.size() - 1);
    return res;
}
