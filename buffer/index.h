#ifndef INDEX_H
#define INDEX_H

#include "../global.h"
#include "record.h"
#include "buffermanager.h"
#include "indexfile.h"

struct IndexIterator;

struct Index {
    Index(IndexFile* file, BufferManager* bm, Signature* signature, uint32 table_id, uint32 column) :
        m_file(file), m_bm(bm), m_signature(signature), m_table_id(table_id), m_column(column), m_index_filename("index_" + int_to_string(table_id) + "_" + int_to_string(column)) {}
    virtual void createIndex() = 0;
    virtual void addKey(DBDataValue key, uint32 value) = 0;
    virtual IndexIterator* findKey(DBDataValue key) = 0;
    virtual void deleteKey(DBDataValue key, uint32 value) = 0;

    IndexFile* m_file;
    BufferManager* m_bm;
    Signature* m_signature;
    uint32 m_table_id;
    uint32 m_column;
    std::string m_index_filename;
};

struct IndexIterator : IDataReader {
    IndexIterator(Index* index, DBDataValue key) : m_index(index), m_key(key) {}
    Record* getNextRecord() = 0;
    bool hasNextRecord() = 0;
    Signature* getSignature() {
        return m_index->m_signature;
    }
protected:
    Index* m_index;
    DBDataValue m_key;
};

#endif // INDEX_H
