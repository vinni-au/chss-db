#ifndef SELECTALLDATAREADER_H
#define SELECTALLDATAREADER_H

#include <vector>
#include <string>
#include <utility>
#include "buffer/record.h"

class SelectAllDataReader : public IDataReader
{
private:
    std::vector<Record > values;
    std::vector<std::pair<DBDataType, void*> > header;
    int idx;
public:
    SelectAllDataReader(std::vector<Record> values, std::vector<std::pair<DBDataType, void*> > header): values(), header(header), idx(0) {
        for (int i=0;i<(int)values.size();++i) {
            values.push_back(std::make_pair(values[i].);
        }
    }

    virtual bool hasNext() {
        return idx<(int)values.size();
    }

    virtual std::vector<std::pair<DBDataType, void*> > getSchema() {
        return header;
    }

    virtual std::vector<std::pair<DBDataType, void*> > getTuple() {
//        std::vector<std::pair<DBDataType, void*> > ret = std::make_pair();
        return ret;
    }
};

#endif // SELECTALLDATAREADER_H
