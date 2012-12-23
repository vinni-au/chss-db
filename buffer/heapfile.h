#ifndef HEAPFILE_H
#define HEAPFILE_H
#include "global.h"
#include "buffermanager.h"
#include "record.h"

struct HeapFile {
public:
    HeapFile(BufferManager* bm, uint32 table_id);
    void create();
    Record get(uint32 index);
    void add(Record const& r);
private:
    BufferManager* m_bm;
    uint32 m_table_id;
    char* m_buffer;
};

#endif // HEAPFILE_H
