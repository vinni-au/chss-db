#ifndef INDEXFILE_H
#define INDEXFILE_H
#include "../global.h"
//#include "buffermanager.h"
#include "record.h"

struct BufferManager;
struct BTreeVertex;

struct BTreeItem {
    int key;
    uint32 value;
    BTreeItem() {}
    BTreeItem(int _key, uint32 _value) : key(_key), value(_value) {}
    ~BTreeItem() {}
};

struct IndexFile {
public:
    IndexFile(BufferManager* bm, uint32 table_id, Signature* signature);
    void create();
    uint32 get_size() const;
    Record* get(uint32 index) const;
    void add(Record* r);
    std::string get_index_filename(uint32 column) const;
    bool exists_index(uint32 column);
    void createIndex(uint32 column);
    void addKey(uint32 column, BTreeItem& item, uint32 page);
    void findKey(uint32 column);
    void BTree_split_child(uint32, uint32, uint32);
    void BTree_insert_nonfull(uint32 column, uint32 u, BTreeItem item);
    std::pair<uint32, uint32> BTree_search(uint32 column, uint32 page, uint32 pos, uint32 key);

    static uint32 const TABLE_HEADER_SIZE = sizeof(uint32);
    static uint32 const HEADER_SIZE = 3 * sizeof(uint32) + sizeof(bool);
    static uint32 const PAGE_CAPACITY = (PAGESIZE - HEADER_SIZE) / sizeof(BTreeItem);
private:
    BufferManager* m_bm;
    uint32 m_table_id;
    std::string m_table_filename;
    Signature* m_signature;
    uint32 m_record_size;
};

struct IndexFileIterator {
    virtual Record* getNextRecord() = 0;
    virtual bool hasNextRecord() = 0;
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

#endif // INDEXFILE_H
