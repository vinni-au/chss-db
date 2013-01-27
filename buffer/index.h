#ifndef INDEX_H
#define INDEX_H

#include "record.h"
#include "buffermanager.h"

struct Index;

struct IndexIterator {
    IndexIterator(Index* index, int key) : m_index(index), m_key(key) {}
    virtual Record* getNextRecord() = 0;
    virtual bool hasNextRecord() = 0;
private:
    Index* m_index;
    int m_key;
};

struct Index {
    Index(BufferManager* bm, Signature* signature, uint32 table_id, uint32 column) :
        m_bm(bm), m_signature(signature), m_table_id(table_id), m_column(column) {}
    virtual void createIndex() = 0;
    virtual void addKey(int key, uint32 page) = 0;
    virtual IndexIterator* findKey(int key) = 0;
    virtual void deleteKey(int key, uint32 page) = 0;
private:
    BufferManager* m_bm;
    Signature* m_signature;
    uint32 m_table_id;
    uint32 m_column;
};

#endif // INDEX_H
