#include "messagedatareader.hpp"

MessageDataReader::MessageDataReader(std::string m)
    : m_hasNext(true),
      m_sig(0),
      m_msg(m)
{
    std::vector< std::string > names;
    std::vector< DBDataType > types;
    names.push_back("Message");
    types.push_back(DBDataType(DBDataType::VARCHAR, m.size()));
    m_sig = new Signature(names, types);
}

Record* MessageDataReader::getNextRecord() {
    if (m_hasNext) {
        m_hasNext = false;
        Record* result = new Record(m_sig);
        result->setVarchar(0, m_msg);
        return result;
    }

    return 0;
}
