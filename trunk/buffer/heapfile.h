#ifndef HEAPFILE_H
#define HEAPFILE_H
#include "../global.h"
#include "buffermanager.h"
#include "record.h"

struct HeapFile {
public:
    HeapFile(BufferManager* bm, uint32 table_id, Signature* signature);
    void create();
    Record get(uint32 index) const;
    void add(Record const& r);
private:
    BufferManager* m_bm;
    uint32 m_table_id;
    Signature* m_signature;
    char* m_buffer;
};

#endif // HEAPFILE_H
