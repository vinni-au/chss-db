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
    MessageDataReader(std::string&);

    bool hasNext() {
        if (ololo) {
            ololo = false;
            return true;
        }
        return false;
    }

    Signature& getSchema() {
        return Signature(std::vector<std::string>(), std::vector<DBDataType>());
    }

    Record& getTuple() {
/*        std::vector<std::pair<DBDataType, void*> > v;
        if (ololo) {
            std::pair<DBDataType, void*> p = std::pair<DBDataType, void*>(DBDataType::VARCHAR, (void*)msg.c_str());
            v.push_back(p);
            return v;
        }
        return v;*/
        return Record(0);
    }

private:
    bool ololo;

    std::string msg;
};

#endif // MESSAGEDATAREADER_HPP
