#ifndef BTREEINDEX_H
#define BTREEINDEX_H

#include "../global.h"
#include "index.h"

struct BTreeItem {
    uint32 value;
    DBDataValue key;
    BTreeItem() {}
    BTreeItem(uint32 _value, DBDataValue _key) : value(_value), key(_key) {}
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
    DBDataType m_type;

    BTreeVertex(BufferManager* _bm, uint32 _u, const std::string& _file, DBDataType type);
    static uint32 get_capacity(DBDataType type);
    ~BTreeVertex();
private:
    BTreeVertex(BTreeVertex& src) {}
    BTreeVertex operator = (BTreeVertex& src) {}
};

struct BTreeIterator : IndexIterator {
    BTreeIterator(Index* index, DBDataValue key, uint32 root) : IndexIterator(index, key) {
        path.push_back(std::make_pair(root, 0));
    }
    Record* getNextRecord();
    bool hasNextRecord();

private:
    Record* current_record;
    std::vector< std::pair<uint32, uint32> > path;
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
    DBDataType type;
    std::string m_btree_file;
};

#endif // BTREEINDEX_H
