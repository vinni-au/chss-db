#ifndef INDEX_H
#define INDEX_H

#include "record.h"
#include "buffermanager.h"

struct Index;

struct IndexIterator {
    IndexIterator(Index* index, int key);
    virtual Record* getNextRecord();
    virtual bool hasNextRecord();
private:
    Index* m_index;
    uint32 m_current_position;
    int m_key;
    Record* current_record;
};

struct Index {
    Index(BufferManager* bm, Signature* signature, uint32 table_id, uint32 column);
    virtual void createIndex();
    virtual void addKey(int key, uint32 page);
    virtual IndexIterator* findKey(int key);
    virtual void deleteKey(int key, uint32 page);
private:
    BufferManager* m_bm;
    Signature* m_signature;
    uint32 m_table_id;
    uint32 m_column;
};

#endif // INDEX_H
