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
    Noindex(IndexFile* file, BufferManager* bm, Signature* signature, uint32 table_id, uint32 column) : Index(file, bm, signature, table_id, column) {};
    void createIndex();
    void addKey(int key, uint32 page);
    NoindexIterator* findKey(int key);
    void deleteKey(int key, uint32 page);
};

#endif // NOINDEX_H
