#ifndef HASHINDEX_H
#define HASHINDEX_H

#include "../global.h"
#include "index.h"
#include <memory.h>

struct HashIndexIterator;

struct HashIndex: Index {
    static int const BUCKETS_CNT = 10000+7;
    HashIndex(IndexFile* file, BufferManager* bm, Signature* signature, uint32 table_id, uint32 column);
    virtual void createIndex();
    virtual void addKey(DBDataValue key, uint32 value);
    virtual IndexIterator* findKey(DBDataValue key);
    virtual void deleteKey(DBDataValue key, uint32 value);

    int table_size;

};

struct HashIndexIterator : IndexIterator {
    HashIndexIterator(Index* index, DBDataValue key);
    int offset1, offset2;
    Record* getNextRecord();
    bool hasNextRecord();
    Signature* getSignature() {
        return m_index->m_signature;
    }
protected:
    HashIndex* m_index;
};

#endif // HASHINDEX_H
