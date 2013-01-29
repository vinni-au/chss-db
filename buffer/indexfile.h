#ifndef INDEXFILE_H
#define INDEXFILE_H
#include "../global.h"
#include "buffermanager.h"
#include "record.h"

struct BufferManager;
struct IndexIterator;

struct IndexFile {
public:
    IndexFile(BufferManager* bm, uint32 table_id, Signature* signature);
    void create();
    uint32 get_size() const;
    Signature* get_file_signature() const;
    Record* get(uint32 index) const;
    void add(Record* r);
    void createIndex(uint32 column, uint32 index_type);
    IndexIterator* select(uint32 column, DBDataValue key, uint32 index_type);
    static uint32 const TABLE_HEADER_SIZE = sizeof(uint32);
private:
    BufferManager* m_bm;
    uint32 m_table_id;
    std::string m_table_filename;
    Signature* m_signature;
    uint32 m_record_size;
};

#endif // INDEXFILE_H
