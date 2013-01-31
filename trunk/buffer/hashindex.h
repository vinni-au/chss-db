#ifndef HASHINDEX_H
#define HASHINDEX_H

#include "../global.h"
#include "index.h"
#include <memory.h>

struct HashIndexIterator;

struct HashIndex: Index {
    static int const BUCKETS_CNT = 1000033;
    HashIndex(IndexFile* file, BufferManager* bm, Signature* signature, uint32 table_id, uint32 column);
    virtual void createIndex() = 0;
    virtual void addKey(DBDataValue key, uint32 value) = 0;
    virtual IndexIterator* findKey(DBDataValue key) = 0;
    virtual void deleteKey(DBDataValue key, uint32 value) = 0;

    int table_size;

};

struct HashIndexIterator : IndexIterator {
    HashIndexIterator(Index* index, DBDataValue key);
    Record* getNextRecord() = 0;
    bool hasNextRecord() = 0;
    Signature* getSignature() {
        return m_index->m_signature;
    }
protected:
    Index* m_index;
    int m_key;
};

#endif // HASHINDEX_H
