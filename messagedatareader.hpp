#ifndef MESSAGEDATAREADER_HPP
#define MESSAGEDATAREADER_HPP

#include "global.h"
#include "buffer/signature.h"
#include "buffer/record.h"
#include <vector>
#include <string>
#include <utility>


struct MessageDataReader : public IDataReader
{
public:
    MessageDataReader(std::string);

    bool hasNextRecord() {
        return m_hasNext;
    }

    Signature* getSignature() {
        return m_sig;
    }

    Record* getNextRecord();

private:
    bool m_hasNext;
    Signature* m_sig;
    std::string m_msg;
};

#endif // MESSAGEDATAREADER_HPP
