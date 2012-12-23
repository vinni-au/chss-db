#ifndef MESSAGEDATAREADER_HPP
#define MESSAGEDATAREADER_HPP

#include "global.h"
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

    std::vector<std::pair<std::string, DBDataType> > getSchema() {
        return std::vector<std::pair<std::string, DBDataType> >();
    }

    std::vector<std::pair<DBDataType, void*> > getTuple() {
        std::vector<std::pair<DBDataType, void*> > v;
        if (ololo) {
            auto p = std::pair<DBDataType, void*>(DBDataType::VARCHAR, (void*)msg.c_str());
            v.push_back(p);
            return v;
        }
        return v;
    }

private:
    bool ololo;

    std::string msg;
};

#endif // MESSAGEDATAREADER_HPP
