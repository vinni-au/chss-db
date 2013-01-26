#ifndef BTREE_H
#define BTREE_H
#include "indexfile.h"

struct BTreeIterator : IndexFileIterator {
    Record* getNextRecord();
    bool hasNextRecord();
private:
    uint32 column;
    uint32 page;
    uint32 pos;
    uint32 key;
    Record* current_record;
    std::vector<uint32> path;
    IndexFile* index;
};

#endif // BTREE_H
