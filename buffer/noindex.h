#ifndef NOINDEX_H
#define NOINDEX_H

#include "index.h"

struct NoindexIterator : IndexIterator {
    NoindexIterator(Index* index, int key);
    Record* getNextRecord();
    bool hasNextRecord();

private:
    uint32 m_current_position;
    Record* current_record;
};

struct Noindex : Index {
    void createIndex();
    void addKey(int key, uint32 page);
    NoindexIterator* findKey(int key);
    void deleteKey(int key, uint32 page);
};

#endif // NOINDEX_H
