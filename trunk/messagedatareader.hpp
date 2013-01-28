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
        if (ololo) {
            ololo = false;
            return true;
        }
        return false;
    }

    Signature* getSignature() {
        return new Signature(std::vector<std::string>(), std::vector<DBDataType>());
    }

    Record* getNextRecord() {
/*        std::vector<std::pair<DBDataType, void*> > v;
        if (ololo) {
            std::pair<DBDataType, void*> p = std::pair<DBDataType, void*>(DBDataType::VARCHAR, (void*)msg.c_str());
            v.push_back(p);
            return v;
        }
        return v;*/
        return new Record(0);
    }

private:
    bool ololo;

    std::string msg;
};

#endif // MESSAGEDATAREADER_HPP
