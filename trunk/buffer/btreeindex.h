#ifndef BTREEINDEX_H
#define BTREEINDEX_H

#include "./global.h"
#include "index.h"

struct BTreeItem {
    DBDataValue key;
    uint32 value;
    BTreeItem() {}
    BTreeItem(DBDataValue _key, uint32 _value) : key(_key), value(_value) {}
    ~BTreeItem() {}
};

struct BTreeVertex {
    uint32 root;
    uint32 total;
    uint32 size;
    bool isleaf;
    BTreeItem* data;
    BufferManager* bm;
    uint32 u;
    std::string file;

    BTreeVertex(BufferManager* _bm, uint32 _u, const std::string& _file);
    ~BTreeVertex();
private:
    BTreeVertex(BTreeVertex& src) {}
    BTreeVertex operator = (BTreeVertex& src) {}
};

struct BTreeIterator : IndexIterator {
    BTreeIterator(Index* index, DBDataValue key, uint32 root) : IndexIterator(index, key), m_current_vertex(root), m_current_position(0) {}
    Record* getNextRecord();
    bool hasNextRecord();

private:
    uint32 m_current_vertex;
    uint32 m_current_position;
    Record* current_record;
    std::vector<uint32> path;
};

struct BTreeindex : Index {
    BTreeindex(IndexFile* file, BufferManager* bm, Signature* signature, uint32 table_id, uint32 column);
    void createIndex();
    void addKey(DBDataValue key, uint32 value);
    BTreeIterator* findKey(DBDataValue key);
    void deleteKey(DBDataValue key, uint32 value);
private:
    void BTree_insert_nonfull(uint32 u, BTreeItem item);
    void BTree_split_child(uint32 u, uint32 index);
    std::pair<uint32, uint32> BTree_search(uint32 page, uint32 pos, uint32 key);
public:
    static uint32 const HEADER_SIZE = 3 * sizeof(uint32) + sizeof(bool);
    static uint32 const PAGE_CAPACITY = (PAGESIZE - HEADER_SIZE) / sizeof(BTreeItem);
private:
    std::string m_btree_file;
};

#endif // BTREEINDEX_H
